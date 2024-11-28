//
// Created by Matthes Böttcher on 17.11.24.
//

#ifndef HWP_15_11_GETBITS_H
#define HWP_15_11_GETBITS_H


class GetBits {
public:
    GetBits(const std::string& filePath); // Deklaration
    ~GetBits();
    std::string getNextBlock(); // Deklaration

private:
    std::ifstream file;
    const std::size_t blockSize;
};

#endif //HWP_15_11_GETBITS_H
