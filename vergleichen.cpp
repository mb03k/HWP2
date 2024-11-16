#include <iostream>
#include <fstream>
#include <vector>

bool compareBinaryFiles(const std::string& file1, const std::string& file2) {
    std::ifstream ifs1(file1, std::ios::binary);
    std::ifstream ifs2(file2, std::ios::binary);

    // Überprüfen, ob die Dateien geöffnet werden konnten
    if (!ifs1.is_open() || !ifs2.is_open()) {
        std::cerr << "Fehler beim Öffnen der Dateien.\n";
        return false;
    }

    // Zum Ende der Datei gehen, um die Größe zu ermitteln
    ifs1.seekg(0, std::ios::end);
    ifs2.seekg(0, std::ios::end);

    std::streamsize size1 = ifs1.tellg();
    std::streamsize size2 = ifs2.tellg();

    // Größen vergleichen
    if (size1 != size2) {
        return false; // Dateien haben unterschiedliche Größen
    }

    // Zurück zum Anfang der Dateien
    ifs1.seekg(0, std::ios::beg);
    ifs2.seekg(0, std::ios::beg);

    // Dateien blockweise vergleichen
    const std::size_t bufferSize = 4096;
    std::vector<char> buffer1(bufferSize);
    std::vector<char> buffer2(bufferSize);

    while (ifs1 && ifs2) {
        ifs1.read(buffer1.data(), bufferSize);
        ifs2.read(buffer2.data(), bufferSize);

        // Vergleich der gelesenen Blöcke
        if (ifs1.gcount() != ifs2.gcount() ||
            !std::equal(buffer1.begin(), buffer1.begin() + ifs1.gcount(), buffer2.begin())) {
            return false; // Unterschied gefunden
        }
    }

    return true; // Dateien sind identisch
}

int main() {
    std::string file1 = "2krandom.bin";
    std::string file2 = "2krandom_neu.bin";

    if (compareBinaryFiles(file1, file2)) {
        std::cout << "Die Dateien sind identisch.\n";
    } else {
        std::cout << "Die Dateien unterscheiden sich.\n";
    }

    return 0;
}
