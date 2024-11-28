#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

int BLOCK_SIZE = 8;

class GetBits {
public:
    GetBits(const std::string& filePath);

    ~GetBits();

    // Gibt die nächsten 16 Bytes als hexadezimale Zeichenkette zurück
    std::string getNextBlock();

private:
    std::ifstream file;
    const std::size_t blockSize;
};

GetBits::GetBits(const std::string& filePath) : file(filePath, std::ios::binary), blockSize(BLOCK_SIZE) {
    if (!file) {
        throw std::runtime_error("Fehler beim Öffnen der Datei: " + filePath);
    }
}

GetBits::~GetBits() {
    if (file.is_open()) {
        file.close();
    }
}

// gibt jeweils 16 HEX Zeichen zurück
std::string GetBits::getNextBlock() {
    unsigned char buffer[blockSize];
    if (file.read(reinterpret_cast<char*>(buffer), blockSize) || file.gcount() > 0) {
        std::size_t bytesRead = file.gcount();
        std::stringstream ss;

        for (std::size_t i = 0; i < bytesRead; ++i) {
            ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
               << static_cast<int>(buffer[i]);
        }

        return ss.str();
    }
    return ""; // Leerstring signalisiert das Ende der Datei
}

