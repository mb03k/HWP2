#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

void writeHexToBinaryFile(const std::string& filename, const std::vector<std::string>& hexValues) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Fehler beim Öffnen der Datei zum Schreiben: " << filename << std::endl;
        return;
    }

    for (const auto& hexValue : hexValues) {
        // Hex-String in eine Zahl konvertieren
        unsigned int byteValue = 0;
        std::istringstream iss(hexValue);
        iss >> std::hex >> byteValue;

        // Den konvertierten Wert als einzelnes Byte in die Datei schreiben
        char byte = static_cast<char>(byteValue);
        outFile.write(&byte, sizeof(byte));
    }

    outFile.close();
    std::cout << "Hex-Werte wurden in die Datei geschrieben: " << filename << std::endl;
}

int main() {
    // Beispiel-Hex-Werte
    std::vector<std::string> hexValues;
    std::string arr[]= { "4F", "6B", "20", "23", "A1", "FF" }; // "Ok #¡ÿ"

    for (std::string st : arr) {
        hexValues.push_back(st);
    }

    std::string filename = "output.bin";
    writeHexToBinaryFile(filename, hexValues);

    return 0;
}
