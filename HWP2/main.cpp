#include <iostream>
#include <fstream>
#include <bitset>
#include <b15f/b15f.h>

#include "saveInVector.h"
#include "linuxpipeCheck.h"
#include "stringToHex.h"



void convertStringToHex(std::string& input);
void writeMSBtoRegister(char ch, int msb);
void writeLSBtoRegister(char ch, int lsb);
void sendLowCLK();
void sendBlockStartSequence();
void sendBlockEndSequence();
void sendCheckSum();
void delay100();
void sendEndOfSending();

bool arduinoSaysNextBlock();
void readPathAndStart();
void calculateCheckSum(int val);
void startSending();
void sendInnerChunk(std::string& chunk);
int calculateMSB(int val);
int calculateLSB(int val);
int calculateHexCharToInt(const char& hexChar);
int offset;
int checksum;
int counter = 1;
bool packageCorrect = true; // Startwert zum senden der Blöcke

B15F& drv = B15F::getInstance();

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

int main() {
    drv.setRegister(&DDRA, 0x0F); // Bit 0-7 Ausgabe // !!!! WICHTIG
    drv.setRegister(&PORTA, 0x00);
    /*!!!WICHTIG!!!
     * setRegister(&DDRA, value) -> der Value ist bei 0x00 zum lesen gesetzt. Jetzt kann man auch getRegister benutzen
     *                           -> wenn Value auf 0xFF ist, kann man auf die Pins schreiben
    */

    std::remove("output.bin");
    readPathAndStart();

    
    
    /*uint8_t neuerWert = 0;
    uint8_t alterWert = -1;

    while (true) {
        neuerWert = drv.getRegister(&PINA) >> 4;
        if (alterWert != neuerWert) {
            alterWert = neuerWert;
            std::cout << "Register PINA: " << std::bitset<8>(neuerWert) << std::endl;
        }
    }*/


    return 0;
}

void readPathAndStart() {
    std::string input;
    std::cout << "Text oder Datei: " << std::endl;
    std::getline(std::cin, input);
    std::cout << "Eingabe war: '" << input << "'" << std::endl;

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

    for (int i=0; i<offset;) {
        if (packageCorrect) { // NEU
            std::string chunk = getChunkAsHexString(i);
            sendInnerChunk(chunk);

            packageCorrect = arduinoSaysNextBlock(); // NEU -> liest Prüfsumme von Arduino ein

            i++; // NEU
        }
    }

    std::cout
        << "-----------------------------------------"
        << "\n"
        << "-----------------------------------------"
        << "\n"
        << "ENDE DES SENDENS"
    << std::endl;
    sendEndOfSending();
}

void sendInnerChunk(std::string& chunk) {
    std::cout
        << "\n\nChunk:\n"
        << chunk
    << std::endl;

    sendBlockStartSequence();

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
        counter++;
        sendLowCLK();
        writeLSBtoRegister(ch, lsb);
        counter++;
        sendLowCLK();

        std::cout << " ------ " << std::endl;
    }
    counter = 1;
    sendBlockEndSequence();
    sendCheckSum();
    std::cout << std::endl;
}

int calculateMSB(int val) {
    int msb = (val >> 2) & 0b11; // schiebt den int 2 stellen nach rechts -> 0b1101 wird zu 0b0011
    calculateCheckSum(msb);
    msb += 8; // für 10XX
    return msb;
}

int calculateLSB(int val) {
    int lsb = val & 0b11;
    calculateCheckSum(lsb);
    lsb += 8; // für 10XX
    return lsb;
}

void calculateCheckSum(int val) {
    int cs_val = __builtin_popcount(val);
    checksum = checksum + cs_val;
    std::cout << std::endl;
}









// ------------------------------------------------------------------------------











void writeMSBtoRegister(char ch, int msb) {
    std::cout
        << "\t\tsend '"
        << ch
        << "'  MSB: "
        << std::bitset<4>(msb)
        << " - " 
        << counter
    << std::endl;

    drv.setRegister(&PORTA, msb);
    delay100();
}

void writeLSBtoRegister(char ch, int lsb) {
    std::cout
        << "\t\tsend '"
        << ch
        << "'  LSB: "
        << std::bitset<4>(lsb)
        << " - " 
        << counter
    << std::endl;

    drv.setRegister(&PORTA, lsb);
    delay100();
}

void sendLowCLK() {
    std::cout << "\t\t0000 - CLK" << std::endl;

    drv.setRegister(&PORTA, 0x00);
    delay100();
}

void sendBlockStartSequence() {
    std::cout << "ANFANG BLOCK: 0001" << std::endl;

    drv.setRegister(&PORTA, 1);
    delay100();
}

void sendBlockEndSequence() {
    std::cout << "ENDE BLOCK: 0010" << std::endl;

    drv.setRegister(&PORTA, 2);
    delay100();
}

void delay100() {
    drv.delay_ms(10);
}

void sendEndOfSending() {
    std::cout << "3 gesendet" << std::endl;

    drv.setRegister(&PORTA, 3);
}

void sendCheckSum() {
    std::cout << "PRÜFSUMME SENDEN" << std::endl;

    std::cout
            << "Prüfsumme: "
            << checksum
            << " - "
            << std::bitset<8>(checksum)
            << std::endl;

    std::vector<int> bits;

    // Aufteilen in 4 Gruppen von 2 Bits, von MSB zu LSB
    for (int i = 3; i >= 0; i--) { // Beginne bei der höchsten Gruppe (MSB)
        int group = (checksum >> (i * 2)) & 0b11; // Extrahiere 2 Bits
        group += 8; // für CLK
        bits.push_back(group);
    }

    for (int i : bits) {
        writeMSBtoRegister('\0', i);
        sendLowCLK();
    }

    checksum = 0;
}

/*
 * Ansatz für Prüfsumme:
 * nach jedem gesendeten Block liest er für einen bestimmten Zeitraum die Werte ein
 * wenn der Arduino eine 1 zurück sendet, will er den nächsten Block
 *
 * -> kann man bestimmt auch mit delay machen - jeweils 10 ms zwei mal oder so
 */
bool arduinoSaysNextBlock() {
    drv.delay_ms(10); // delay um das Signal einzulesen

    if (drv.getRegister(&DDRA) == 1) {
        return false;
    }

    return true;
}

