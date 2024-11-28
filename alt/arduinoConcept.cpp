#include <iostream>
#include <sstream>

std::stringstream val;
std::bitset<128> bs;
std::bitset<4> combined;

void readPorts(std::bitset<2> msb, std::bitset<2> lsb) {
    /*std::cout << "\n----------------------------------------------\n";
    std::cout << "bs: " << msb << lsb << std::endl;
    std::cout << "\n----------------------------------------------\n";*/

    val << msb << lsb;


    combined = (msb.to_ulong() << 2) | lsb.to_ulong();





    std::cout
        << "\n----------------------------------------------\n"
        << val.str()
        << " - "
        << combined
        << "\n----------------------------------------------\n"
    << std::endl;
}
