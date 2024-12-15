#include <iostream>
#include <bitset>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <cstdint>
#include <vector>

#include "writeToArduino.h"

void writeToArduino(int& val);
void sendBlockStartSequence();
void sendNowComescheckSum();
void sendcheckSum();
void delay100();
void sendEndOfSending();
void sendLastBlockSequence();
void sendEndOfcheckSum();
void clearLEDs();

bool arduinoSaysNextBlock();
void readInputAndStart();
void calculatecheckSum(unsigned char& val);
void startSending();
void sendInnerChunk();
int calculateMSB1(unsigned char val);
int calculateMSB2(unsigned char val);
int calculateLSB3(unsigned char val);
int calculateLSB4(unsigned char val);
int calculateHexCharToInt(const char& hexChar);
void terminalMessageOfSending(int sendVal);


int offset;
int checkSum;
int counter = 1;
bool packageCorrect = true; // Startwert zum senden der Blöcke

std::vector<unsigned char> chunk;

int LOW_CLK = 0;
int BLOCK_START = 1;
int NOW_CS = 2;
int END_CS = 3;
int DELAYMS = 1;
int LASTBLOCK = BLOCK_START;
bool SENDCHUNK = true;
bool SEND_LAST_BLOCK = false;


int main() {
    std::remove("output.bin");
    readInputAndStart();
    return 0;
}

void readInputAndStart() {
    char byte;
    int counter = 0;
    while (std::cin.read(&byte, 1)) {
        counter++;
        chunk.push_back(byte);

        // ein chunk mit 16 zeichen gefüllt
        if (counter % 16 == 0) {
            counter = 0;
            // es kommen keine weiteren Werte

            if (std::cin.peek() == EOF) {
                std::cout << "LETZTER BLOCK!!!! IN PEEK" << std::endl;
                SEND_LAST_BLOCK = true;
            }
            startSending();
            chunk.clear();
            SENDCHUNK = true;
            SEND_LAST_BLOCK = false;
        }
    }

    if (checkSumIsFine()) {
        SEND_LAST_BLOCK = true;
        std::cout << "LETZTER BLOCK SEQUENCE SENDEN NACH WHILE" << std::endl;
        startSending();
    }
    sendEndOfSending();
}

void startSending() {
    while (SENDCHUNK) {
        std::cout << "\n\nChunk:\n'";
        for (unsigned char ch: chunk) {
            // in HEX ausgeben
            std::stringstream ss;
            ss << std::hex << std::uppercase << ((ch >> 4) & 0b1111);

            std::stringstream ss1;
            ss1 << std::hex << std::uppercase << (ch & 0b1111);

            std::cout << ss.str() << ss1.str() << " ";
        }
        std::cout << "'" << std::endl;

        if (SEND_LAST_BLOCK) {
            sendLastBlockSequence();
        }
        sendBlockStartSequence();
        sendInnerChunk();
        sendNowComescheckSum();
        sendcheckSum();
        sendEndOfcheckSum();

        if (checkSumIsFine()) {
            SENDCHUNK = false;
        } else {
            std::cout << "!!!!!!! FALSCHE PS ERKANNT !!!!!!!" << std::endl;
            SENDCHUNK = true;
            //drv.delay_ms(2000);
        }
        counter = 1;
    }
}

void sendInnerChunk() {
    // unsigned chars direkt in deren Bitwerte umwandeln
    // bsp.: 'H' -> 01001000
    // -> MSB1: 01
    // -> MSB2: 00
    // -> LSB1: 10
    // -> LSB2: 00
    for (unsigned char ch : chunk) {
        std::cout << "\t\tBYTE SENDEN: '" << ch << "' " << std::bitset<8>(ch) << std::endl;

        // MSB1
        int msb1 = calculateMSB1(ch);
        terminalMessageOfSending(msb1);
        writeToArduino(msb1);
        counter++;

        // MSB2
        int msb2 = calculateMSB2(ch);
        terminalMessageOfSending(msb2);
        writeToArduino(msb2);
        counter++;

        // LSB1
        int lsb3 = calculateLSB3(ch);
        terminalMessageOfSending(lsb3);
        writeToArduino(lsb3);
        counter++;

        // LSB2
        int lsb4 = calculateLSB4(ch);
        terminalMessageOfSending(lsb4);
        writeToArduino(lsb4);
        counter++;

        std::cout << std::endl;
        calculatecheckSum(ch);
    }
    std::cout << " ------ " << std::endl;
}

int calculateMSB1(unsigned char val) {
    int msb = (val >> 6) & 0b11;
    msb += 4; // für 1XX (CLK)
    return msb;
}

int calculateMSB2(unsigned char val) {
    int msb = (val >> 4) & 0b11;
    msb += 4; // für 1XX (CLK)
    return msb;
}

int calculateLSB3(unsigned char val) {
    int lsb = (val >> 2) & 0b11;
    lsb += 4; // für 1XX (CLK)
    return lsb;
}

int calculateLSB4(unsigned char val) {
    int lsb = val & 0b11;
    lsb += 4; // für 1XX (CLK)
    return lsb;
}

void calculatecheckSum(unsigned char& val) {
    int cs_val = __builtin_popcount(val);
    checkSum = checkSum + cs_val;
    std::cout << std::endl;
}

void terminalMessageOfSending(int sendVal) {
    std::cout
        << "\t\tsend '"
        << sendVal
        << "' bits: "
        << std::bitset<4>(sendVal)
        << " ("
        << counter
        << ")"
    << std::endl;
}

void sendBlockStartSequence() {
    std::cout << "START BLOCK " << std::bitset<4>(BLOCK_START) << std::endl;
    writeToArduino(BLOCK_START);
}

void sendNowComescheckSum() {
    std::cout << "JETZT PS " << std::bitset<4>(NOW_CS)  << std::endl;
    writeToArduino(NOW_CS);
}

void sendEndOfcheckSum() {
    std::cout << "ENDE PS " << std::bitset<4>(END_CS) << "\n\n" << std::endl;
    writeToArduino(END_CS);
}

void sendLastBlockSequence() {
    std::cout << "0001 LETZTER BLOCK" << std::endl;
    writeToArduino(LASTBLOCK);
}

void sendEndOfSending() {
    std::cout << "ENDE SENDEN" << std::endl;

    std::cout << "0011 gesendet" << std::endl;
    writeToArduino(END_CS);

    std::cout << "0011 gesendet" << std::endl;
    writeToArduino(END_CS);
}

void sendcheckSum() {
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
        std::cout << "\t\tPS send '" << i << "' bits: " << std::bitset<4>(i) << std::endl;
        writeToArduino(i);
    }

    checkSum = 0;
}

void writeToArduino(int& val) {
    writeToB15(val);
}
