#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <filesystem>

// Vektor für alle Blöcke
std::vector<std::vector<unsigned char>> dataChunks;

void saveInVector(const std::string& input) {
    std::vector<unsigned char> chunk;
    for (size_t i=0; i<input.length(); i++) {
        if ((i % 16 == 0) && (i > 0)) {
            dataChunks.push_back(chunk);
            chunk.clear();
        }
        chunk.push_back(input.at(i));
    }
    if (input.length() % 16 != 0) { // letzten chunk der evtl nicht voll ist einfügen
        dataChunks.push_back(chunk);
    }

    for (std::vector<unsigned char> chunk : dataChunks) {
        std::cout << "Chunk: ";
        for (unsigned char c : chunk) {
            std::cout << c << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<unsigned char> getChunk(size_t offset) {
    return dataChunks[offset];
}

int getVecSize() {
    return dataChunks.size();
}

void setChunk(std::vector<std::vector<unsigned char>>& newDataChunk) {
    dataChunks = newDataChunk;
}
