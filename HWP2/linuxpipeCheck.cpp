#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iomanip>

bool isBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Fehler beim Öffnen der Datei." << std::endl;
        //std::cout << "Current path is " << std::filesystem::current_path();
        return false;
    }

    char c;
    while (file.get(c)) {
        if (!std::isprint(static_cast<unsigned char>(c)) && !std::isspace(static_cast<unsigned char>(c))) {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}

bool isPath(const std::string& input) {
    // Prüfen ob .bin vorkommt
    if (input.find(".bin") != std::string::npos) {
        return true;
    }

    // Zusätzlicher Check: Ist es eine existierende Datei/Verzeichnis?
    if (std::filesystem::exists(input)) {
        return true;
    }

    return false;
}

void convertToHEX(std::string eingabe) {
    // Buffer für Eingabe
    char buffer[1024];
    std::ostringstream hexOutput;

    // Lesen von stdin (Pipedaten)
    while (std::cin.read(buffer, sizeof(buffer)) || std::cin.gcount() > 0) {
        std::streamsize bytesRead = std::cin.gcount();

        // Konvertiere jeden Byte in Hex
        for (std::streamsize i = 0; i < bytesRead; ++i) {
            hexOutput << std::hex << std::setfill('0') << std::setw(2)
                      << static_cast<unsigned int>(static_cast<unsigned char>(buffer[i]));
        }
    }

    // Schreibe Hex-Darstellung in die Ausgabe
    std::cout << hexOutput.str() << std::endl;
}