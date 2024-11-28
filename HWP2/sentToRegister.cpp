#include <iostream>
#include <bitset>

void writeMSBtoRegister(char ch, int msb) {
    std::cout
        << "\t\tsend '"
        << ch
        << "'  MSB: "
        << std::bitset<4>(msb)
    << std::endl;
}

void writeLSBtoRegister(char ch, int lsb) {
    std::cout
        << "\t\tsend '"
        << ch
        << "'  LSB: "
        << std::bitset<4>(lsb)
    << std::endl;
}

void sendLowCLK() {
    //drv.setRegister(&PORTA, 0x00);
    //delay100();

    std::cout << "\t\t0000 - CLK" << std::endl;
}

void sendBlockStartSequence() {
    std::cout << "ANFANG BLOCK: 0001" << std::endl;
    //drv.setRegister(&PORTA, 1);
    //delay100();
}

void sendBlockEndSequence() {
    std::cout << "ENDE BLOCK: 0010" << std::endl;
    //drv.setRegister(&PORTA, 2);
    //delay100();
}

void delay100() {
    //drv.delay_ms(100);
}