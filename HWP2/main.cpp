#include <iostream>
#include <fstream>

#include "saveInVector.h"
#include "sentToRegister.h"
#include "linuxpipeCheck.h"
#include "stringToHex.h"


void startSending();
void sendInnerChunk(std::string& chunk);
int calculateMSB(int val);
int calculateLSB(int val);
int calculateHexCharToInt(const char& hexChar);

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
    }

    startSending();

    return 0;
}

void startSending() {
    //offset = getVecSize();

    //for (int i=0; i<offset; i++) {
        //std::string chunk = getChunkAsHexString(i);
        //sendInnerChunk(chunk);
    //}

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

    //sendBlockStartSequence();

    /*
     * Hauptfunktion:
     * wandelt jedes HEX Zeichen in seinen int um und sendet diesen
     * als MSB und LSB
     */
    for (char ch : chunk) {
        int innerChunkInt = calculateHexCharToInt(ch);
        int msb = calculateMSB(innerChunkInt);
        int lsb = calculateLSB(innerChunkInt);

        /*writeMSBtoRegister(ch, msb);
        sendLowCLK();
        writeLSBtoRegister(ch, lsb);*/
       // sendLowCLK();

        std::cout << " ------ " << std::endl;
    }
    //sendBlockEndSequence();
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