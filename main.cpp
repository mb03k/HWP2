#include <iostream>
#include <fstream>
#include <sstream>

//#include "FileSystem/schreiben.h"
#include "getBits.h"
void startProgram();

std::string input;
bool CLK_HIGH = true;
void calculateAndSendBits(std::string& block, unsigned int& counter);
std::string calculacteCheckSum(std::string& block);

int get16hxFromFile() {
    // Pfad zur Binärdatei
    const std::string filePath = "../2krandom.bin";

    // Datei im Binärmodus öffnen
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filePath << std::endl;
        return 1;
    }

    // Puffer für die 16 Bytes
    const std::size_t blockSize = 16;
    unsigned char buffer[blockSize];

    std::stringstream ss;

    // Datei blockweise lesen
    while (file.read(reinterpret_cast<char*>(buffer), blockSize) || file.gcount() > 0) {
        // Anzahl der tatsächlich gelesenen Bytes
        std::size_t bytesRead = file.gcount();

        // Hexadezimale Darstellung ausgeben
        for (std::size_t i = 0; i < bytesRead; ++i) {
            ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]);
        }
    }

    input = ss.str();

    file.close();

        /*std::stringstream ss;
        for (char ch : text) {
            ss << std::hex << std::uppercase << static_cast<unsigned>(ch);
        }
        input = ss.str();*/

    return 0;
}

std::string calculateCheckSum(std::string& block) {

}

void startSending() {
    try {
        GetBits reader("../2krandom.bin");
        unsigned int counter = 1;

        std::cout << "\n"<<std::endl;

        while (true) {
            std::string block = reader.getNextBlock(); // originale Datei - nicht enkodiert

            if (block.empty()) {
                break; // Ende der Datei erreicht
            }

            std::cout << "BLOCK " << counter << std::endl;
            std::cout << "\t" << block << std::endl;


            calculateAndSendBits(block, counter);
            counter++;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fehler: " << e.what() << std::endl;
    }
}

int printHexBits(const char& hexChar) {
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

    std::cout << "\t\t\tCLK: 0000" << std::endl;
}

void calculateAndSendBits(std::string& block, unsigned int& counter) {
    for (char ch : block) {
        int bs = printHexBits(ch);

        // man kann am B15 mit Integern senden
        int msb = (bs >> 2) & 0b11;
        msb += 8; // für 10XX

        int lsb = bs & 0b11;
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

        sendLowCLK();

        //drv.setRegister(&PORTA, msb_bits);
        //drv.setRegister(&PORTA, lsb_bits);
    }
}

int main() {

    get16hxFromFile(); // nur zum ausgeben !ALLER! HEX werte

    std::cout << "org1: " << input << std::endl;

    startSending();

    return 0;
}

