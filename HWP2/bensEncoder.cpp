#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <vector>
#include <optional>
#include <cstdint>

class encoder {
private:
    std::vector<uint8_t> data; // The data to encode
    size_t currentIndex;       // Tracks the current position in the data
    uint8_t startByte;         // Optional start byte

public:
    // Constructor: Initialize with start byte and data
    encoder(uint8_t start, const std::vector<uint8_t>& inputData)
        : startByte(start), currentIndex(0) {
        // Add the start byte to the data (optional protocol feature)
        data.push_back(startByte);
        data.insert(data.end(), inputData.begin(), inputData.end());
    }

    // Check if more data is available
    bool hasData() const {
        return currentIndex < data.size();
    }

    // Get the next byte to send
    std::optional<uint8_t> nextByte() {
        if (hasData()) {
            return data[currentIndex++];
        } else {
            return std::nullopt; // No more data
        }
    }
};

#endif // ENCODER_HPP
