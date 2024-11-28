//
// Created by Matthes Böttcher on 27.11.24.
//

#include "stringToHex.h"
#include "saveInVector.h"
#include <vector>

void convertStringToHex(std::string & input) {
    std::vector<std::vector<unsigned char>> result;
    std::vector<unsigned char> currentBlock;

    std::cout << "INSTRINGTOHEX"<<std::endl;

    for (size_t i = 0; i < input.size(); ++i) {
        // Füge das aktuelle Zeichen als Byte hinzu
        currentBlock.push_back(static_cast<unsigned char>(input[i]));

        // Wenn der Block voll ist, füge ihn zum Ergebnis hinzu
        if (currentBlock.size() == 16) {
            result.push_back(currentBlock);
            currentBlock.clear();
        }
    }

    // Falls noch Bytes übrig sind, füge sie als letzten Block hinzu
    if (!currentBlock.empty()) {
        result.push_back(currentBlock);
    }

    setChunk(result);
}
