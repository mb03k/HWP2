#include <iostream>
#include <fstream>
#include <sstream>

//#include "FileSystem/schreiben.h"
#include "getBits.h"
void startProgram();

std::string input;
void sendBits(std::string& block, unsigned int counter);
std::string calculacteCheckSum(std::string& block);

int get16hxFromFile() {
    // Pfad zur Binärdatei
    const char* filePath = "../2krandom.bin";

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
            ss << std::hex << std::uppercase << std::setw(2) << static_cast<int>(buffer[i]);
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


// Funktion zur Kodierung
std::string encodeHexStream(const std::string& block) {
    std::ostringstream encoded;
    char prevChar = '\0';  // vorheriges Zeichen, initial leer
    char escapeChar = 'X'; // Fluchtzeichen (oder beliebiges anderes)

    for (char currentChar : block) {
        // Falls das aktuelle Zeichen gleich dem vorherigen ist, Fluchtzeichen einfügen
        if (currentChar == prevChar) {
            encoded << escapeChar;
        }
        encoded << currentChar;
        prevChar = currentChar;
    }
    return encoded.str();
}

// Funktion zur Dekodierung -> für Arduino
std::string decodeHexStream(const std::string& encoded) {
    std::ostringstream decoded;
    char escapeChar = 'X'; // Das gleiche Fluchtzeichen wie in der Kodierung
    bool skipNext = false; // Flag zum Überspringen des nächsten Zeichens

    for (char currentChar : encoded) {
        if (skipNext) {
            // Überspringe das Fluchtzeichen und dekodiere das aktuelle Zeichen
            decoded << currentChar;
            skipNext = false;
        } else if (currentChar == escapeChar) {
            // Falls ein Fluchtzeichen gefunden wird, aktiviere Skip-Modus
            skipNext = true;
        } else {
            decoded << currentChar;
        }
    }
    return decoded.str();
}

// was will ich? Prüfen ob sich wirklich jeder binäre Wert ändert
// bekommt encoded string
std::string getBinary(std::string& text) {
    std::bitset<8> bs;
    std::bitset<8> oldBS;

    int counter = 1;

    for (char ch : text) {
        unsigned int n;
        std::stringstream ss;

        n = static_cast<unsigned int>(ch);

        //ss << std::hex << std::uppercase << ch;
        //ss >> n;
        bs = n; // aktueller binäre Wert

        // prüft ob sich alle binärdaten unterscheiden
        if(oldBS == bs) {
            std::cerr << "ALAAAAAAARRRRM - IMMENSER Fehler" << std::endl;
            std::cout << "'" << ch << "' - oldBS "<< oldBS << " - bs " << bs<<std::endl;
            return "";
        }

        oldBS = bs;
        counter++;
    }
    return "alles fine";
}

std::string calculateCheckSum(std::string& block) {

}

void startSending() {
    try {
        GetBits reader("../2krandom.bin");
        unsigned int counter = 1;

        std::cout << "\n\n\n"<<std::endl;

        while (true) {
            std::string block = reader.getNextBlock(); // originale Datei - nicht enkodiert

            if (block.empty()) {
                break; // Ende der Datei erreicht
            }

            std::string checkSum;
            //checkSum = calculateCheckSum(block);

            std::string block_encoded;
            block_encoded = encodeHexStream(block);

            std::cout << "BLOCK " << counter << std::endl;
            std::cout << "\t\"org: " << block << "\"" << std::endl;
            std::cout << "\t\"enk: " << block_encoded << "\"\n" << std::endl;

            sendBits(block_encoded, counter);
            counter++;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fehler: " << e.what() << std::endl;
    }
}

std::bitset<4> printHexBits(const char& hexChar) {
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

void sendBits(std::string& block, unsigned int counter) {
    for (char ch : block) {

        std::cout << "\tsend '" << ch << "' " << printHexBits(ch) << std::endl;
    }
}

int main1() {
    /*
     * FUNKTIONIERT !!!!
     * Ziel: eingelesenen Text encoden, decoden und beides ausgeben
     *
     */
    get16hxFromFile();

    // Original-String
    std::cout << "org1: " << input << std::endl;

    // Kodieren
    std::string encoded = encodeHexStream(input);
    std::cout << "kod2: " << encoded << std::endl;

    // Dekodieren
    std::string decoded = decodeHexStream(encoded);
    std::cout << "eko3: " << decoded << std::endl;

    // Prüfen ob sich alle Werte unterscheiden
    std::cout << "bin4: " << getBinary(encoded) << std::endl;

    startSending();

    return 0;
}

