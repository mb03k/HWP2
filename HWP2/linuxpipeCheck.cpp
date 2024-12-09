#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

bool isBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Fehler beim Öffnen der Datei." << std::endl;
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