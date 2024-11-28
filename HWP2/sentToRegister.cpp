#include <iostream>
#include <bitset>
#include <b15f/b15f.h>

void delay100(B15F& drv);

void writeMSBtoRegister(B15F& drv, char ch, int msb) {
    std::cout
        << "\t\tsend '"
        << ch
        << "'  MSB: "
        << std::bitset<4>(msb)
    << std::endl;
}

void writeLSBtoRegister(B15F& drv, char ch, int lsb) {
    std::cout
        << "\t\tsend '"
        << ch
        << "'  LSB: "
        << std::bitset<4>(lsb)
    << std::endl;
}

void sendLowCLK(B15F& drv) {
    drv.setRegister(&PORTA, 0x00);
    delay100(drv);

    std::cout << "\t\t0000 - CLK" << std::endl;
}

void sendBlockStartSequence(B15F& drv) {
    std::cout << "ANFANG BLOCK: 0001" << std::endl;
    drv.setRegister(&PORTA, 1);
    delay100(drv);
}

void sendBlockEndSequence(B15F& drv) {
    std::cout << "ENDE BLOCK: 0010" << std::endl;
    drv.setRegister(&PORTA, 2);
    delay100(drv);
}

void delay100(B15F& drv) {
    drv.delay_ms(100);
}