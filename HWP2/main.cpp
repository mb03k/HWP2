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
void sendNowComesCheckSum();
void sendCheckSum();
void delay100();
void sendEndOfSending();
void sendLastBlockSequence();
void sendEndOfCheckSum();

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

int LOW_CLK = 0;
int BLOCK_START = 1;
int NOW_CS = 2;
int END_CS = 3;
int DELAYMS = 1;
int LASTBLOCK = BLOCK_START;

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
    //drv.setRegister(&DDRA, 0x0F); // Bit 0-7 Ausgabe // !!!! WICHTIG    
    /*!!!WICHTIG!!!
     * setRegister(&DDRA, value) -> der Value ist bei 0x00 zum lesen gesetzt. Jetzt kann man auch getRegister benutzen
     *                           -> wenn Value auf 0xFF ist, kann man auf die Pins schreiben
    */

    drv.setRegister(&DDRA, 0x47);
    drv.setRegister(&PORTA, 0x00);
    // aktueller Test:
        // PINS so anpassen, dass wir auf welchen lesen können und auf den anderen schreiben
        // -> 0b01000111
        // 7: für empfangene Daten PS schreiben
        // 6: für geschriebene Daten PS empfangen
        // 5, 4, 3: Daten von anderen PC empfangen 
        // 2, 1, 0: Daten an anderen PC senden
        //  (von MSB zu LSB)
        //  1: schreiben, 0: lesen

    std::remove("output.bin");
    readPathAndStart();

    drv.setRegister(&PORTA, 0x00);
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

    for (int i=0; i<offset;) {
        std::string chunk = getChunkAsHexString(i);
        std::cout
            << "\n\nChunk:\n"
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

        if (arduinoSaysNextBlock()) {
            i++;
        } else {
            std::cout << "!!!!!!! FALSCHE PS ERKANNT !!!!!!!" << std::endl;
            drv.delay_ms(2000);
        }
    }
    sendEndOfSending();
}

void delay100() {
    drv.delay_ms(DELAYMS);
}

void sendLowCLK() {
    //delay100();
    drv.setRegister(&PORTA, 0);
    //delay100();
}

void sendBlockStartSequence() {
    std::cout << "START BLOCK " << std::bitset<4>(BLOCK_START) << std::endl;
    drv.setRegister(&PORTA, BLOCK_START);
    sendLowCLK();
}

void sendNowComesCheckSum() {
    std::cout << "JETZT PS " << std::bitset<4>(NOW_CS)  << std::endl;
    drv.setRegister(&PORTA, NOW_CS);
    sendLowCLK();
}

void sendEndOfCheckSum() {
    std::cout << "ENDE PS " << std::bitset<4>(END_CS)  << std::endl;
    drv.setRegister(&PORTA, END_CS);
    sendLowCLK();
}

void sendLastBlockSequence() {
    std::cout << "LETZTER BLOCK" << std::endl;
    drv.setRegister(&PORTA, LASTBLOCK);
    sendLowCLK();
}

void sendEndOfSending() {
    std::cout << "ENDE SENDEN" << std::endl;

    std::cout << "0011 gesendet" << std::endl;
    drv.setRegister(&PORTA, 3);
    sendLowCLK();   

    std::cout << "0011 gesendet" << std::endl;
    drv.setRegister(&PORTA, 3);
    sendLowCLK();
}

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
    sendLowCLK();
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
    sendLowCLK();
}

void sendCheckSum() {
    std::cout << "PRÜFSUMME SENDEN" << std::endl;

    std::cout
            << "Prüfsumme: "
            << (int)checksum
            << " - "
            << std::bitset<8>(checksum)
            << std::endl;

    std::vector<int> bits;

    // Aufteilen in 4 Gruppen von 2 Bits, von MSB zu LSB
    for (int i = 3; i >= 0; i--) { // Beginne bei der höchsten Gruppe (MSB)
        int group = (checksum >> (i * 2)) & 0b11; // Extrahiere 2 Bits
        group += 4; // für CLK
        bits.push_back(group);
    }

    for (int i : bits) {
        writeMSBtoRegister('\0', i);
        sendLowCLK();
    }

    checksum = 0;
}

bool arduinoSaysNextBlock() {
    //for (int i=0; i<50; i++) {
        int rg = (drv.getRegister(&PINA) & 0b10000000);

        if (__builtin_popcount(rg) >= 1) {
            std::cout << "\t\t FEHLER ERKANNT: " << std::bitset<8>(rg) << std::endl; 
            return false;
        }
    //}

    return true;
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
        counter++;

        writeLSBtoRegister(ch, lsb);
        counter++;

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
    checksum = checksum + cs_val;
    std::cout << std::endl;
}
