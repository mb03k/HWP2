#include <iostream>
#include <fstream>
#include <iomanip>

int main() {
    // Dateiname der Binärdatei
    std::string filename = "2krandom.bin";

    // Datei im Binärmodus öffnen
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Datei konnte nicht geöffnet werden!" << std::endl;
        return 1;
    }

    // Zeichenweise Lesen und Ausgabe der Hex-Werte
    char byte;
    int counter = 1;
    std::bitset<4> oldBits;

    while (file.get(byte)) {
        std::bitset<4> newBits = byte &0x0F;

        if (oldBits != newBits) {
            oldBits = newBits;
        } else {
            /*std::cout << byte << " - " << newBits;
            // Ausgabe des Hex-Wertes des aktuellen Zeichens
            std::cout << " - " << std::hex << std::setw(2) << std::setfill('0')
                  << (static_cast<unsigned int>(static_cast<unsigned char>(byte)))
                  << std::endl;*/
            std::cout<<"alt - " << "c: " << counter << " - " << byte <<" - "<< oldBits<<std::endl;
            std::cout<<"neu - " << "c: " << counter << " - " << byte << " - "<<newBits<<std::endl;
        }

        counter++;
    }

    std::cout << std::endl;

    file.close();
    return 0;
}
