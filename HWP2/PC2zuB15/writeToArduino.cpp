#include <iostream>
#include <bitset>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <b15f/b15f.h>

#include "writeToArduino.h"


int serial_port;
int num_bytes;
char read_buf [256];

bool SERIALPORT_OPENED = false;

int openSerialPort();
void clearBufferAndReadData();
void writeToB15(int& val);
bool checkSumIsFine();

/*int main() {
    if (openSerialPort() == 0) {
        readArduinosResponse();
    }

    return 1;
}*/

int openSerialPort() {
    // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
    serial_port = open("/dev/ttyUSB1", O_RDWR);

    // Create new termios struct, we call it 'tty' for convention
    struct termios tty;

    // Read in existing settings, and handle any error
    if(tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B57600);
    cfsetospeed(&tty, B57600);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    SERIALPORT_OPENED = true;
    return 0; // success opening port
};

void clearBufferAndReadData() {
    // Normally you wouldn't do this memset() call, but since we will just receive
    // ASCII data for this example, we'll set everything to 0 so we can
    // call printf() easily.
    memset(&read_buf, 0, sizeof(read_buf)); // Buffer leeren
    // read() verursacht einen delay von ca. 1-2 Sekunden
    num_bytes = read(serial_port, &read_buf, sizeof(read_buf)); // bytes einlesen
}

// übernimmt die lowCLK
void writeToB15(int& val) {
    if (!SERIALPORT_OPENED) {
        std::cout << "öffnen von SerialPort" << std::endl;
        openSerialPort();
        usleep(2000000); // kann die ersten zeichen ohne timeout nicht senden (??)
    }


    char msg1 = '0' + val;  // Zahl in ASCII-Zeichen umwandeln
    write(serial_port, &msg1, sizeof(msg1));

    usleep(20000);

    char msg2 = '0' + 0;  // Zahl in ASCII-Zeichen umwandeln
    write(serial_port, &msg2, sizeof(msg2));

    usleep(20000);
}

bool checkSumIsFine() {
    clearBufferAndReadData();

    usleep(200000);

    if (num_bytes > 0) {
        int binary(read_buf[0] & 0b01000000); // gesendete 64 bedeutet falsche PS erkannt
        std::cout << "BINÄR PS: " << std::bitset<8>(binary) << std::endl;

        if (__builtin_popcount(binary) >= 1) {
            std::cerr << "\tFEHLER ERKANNT: " << std::bitset<8>(binary) << std::endl; 
            memset(&read_buf, '\0', sizeof(read_buf)); // Buffer leeren
            return false;
        }
    }

    memset(&read_buf, '\0', sizeof(read_buf)); // Buffer leeren
    return true;
}