#include <iostream>
#include <bitset>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <b15f/b15f.h>

#include "writeToArduino.h"


int offset = 0;
int checkSum = 0;
int LOWCLK = 0;
int BLOCK_START = 1;
int BLOCK_END = 2;
int NOW_CS = 2;
int END_CS = 3;
int LASTBLOCK = BLOCK_START;
int ENDOFSENDING_SEQUENCE = 3;

void readPathAndStart();
void startSending();
int getVecSize();
void setChunk(std::vector<std::vector<unsigned char>>& newChunk);

bool isBinaryFile(const std::string& filename);
bool isPath(const std::string& input);

void convertStringToHex(std::string & input);

int saveInVector(const std::string& filepath);
std::string getChunkAsHexString(size_t offset);

int calculateHexCharToInt(const char& hexChar);
void writeToArduino(int& val);
void sendBlockStartSequence();
void sendLastBlockSequence();
void sendInnerChunk(std::string& chunk);
void sendNowComesCheckSum();
void sendCheckSum();
void sendEndOfCheckSum();
bool arduinoSaysNextBlock();
void sendEndOfSending();
void clearBufferAndReadData();
int calculateMSB(int val);
int calculateLSB(int val);
void calculateCheckSum(int val);
void writeMSBtoRegister(char ch, int msb);
void writeLSBtoRegister(char ch, int msb);


int main() {
    std::remove("output.bin");
    readPathAndStart();
    while (true) {
        int val = 1;
        writeToArduino(val);
        val = 3;
        writeToArduino(val);
    }
    return 0;
}

void readPathAndStart() {
    std::string input;
    std::cout << "Text oder Datei: " << std::endl;
    std::getline(std::cin, input);

    // prüfen ob die Eingabe eine Datei oder Text ist
    if (isPath(input)) {
        saveInVector(input);
    } else {
        convertStringToHex(input);
        saveInVector(input);
    }

    startSending();
}

void startSending() {
    offset = getVecSize();

    for (int i=0; i<offset; ) {
        std::string chunk = getChunkAsHexString(i);
        std::cout
            << "Chunk:\n"
            << chunk
        << std::endl;

        sendBlockStartSequence();
        
        if (i == (offset-1)) {
            sendLastBlockSequence();
        }

        sendInnerChunk(chunk);

        sendNowComesCheckSum();

        sendCheckSum();

        sendEndOfCheckSum();

        if (checkSumIsFine()) {
            i++;
        } else {
            std::cout << "!!!!!!! FALSCHE PS ERKANNT !!!!!!!" << std::endl;
            usleep(2000000);
        }
    }
    sendEndOfSending();
}

void sendInnerChunk(std::string& chunk) {
    /*
     * Hauptfunktion:
     * wandelt jedes HEX Zeichen in seinen int um und sendet diesen
     * als MSB und LSB
     */
    for (char ch : chunk) {
        int innerChunkInt = calculateHexCharToInt(ch);
        int msb = calculateMSB(innerChunkInt);
        int lsb = calculateLSB(innerChunkInt);

        writeMSBtoRegister(ch, msb);
        writeLSBtoRegister(ch, lsb);

        std::cout << " ------ " << std::endl;
    }
}

int calculateMSB(int val) {
    int msb = (val >> 2) & 0b11; // schiebt den int 2 stellen nach rechts -> 0b1101 wird zu 0b0011
    calculateCheckSum(msb);
    msb += 4; // für 1XX
    return msb;
}

int calculateLSB(int val) {
    int lsb = val & 0b11;
    calculateCheckSum(lsb);
    lsb += 4; // für 1XX
    return lsb;
}

void calculateCheckSum(int val) {
    int cs_val = __builtin_popcount(val);
    checkSum += cs_val;
    std::cout << std::endl;
}

void sendBlockStartSequence() {
    std::cout << "START BLOCK " << std::bitset<4>(BLOCK_START) << std::endl;
    writeToArduino(BLOCK_START);
}

void sendNowComesCheckSum() {
    std::cout << "JETZT PS " << std::bitset<4>(NOW_CS)  << std::endl;
    writeToArduino(NOW_CS);
}

void sendEndOfCheckSum() {
    std::cout << "ENDE PS " << std::bitset<4>(END_CS) << "\n\n" << std::endl;
    writeToArduino(END_CS);
}

void sendLastBlockSequence() {
    std::cout << "LETZTER BLOCK" << std::endl;
    writeToArduino(LASTBLOCK);
}

void sendEndOfSending() {
    std::cout << "ENDE SENDEN" << std::endl;

    std::cout << "0011 gesendet" << std::endl;
    //drv.setRegister(&PORTA, 3);
    writeToArduino(ENDOFSENDING_SEQUENCE);

    std::cout << "0011 gesendet" << std::endl;
    writeToArduino(ENDOFSENDING_SEQUENCE);
}

void writeMSBtoRegister(char ch, int msb) {
    std::cout
        << "\t\tsend '"
        << ch
        << "'  MSB: "
        << std::bitset<4>(msb)
    << std::endl;
    writeToArduino(msb);
}

void writeLSBtoRegister(char ch, int lsb) {
    std::cout
        << "\t\tsend '"
        << ch
        << "'  LSB: "
        << std::bitset<4>(lsb)
    << std::endl;
    writeToArduino(lsb);
}

void sendCheckSum() {
    std::cout << "PRÜFSUMME SENDEN" << std::endl;

    std::cout
            << "Prüfsumme: "
            << (int)checkSum
            << " - "
            << std::bitset<8>(checkSum)
            << std::endl;

    std::vector<int> bits;

    // Aufteilen in 4 Gruppen von 2 Bits, von MSB zu LSB
    for (int i = 3; i >= 0; i--) { // Beginne bei der höchsten Gruppe (MSB)
        int group = (checkSum >> (i * 2)) & 0b11; // Extrahiere 2 Bits
        group += 4; // für CLK
        bits.push_back(group);
    }

    for (int i : bits) {
        writeMSBtoRegister('\0', i);
        
    }

    checkSum = 0;
}

void writeToArduino(int& val) {
    writeToB15(val);
}





int calculateHexCharToInt(const char& hexChar) {
    // Hex-Zeichen in einen Integer (0-15) umwandeln
    int value = 0;
    if (hexChar >= '0' && hexChar <= '9') {
        return (hexChar - '0');
    } else if (hexChar >= 'A' && hexChar <= 'F') {
        return (hexChar - 'A' + 10);
    } else if (hexChar >= 'a' && hexChar <= 'f') {
        return (hexChar - 'a' + 10);
    }
    return value;
}













// ----------------------------------------------
































/*
    saveInVector.cpp
*/


// Vektor für alle Blöcke
std::vector<std::vector<unsigned char>> chunk;

int saveInVector(const std::string& filepath) {
    // Datei öffnen
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        std::cout << "Fehler beim Öffnen der Datei! '" << filepath << "'" << std::endl;
        return 1;
    }

    // Buffer für 16 Bytes
    std::vector<unsigned char> buffer(16);

    // Daten lesen und speichern
    while (file.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || file.gcount() > 0) {
        // Anzahl der gelesenen Bytes
        std::streamsize bytesRead = file.gcount();

        // Neuen Block erstellen und hinzufügen
        std::vector<unsigned char> block(buffer.begin(), buffer.begin() + bytesRead);
        chunk.push_back(block);

        // Wenn die Datei zu Ende ist, wird möglicherweise ein unvollständiger Block gelesen
        if (bytesRead < buffer.size()) {
            break;
        }
    }

    file.close();

    // Ausgabe der gespeicherten Blöcke
    for (size_t i = 0; i < chunk.size(); ++i) {
        std::cout << "Block " << i + 1 << ": ";
        for (unsigned char byte : chunk[i]) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(byte) << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}

std::string getChunkAsHexString(size_t offset) {
    if (offset >= chunk.size()) {
        throw std::out_of_range("Index außerhalb des gültigen Bereichs!");
    }
    std::ostringstream hexStream;
    for (unsigned char byte : chunk[offset]) {
        hexStream
        << std::hex
        << std::setw(2)
        << std::setfill('0')
        << std::uppercase
        << static_cast<int>(byte);
    }

    return hexStream.str();
}

int getVecSize() {
    return chunk.size();
}

void setChunk(std::vector<std::vector<unsigned char>>& newChunk) {
    chunk = newChunk;
}













/*
    linuxpipeCheck.cpp
*/
bool isBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Fehler beim Öffnen der Datei." << std::endl;
        return false;
    }

    char c;
    while (file.get(c)) {
        if (!std::isprint(static_cast<unsigned char>(c)) && !std::isspace(static_cast<unsigned char>(c))) {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}

bool isPath(const std::string& input) {
    // Prüfen ob .bin vorkommt
    if (input.find(".bin") != std::string::npos) {
        return true;
    }

    // Zusätzlicher Check: Ist es eine existierende Datei/Verzeichnis?
    if (std::filesystem::exists(input)) {
        return true;
    }

    return false;
}















/*
    stringToHex.cpp
*/
void convertStringToHex(std::string & input) {
    std::vector<std::vector<unsigned char>> result;
    std::vector<unsigned char> currentBlock;

    for (size_t i = 0; i < input.size(); ++i) {
        // Füge das aktuelle Zeichen als Byte hinzu
        currentBlock.push_back(static_cast<unsigned char>(input[i]));

        // Wenn der Block voll ist, füge ihn zum Ergebnis hinzu
        if (currentBlock.size() == 16) {
            result.push_back(currentBlock);
            currentBlock.clear();
        }
    }

    // Falls noch Bytes übrig sind, füge sie als letzten Block hinzu
    if (!currentBlock.empty()) {
        result.push_back(currentBlock);
    }

    setChunk(result);
}
