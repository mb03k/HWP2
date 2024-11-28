#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <filesystem>

// Vektor für alle Blöcke
std::vector<std::vector<unsigned char>> chunk;

int saveInVector(const std::string& filepath) {
    // Datei öffnen
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        std::cout << "Fehler beim Öffnen der Datei! saveInVector" << std::endl;
        //std::cout << "Current path is " << std::filesystem::current_path();
        return 1;
    }

    // Buffer für 16 Bytes
    std::vector<unsigned char> buffer(16);

    // Daten lesen und speichern
    while (file.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || file.gcount() > 0) {
        // Anzahl der gelesenen Bytes
        std::streamsize bytesRead = file.gcount();

        // Neuen Block erstellen und hinzufügen
        std::vector<unsigned char> block(buffer.begin(), buffer.begin() + bytesRead);
        chunk.push_back(block);

        // Wenn die Datei zu Ende ist, wird möglicherweise ein unvollständiger Block gelesen
        if (bytesRead < buffer.size()) {
            break;
        }
    }

    file.close();

    // Ausgabe der gespeicherten Blöcke
    for (size_t i = 0; i < chunk.size(); ++i) {
        std::cout << "Block " << i + 1 << ": ";
        for (unsigned char byte : chunk[i]) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(byte) << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}



std::string getChunkAsHexString( size_t offset) {
    if (offset >= chunk.size()) {
        throw std::out_of_range("Index außerhalb des gültigen Bereichs!");
    }

    const std::vector<unsigned char>& chunk1 = chunk[offset];
    std::ostringstream hexStream;

    for (unsigned char byte : chunk[offset]) {
        hexStream
        << std::hex
        << std::setw(2)
        << std::setfill('0')
        << std::uppercase
        << static_cast<int>(byte);
    }

    return hexStream.str();
}

int getVecSize() {
    return chunk.size();
}

void setChunk(std::vector<std::vector<unsigned char>>& newChunk) {
    chunk = newChunk;
}
