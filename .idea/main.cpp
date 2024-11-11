#include <iostream>
#include <b15f/b15f.h>
#include <vector>
#include <string>
#include <bitset>
#include <fstream>

using namespace std;

// reads binary data from file into vector
std::vector<std::byte> readFileData(const std::string& name) {
	//open file in binary mode
	std::ifstream inputFile(name, std::ios_base::binary);
	
	//check if successfully opened
	if(!inputFile) {
		throw std::ios_base::failure("Failed to open file: " + name);
	}
	
	// move to end of file to determine file length
	inputFile.seekg(0, std::ios_base::end);
	auto length = inputFile.tellg();
	inputFile.seekg(0, std::ios_base::beg);
	
	//check for valid file length
	if (length <= 0) {
        	throw std::ios_base::failure("Invalid file size for file: " + name);
	}
	
	// allocate buffer of length
	std::vector<std::byte> buffer(length);
	
	 //read file content into buffer	
	inputFile.read(reinterpret_cast<char*>(buffer.data()), length);
	
   	if (!inputFile) {
        	throw std::ios_base::failure("Failed to read file data: " + name);
    	}
    
	inputFile.close();
	return buffer;
	
}

void send4bitdata(B15F& drv, uint8_t data, bool sync) {

	uint8_t packet = (sync << 4) | (data & 0x0F);
	
	drv.setRegister(&PORTA, packet);
	cout << "packet sent: " << bitset<8>(data) << endl;
	drv.delay_ms(100);	

}

void receivedata(B15F& drv) {
uint8_t highNibble = 0;
    uint8_t lowNibble = 0;
    bool expectingHighNibble = true;  // Flag to track if we're expecting the high nibble

    while (true) {
        // Read the 4-bit packet from PORTA
        uint8_t receivedPacket = drv.getRegister(&PINA) & 0x1F;  // Mask to 5 bits (4 data + 1 sync)

        // Extract sync and data bits
        bool sync = (receivedPacket >> 4) & 0x01;  // 5th bit as sync
        uint8_t data = receivedPacket & 0x0F;      // Lower 4 bits as data

        cout << "Received packet: " << bitset<8>(receivedPacket) 
             << " (sync bit: " << sync << ", data: " << bitset<4>(data) << ")" << endl;

        if (sync) {  // If sync bit is set, it indicates the start of a new 8-bit byte
            highNibble = data;
            expectingHighNibble = false;
        } else if (!sync && !expectingHighNibble) {  // Low nibble follows high nibble
            lowNibble = data;

            // Combine high and low nibbles into one byte
            uint8_t reconstructedByte = (highNibble << 4) | lowNibble;

            // Output the reconstructed byte for verification
            cout << "Reconstructed byte: " << bitset<8>(reconstructedByte) 
                 << " (hex: " << std::hex << int(reconstructedByte) << ")" << endl;

            // Reset for the next byte
            expectingHighNibble = true;
        }

        drv.delay_ms(100);  // Small delay to avoid reading too fast
    }

}

int main() {

    try {
    B15F& drv = B15F::getInstance();
    drv.setRegister(&DDRA, 0xFF);
    
    std::string fileName = "200random.bin";
    std::vector<std::byte> fileData = readFileData(fileName);

    std::cout << "File read successfully! File size: " << fileData.size() << " bytes." << std::endl;

	//output in byte, hex, binary
        for (size_t i = 0; i < fileData.size(); ++i) {
        	uint8_t byte = std::to_integer<uint8_t>(fileData[i]);
        	
        	uint8_t high = (byte >> 4) & 0x0F;  // High 4 bits
            	uint8_t low = byte & 0x0F;          // Low 4 bits
            	
       		std::cout << "Hex: " << std:: hex << std::to_integer<int>(fileData[i]) << endl;
       		std::cout << "Binary: " << std::bitset<8>(byte) << endl;
       		
            	std::cout << "  High: " << bitset<4>(high) << " (hex: " << std::hex << int(high) << ")" << std::endl;
            	std::cout << "  Low:  " << bitset<4>(low) << " (hex: " << std::hex << int(low) << ")" << std::endl;
              	send4bitdata(drv, high, true); 
            	send4bitdata(drv, low, false);
            	
        	}   
    	} catch (const std::ios_base::failure& e) {
        	std::cerr << "Error: " << e.what() << std::endl;
        
     }	
	
    return 0;
}


//filestream iobinary send in packages

