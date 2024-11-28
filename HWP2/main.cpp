#include <iostream>
#include <fstream>
#include <bitset>
#include <b15f/b15f.h>

#include "saveInVector.h"
//#include "sentToRegister.h"
#include "linuxpipeCheck.h"
#include "stringToHex.h"



void convertStringToHex(std::string& input);
void writeMSBtoRegister(char ch, int msb);
void writeLSBtoRegister(char ch, int lsb);
void sendLowCLK();
void sendBlockStartSequence();
void sendBlockEndSequence();
void delay100();

void startSending();
void sendInnerChunk(std::string& chunk);
int calculateMSB(int val);
int calculateLSB(int val);
int calculateHexCharToInt(const char& hexChar);
B15F& drv = B15F::getInstance();
int offset;

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
    drv.setRegister(&DDRA, 0xFF); // Bit 0-7 Ausgabe // !!!! WICHTIG
    /*!!!WICHTIG!!!
     * setRegister(&DDRA, value) -> der Value ist bei 0x00 zum lesen gesetzt. Jetzt kann man auch getRegister benutzen
     *                           -> wenn Value auf 0xFF ist, kann man auf die Pins schreiben
     * */


    // speichert die eingangsdatei in einen Vektor

    std::string input;
    std::getline(std::cin, input);

    std::cout
        << "Eingabe war: '"
        << input << "'"
    << std::endl;

    // prüfen ob die Eingabe eine Datei oder Text ist
    if (isPath(input)) {
        saveInVector(input);
    } else {
        // String in HEX umwandeln
        std::cout << "String in HEX umwandeln" << std::endl;
        convertStringToHex(input);
        saveInVector(input);
    }

    startSending();

    return 0;
}

void startSending() {
    offset = getVecSize();

    for (int i=0; i<offset; i++) {
        std::string chunk = getChunkAsHexString(i);
        sendInnerChunk(chunk);
    }

    std::cout
        << "-----------------------------------------"
        << "\n"
        << "-----------------------------------------"
        << "\n"
        << "-----------------------------------------"
        << "\n"
        << "ENDE DES SENDENS"
    << std::endl;
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
        sendLowCLK();
        writeLSBtoRegister(ch, lsb);
        sendLowCLK();

        std::cout << " ------ " << std::endl;
    }
    sendBlockEndSequence();
    std::cout << std::endl;
}

int calculateMSB(int val) {
    int msb = (val >> 2) & 0b11; // schiebt den int 2 stellen nach rechts -> 0b1101 wird zu 0b0011
    msb += 8; // für 10XX
    return msb;
}

int calculateLSB(int val) {
    int lsb = val & 0b11;
    lsb += 8; // für 10XX
    return lsb;
}









// ------------------------------------------------------------------------------











void writeMSBtoRegister(char ch, int msb) {
    std::cout
        << "\t\tsend '"
        << ch
        << "'  MSB: "
        << std::bitset<4>(msb)
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
    << std::endl;
    drv.setRegister(&PORTA, lsb);
    delay100();
}

void sendLowCLK() {
    drv.setRegister(&PORTA, 0x00);
    delay100();

    std::cout << "\t\t0000 - CLK" << std::endl;
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
    drv.delay_ms(100);
}



