#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>

//#include "FileSystem/schreiben.h"
#include "writeInBinary.h"
#include "arduinoConcept.h"

#include "getBits.h"

void calculateAndSendBits(std::string& block, unsigned int& counter);
int calculateHexCharToInt(const char& hexChar);
int cs = 0;
std::string outputFilePath = "../output.bin";

void calculateCheckSum(std::bitset<2> bs) {
    cs += (int) bs.count();
}

void sendCheckSum(int checkSum) {
    std::cout << "----------\n";
    std::cout << "\tPrüfsumme in int: " << checkSum << std::endl;

    // für beide HEX Werte aus dem int
    for (int i=0; i<2; i++) {
        std::stringstream ss;
        ss << std::hex << std::uppercase << checkSum;

        std::bitset<4> bsHEX1 = calculateHexCharToInt(ss.str()[0]);
        std::bitset<4> bsHEX2 = calculateHexCharToInt(ss.str()[1]);

        int hex1toInt_msb;
        hex1toInt_msb = calculateHexCharToInt(ss.str()[i]) >> 2 & 0b11;
        std::bitset<2> test = hex1toInt_msb;

        int hex1toInt_lsb;
        hex1toInt_lsb = calculateHexCharToInt(ss.str()[i]) & 0b11;
        std::bitset<2> test2 = hex1toInt_lsb;

        std::cout << "\taktuelle PS: " << ss.str()[i] << std::endl;
        std::cout << "\tPrüfsumme HEX in int - MSB: " << hex1toInt_msb << " (" << test << ")" << std::endl;
        std::cout << "\tPrüfsumme HEX in int - LSB: " << hex1toInt_lsb << " (" << test2 << ")" << std::endl;
        std::cout << std::endl;
    }

    std::cout << "--------------------------------------------------" << std::endl;
}

void startSending() {
    try {
        GetBits reader("../../out.bin");
        unsigned int counter = 1;

        std::cout << "\n"<<std::endl;

        while (true) {
            std::string block = reader.getNextBlock(); // originale Datei - nicht enkodiert

            if (block.empty()) {
                break; // Ende der Datei erreicht
            }

            std::cout << "----------\n"
                         "\t0001 - START"
                         "\n----------" << std::endl;

            std::cout << "BLOCK " << counter << std::endl;
            std::cout << "\t" << block << std::endl;

            calculateAndSendBits(block, counter);

            sendCheckSum(cs);

            counter++;
            cs = 0;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fehler: " << e.what() << std::endl;
    }
}

int calculateHexCharToInt(const char& hexChar) {
    // Hex-Zeichen in einen Integer (0-15) umwandeln
    int value = 0;
    if (hexChar >= '0' && hexChar <= '9') {
        return (hexChar - '0');
    } else if (hexChar >= 'A' && hexChar <= 'F') {
        return (hexChar - 'A' + 10);
    }
    return value;
}

void sendLowCLK() {
    //drv.setRegister(&PINA, 0);

    std::cout << "\t\t\t0000 - CLK" << std::endl;
}

void calculateAndSendBits(std::string& block, unsigned int& counter) {
    for (char ch : block) {
        int hexVal = calculateHexCharToInt(ch);

        // man kann am B15 mit Integern senden
        int msb = (hexVal >> 2) & 0b11; // schiebt den int 2 stellen nach rechts -> 0b1101 wird zu 0b0011
        calculateCheckSum(msb);
        int lsb = hexVal & 0b11;

        //readPorts(msb, lsb);

        msb += 8; // für 10XX
        calculateCheckSum(lsb);
        lsb += 8; // für 10XX



        std::bitset<4> msb_bits = msb;
        std::bitset<4> lsb_bits = lsb;

        std::cout
            << "\t\tsend: '"
            << ch
            << "'"
        << std::endl;

        // hier werden jeweils 2 bits effektiv gesendet
        std::cout
            << "\t\t\t"
            << "msb "
            << msb
            << " - bits "
            << msb_bits
            << "  ||  "
            << "gesendet wird: "
            //<< "10:"
            << msb_bits
        << std::endl;

        //std::cout << "\t" << msb_bits << std::endl;

        sendLowCLK();

        std::cout
            << "\t\t\t"
            << "lsb "
            << lsb
            << " - bits "
            << lsb_bits
            << "  ||  "
            << "gesendet wird: "
            //<< "10:"
            << lsb_bits
        << std::endl;

        //std::cout << "\t" << lsb_bits << std::endl;

        sendLowCLK();

        //drv.setRegister(&PORTA, msb_bits);
        //drv.setRegister(&PORTA, lsb_bits);
    }
}

int main() {

    //get16hxFromFile(); // nur zum ausgeben !ALLER! HEX werte
    //std::cout << "org1: " << input << std::endl;


    //char x = 0b01001000;
    //writeToBinaryFile(x, outputFilePath);
    //std::cout<< x << std::endl;

    //startSending();



    int neuerWert = 0;
    int links = 1;
    int rechts = 0;

    neuerWert = (links << 1) | (rechts);

    std::cout << neuerWert << std::endl;



    return 0;
}

bool arduinoSaysNextBlock() {
    /*
     * Idee:
     * Arduino sendet eine 1 als startsignal
     * wenn er eine weitere sendet, ist das Paket richtig angekommen
     * wenn nicht -> neuen senden
     */

    // bool nextBlock = drv.getRegister(&PINB); // oder so
    bool nextBlock = true;

    bool aktuellerWert = false;

    while (1) {
        // aktuellerWert = drv.getRegister(&PINB);
        if (aktuellerWert) { // erste 1 wurde gelesen
            aktuellerWert = false;
            while (1) {
                // aktuellerWert = drv.getRegister(&PINB);
                aktuellerWert = false;
                if (aktuellerWert) {
                    // nächsten Block senden
                }

                // Abbruchbedingung?
            }
        }
    }
}
