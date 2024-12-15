#include <iostream>
#include <fstream>
#include <bitset>
#include <cstdint>
#include <vector>
#include <sstream>

#include <unistd.h>

#include "senden_B15.h"

void writeToRegister(int val);
void sendLowCLK();
void sendBlockStartSequence();
void sendNowComescheckSumS();
void sendcheckSumS();
void delay100();
void sendEndOfSending();
void sendLASTBLOCK_SSequence();
void sendEndOfcheckSumS();
void clearLEDs();

bool arduinoSaysNextBlock();
void readInputAndStart();
void calculatecheckSumS(unsigned char& val);
void startSending();
void sendInnerchunkS();
int calculateMSB1(unsigned char val);
int calculateMSB2(unsigned char val);
int calculateLSB3(unsigned char val);
int calculateLSB4(unsigned char val);
int calculateHexCharToInt(const char& hexChar);
void terminalMessageOfSending(int sendVal);


int checkSumS;
int counterS = 1;
std::vector<unsigned char> chunkS;

int BLOCK_START_S = 1;
int NOW_CS = 2;
int END_CS = 3;
int DELAYMS = 1;
int LASTBLOCK_S = BLOCK_START_S;
bool SENDchunkS = true;

//B15F& drv = B15F::getInstance();

void init_sending() {
    /*!!!WICHTIG!!!
     * setRegister(&DDRA, value) -> der Value ist bei 0x00 zum lesen gesetzt. Jetzt kann man auch getRegister benutzen
     *                           -> wenn Value auf 0xFF ist, kann man auf die Pins schreiben
    */

    //drv.setRegister(&DDRA, 0x47);
    clearLEDs();

    std::remove("output.bin");
    readInputAndStart();

    clearLEDs();
}

void readInputAndStart() {
    /*
     * Idee: könnte man mit einem Buffer effizienter schreiben

        char buffer[1024];

        while (std::cin.read(buffer, sizeof(buffer))) {
            std::cout.write(buffer, std::cin.gcount());
        }
     */

    char byte;
    int counterS = 0;
    while (std::cin.read(&byte, 1)) {
        counterS++;

        chunkS.push_back(byte);
        // ein chunkS mit 16 zeichen gefüllt
        if (counterS % 16 == 0) {
            counterS = 0;
            // es kommen keine weiteren Werte
            if (std::cin.peek() == EOF) {
                sendLASTBLOCK_SSequence();
            }
            startSending();
            chunkS.clear();
            SENDchunkS = true;

        }
    }

    if (counterS % 16 != 0) {
        sendLASTBLOCK_SSequence();
        startSending();
    }
    sendEndOfSending();
}

void startSending() {
    while (SENDchunkS) {
        std::cout << "\n\nchunkS:\n'";
        for (unsigned char ch: chunkS) {
            // in HEX ausgeben
            std::stringstream ss;
            ss << std::hex << std::uppercase << ((ch >> 4) & 0b1111);

            std::stringstream ss1;
            ss1 << std::hex << std::uppercase << (ch & 0b1111);

            std::cout << ss.str() << ss1.str() << " ";
        }
        std::cout << "'" << std::endl;

        sendBlockStartSequence();
        sendInnerchunkS();
        sendNowComescheckSumS();
        sendcheckSumS();
        sendEndOfcheckSumS();

        if (arduinoSaysNextBlock()) {
            SENDchunkS = false;
        } else {
            std::cout << "!!!!!!! FALSCHE PS ERKANNT !!!!!!!" << std::endl;
            SENDchunkS = true;
            //drv.delay_ms(2000);
        }
        counterS = 1;
    }
}


void sendInnerchunkS() {
    // unsigned chars direkt in deren Bitwerte umwandeln
    // bsp.: 'H' -> 01001000
    // -> MSB1: 01
    // -> MSB2: 00
    // -> LSB1: 10
    // -> LSB2: 00
    for (unsigned char ch : chunkS) {
        usleep(1000 * 10);
        std::cout << "\t\tBYTE SENDEN: '" << ch << "' " << std::bitset<8>(ch) << std::endl;

        // MSB1
        int msb1 = calculateMSB1(ch);
        terminalMessageOfSending(msb1);
        writeToRegister(msb1);
        counterS++;

        // MSB2
        int msb2 = calculateMSB2(ch);
        terminalMessageOfSending(msb2);
        writeToRegister(msb2);
        counterS++;

        // LSB1
        int lsb3 = calculateLSB3(ch);
        terminalMessageOfSending(lsb3);
        writeToRegister(lsb3);
        counterS++;

        // LSB2
        int lsb4 = calculateLSB4(ch);
        terminalMessageOfSending(lsb4);
        writeToRegister(lsb4);
        counterS++;

        std::cout << std::endl;
        calculatecheckSumS(ch);
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

void calculatecheckSumS(unsigned char& val) {
    int cs_val = __builtin_popcount(val);
    checkSumS = checkSumS + cs_val;
    std::cout << std::endl;
}

void terminalMessageOfSending(int sendVal) {
    usleep(1000 * 10);

    std::cout
            << "\t\tsend '"
            << sendVal
            << "' bits: "
            << std::bitset<4>(sendVal)
            << " ("
            << counterS
            << ")"
            << std::endl;
}







void delay100() {
    //drv.delay_ms(DELAYMS);
}

void sendLowCLK() {
    //delay100();
    //writeToRegister(0);
    //delay100();
}

void sendBlockStartSequence() {
    usleep(1000 * 10);
    std::cout << "\tSTART BLOCK " << std::bitset<4>(BLOCK_START_S) << std::endl;
    writeToRegister(BLOCK_START_S);
}

void sendNowComescheckSumS() {
    usleep(1000 * 10);
    std::cout << "\tJETZT PS " << std::bitset<4>(NOW_CS)  << std::endl;
    writeToRegister(NOW_CS);
}

void sendEndOfcheckSumS() {
    usleep(1000 * 10);
    std::cout << "\tENDE PS " << std::bitset<4>(END_CS)  << std::endl;
    writeToRegister(END_CS);
}

void sendLASTBLOCK_SSequence() {
    usleep(1000 * 10);
    std::cout << "\tLETZTER BLOCK " << std::bitset<4>(LASTBLOCK_S) << std::endl;
    writeToRegister(LASTBLOCK_S);
}

void sendEndOfSending() {
    usleep(1000 * 10);
    std::cout << "\t0011 gesendet" << std::endl;
    writeToRegister(END_CS);

    usleep(1000 * 10);
    std::cout << "\t0011 gesendet" << std::endl;
    writeToRegister(END_CS);

    usleep(1000 * 10);
    std::cout << "*** ENDE SENDEN *** " << std::endl;
}

void clearLEDs() {
    //drv.setRegister(&PORTA, 0);
}

void writeToRegister(int val) {
    //drv.setRegister(&PORTA, val);
    //drv.setRegister(&PORTA, 0); // low CLK
}

void sendcheckSumS() {
    usleep(1000 * 10);
    std::cout
            << "\t\tPrüfsumme: "
            << (int)checkSumS
            << " - "
            << std::bitset<8>(checkSumS)
            << std::endl;

    std::vector<int> bits;

    // Aufteilen in 4 Gruppen von 2 Bits, von MSB zu LSB
    for (int i = 3; i >= 0; i--) { // Beginne bei der höchsten Gruppe (MSB)
        int group = (checkSumS >> (i * 2)) & 0b11; // Extrahiere 2 Bits
        group += 4; // für CLK
        bits.push_back(group);
    }

    for (int i : bits) {
        usleep(1000 * 10);
        std::cout << "\t\tPS send '" << i << "' bits: " << std::bitset<4>(i) << std::endl;
        writeToRegister(i);
        sendLowCLK();
    }

    checkSumS = 0;
}

bool arduinoSaysNextBlock() {
    /*int rg = (drv.getRegister(&PINA) & 0b10000000);

    if (__builtin_popcount(rg) >= 1) {
        std::cout << "\t\t FEHLER ERKANNT: " << std::bitset<8>(rg) << std::endl;
        return false;
    }*/

    return true;
}
