#include <iostream>
#include <fstream>
#include <vector>

int writeToBinaryFile(char val, std::string& filePath) {
    // Datei im Binär- und Anhängemodus öffnen
    std::ofstream outfile(filePath, std::ios::binary | std::ios::app);
    if (!outfile) {
        std::cerr << "Fehler: Datei konnte nicht geöffnet werden!" << std::endl;
        return 1;
    }

    system("pwd");

    // Werte anhängen
    outfile.write(&val, sizeof(val));

    // Datei schließen
    outfile.close();

    std::cout << "Werte wurden an 'output.bin' angehängt!" << std::endl;

    return 0;
}

