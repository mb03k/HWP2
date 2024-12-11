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

    for (int i=0; i<input.length(); i++) {
        if (i % 16 == 0) {
            dataChunks.push_back(chunk);
            chunk.clear();
        }
        chunk.push_back(input.at(i));
    }
    if (input.length() % 16 != 0) { // letzten chunk der evtl nicht voll ist einfügen
        dataChunks.push_back(chunk);
    }

    for (std::vector<unsigned char> chunk : dataChunks) {
        for (unsigned char c : chunk) {
            std::cout << c << " ";
        }
        std::cout << std::endl;
    }
}

std::string getChunkAsHexString(size_t offset) {
    if (offset >= dataChunks.size()) {
        throw std::out_of_range("Index außerhalb des gültigen Bereichs!");
    }
    std::ostringstream hexStream;
    for (unsigned char byte : dataChunks[offset]) {
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
    return dataChunks.size();
}

void setChunk(std::vector<std::vector<unsigned char>>& newDataChunk) {
    dataChunks = newDataChunk;
}
