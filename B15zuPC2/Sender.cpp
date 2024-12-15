#include <iostream>
#include <fstream>
#include <bitset>
#include <cstdint>
#include <vector>
#include <sstream>
#include <b15f/b15f.h>

void writeToRegister(int val);
void sendLowCLK();
void sendBlockStartSequence();
void sendNowComesCheckSum();
void sendCheckSum();
void delay100();
void sendEndOfSending();
void sendLastBlockSequence();
void sendEndOfCheckSum();
void clearLEDs();

bool arduinoSaysNextBlock();
void readInputAndStart();
void calculateCheckSum(unsigned char& val);
void startSending();
void sendInnerChunk();
int calculateMSB1(unsigned char val);
int calculateMSB2(unsigned char val);
int calculateLSB3(unsigned char val);
int calculateLSB4(unsigned char val);
int calculateHexCharToInt(const char& hexChar);
void terminalMessageOfSending(int sendVal);

int offset;
int checksum;
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

B15F& drv = B15F::getInstance();

int main() {
    //drv.setRegister(&DDRA, 0x0F); // Bit 0-7 Ausgabe // !!!! WICHTIG    
    /*!!!WICHTIG!!!
     * setRegister(&DDRA, value) -> der Value ist bei 0x00 zum lesen gesetzt. Jetzt kann man auch getRegister benutzen
     *                           -> wenn Value auf 0xFF ist, kann man auf die Pins schreiben
    */

    drv.setRegister(&DDRA, 0x47);
    // aktueller Test:
        // PINS so anpassen, dass wir auf welchen lesen können und auf den anderen schreiben
        // -> 0b01000111
        // 7: für empfangene Daten PS schreiben
        // 6: für geschriebene Daten PS empfangen
        // 5, 4, 3: Daten von anderen PC empfangen 
        // 2, 1, 0: Daten an anderen PC senden
        //  (von MSB zu LSB)
        //  1: schreiben, 0: lesen

    clearLEDs();

    std::remove("output.bin");
    readInputAndStart();

    clearLEDs();
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
                sendLastBlockSequence();
            }
            startSending();
            chunk.clear();
            SENDCHUNK = true;

        }
    }

    if (counter % 16 != 0) {
        sendLastBlockSequence();
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

        sendBlockStartSequence();
        sendInnerChunk();
        sendNowComesCheckSum();
        sendCheckSum();
        sendEndOfCheckSum();

        if (arduinoSaysNextBlock()) {
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
        writeToRegister(msb1);
        counter++;

        // MSB2
        int msb2 = calculateMSB2(ch);
        terminalMessageOfSending(msb2);
        writeToRegister(msb2);
        counter++;

        // LSB1
        int lsb3 = calculateLSB3(ch);
        terminalMessageOfSending(lsb3);
        writeToRegister(lsb3);
        counter++;

        // LSB2
        int lsb4 = calculateLSB4(ch);
        terminalMessageOfSending(lsb4);
        writeToRegister(lsb4);
        counter++;

        std::cout << std::endl;
        calculateCheckSum(ch);
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

void calculateCheckSum(unsigned char& val) {
    int cs_val = __builtin_popcount(val);
    checksum = checksum + cs_val;
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







void delay100() {
    //drv.delay_ms(DELAYMS);
}

void sendLowCLK() {
    //delay100();
    //writeToRegister(0);
    //delay100();
}

void sendBlockStartSequence() {
    std::cout << "\tSTART BLOCK " << std::bitset<4>(BLOCK_START) << std::endl;
    writeToRegister(BLOCK_START);
}

void sendNowComesCheckSum() {
    std::cout << "\tJETZT PS " << std::bitset<4>(NOW_CS)  << std::endl;
    writeToRegister(NOW_CS);
}

void sendEndOfCheckSum() {
    std::cout << "\tENDE PS " << std::bitset<4>(END_CS)  << std::endl;
    writeToRegister(END_CS);
}

void sendLastBlockSequence() {
    std::cout << "\tLETZTER BLOCK " << std::bitset<4>(LASTBLOCK) << std::endl;
    writeToRegister(LASTBLOCK);
}

void sendEndOfSending() {
    std::cout << "\t0011 gesendet" << std::endl;
    writeToRegister(END_CS);

    std::cout << "\t0011 gesendet" << std::endl;
    writeToRegister(END_CS);

    std::cout << "*** ENDE SENDEN *** " << std::endl;
}

void clearLEDs() {
    drv.setRegister(&PORTA, 0);
}

void writeToRegister(int val) {
    drv.setRegister(&PORTA, val);
    drv.setRegister(&PORTA, 0); // low CLK
}

void sendCheckSum() {
    std::cout
            << "\t\tPrüfsumme: "
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
        std::cout << "\t\tPS send '" << i << "' bits: " << std::bitset<4>(i) << std::endl;
        writeToRegister(i);
        sendLowCLK();
    }

    checksum = 0;
}

bool arduinoSaysNextBlock() {
    int rg = (drv.getRegister(&PINA) & 0b10000000);

    if (__builtin_popcount(rg) >= 1) {
        std::cout << "\t\t FEHLER ERKANNT: " << std::bitset<8>(rg) << std::endl;
        return false;
    }

    return true;
}
