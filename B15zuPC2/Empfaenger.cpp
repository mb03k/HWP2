#include <iostream>
#include <vector>
#include <bitset>
#include <fstream>

// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

/*
 *
 * https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
 * Von diesem Blog -> sehr detailliert
 */

void calcReceivedCheckSumFromVec();
void handleInput();
void clearBufferAndReadData();
void writeToFile();
void writeToBin(unsigned char val);
void sendBlockAgain();
void dontSendBlockAgain();

void setWrongCheckSum();
void setCorrectCheckSum();
void calculateBitwiseCheckSum(int& bin);
void calculateCS();
bool checkSumsAreEqual();

int serial_port;

// Write to serial port
unsigned char msg[] = { 'H', 'e', 'l', 'l', 'o', '\r' };

// Allocate memory for read buffer, set size according to your needs
char read_buf [256]; // in meinem Programm unsigned

// Read bytes. The behaviour of read() (e.g. does it block?,
// how long does it block for?) depends on the configuration
// settings above, specifically VMIN and VTIME
int num_bytes;

// für switch case
std::vector<std::vector<int>> werteVec;
std::vector<int> checkSumVec;
int calculatedCheckSum = 0; // die man mit jedem Wert eingelesen hat
int receivedCheckSum = 0; // die man zum Ende bekommen hat
bool readCheckSum = false;
int offset = 0;
bool READCS = false;
bool ENDOFSENDING = false;
bool LASTBLOCK_CKECK = false;
bool LASTBLOCK = false;
std::vector<std::vector<int>> valVec;
std::vector<int> csVec;

int main() {
    // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
    serial_port = open("/dev/ttyUSB1", O_RDWR);

    // Create new termios struct, we call it 'tty' for convention
    struct termios tty;

    // Read in existing settings, and handle any error
    if(tcgetattr(serial_port, &tty) != 0) {
        std::cerr << "Fehler USB1 zu öffnen. Ende" << std::endl;
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

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B57600);
    cfsetospeed(&tty, B57600);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        //printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    // Hauptfunktion zum einlesen
    handleInput();

    // n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
    if (num_bytes < 0) {
        //printf("Error reading: %s", strerror(errno));
        return 1;
    }

    close(serial_port);
    return 0;
}

void handleInput() {
    int alterWert = -1;
    int valuesReadCounter = 0;
    int counter = 1;
    valVec.resize(offset+1);
    while (true) {
        clearBufferAndReadData();
        if (num_bytes > 0) {
            int binary(read_buf[0] & 0b00001111); // alle Daten einlesen
            int bin(read_buf[0] & 0b11); // Daten auf die relevanten kürzen
            if (binary > 0) {
                switch(binary) {
                    case 1:
                        // unnötig -> ABER
                        // testen ob in einem Block mehr als 64 Zeichen gesendet wurde
                        // -> es kann sein dass in dem Block eine '0100' gesendet wurde
                        //    also dass keine neuen positiven Bits hinzugefügt wurden.
                        //    Das würde aber trotzdem den Wert ändern -> wird hiermit abgefangen
                        if (LASTBLOCK_CKECK) {
                            LASTBLOCK = true;
                        } else {
                            LASTBLOCK = false;
                        }
                        std::cerr << "***ANFANG BLOCK***" << std::endl;
                        calculatedCheckSum = 0;
                        receivedCheckSum = 0;
                        counter = 0;
                        csVec.clear();
                        ENDOFSENDING = false;
                        LASTBLOCK_CKECK = true;
                        break;

                    case 2:
                        std::cerr << "---JETZT PS---" << std::endl;
                        READCS = true;
                        ENDOFSENDING = false;
                        LASTBLOCK_CKECK = false;
                        break;
                    
                    case 3: 
                        if (ENDOFSENDING) {
                            std::cerr << "!!!ENDE SENDEN!!!" << std::endl;
                            goto exit_loop;
                        }

                        std::cerr << "___ENDE PS___" << std::endl;
                        calculateCS();

                        if (!checkSumsAreEqual() || (counter != 64 && !LASTBLOCK)) {
                            std::cerr << "\tFALSCHE PS counter "<< counter << std::endl;
                            valVec[offset].clear();
                            counter = 0;
                            calculatedCheckSum = 0;
                            setWrongCheckSum();
                        } else {
                            offset++;
                            valVec.resize(offset+1);
                        }

                        READCS = false;
                        ENDOFSENDING = true;
                        LASTBLOCK_CKECK = false;
                        break;

                    default:
                        if (READCS) {
                            csVec.push_back(bin);
                            std::cerr << "PS: " << std::bitset<8>(binary) << " - " << std::bitset<2>(bin) << std::endl;
                        } else if (binary >= 4) {

                            std::cerr
                                << std::bitset<8>(binary)
                                << " - "
                                << valuesReadCounter
                                << " - pb: "
                                << __builtin_popcount(bin)
                                << " - " 
                                << counter
                            << std::endl;
                            
                            valVec[offset].push_back(bin);
                            calculateBitwiseCheckSum(bin);
                            ENDOFSENDING = false;
                            LASTBLOCK_CKECK = false;
                            counter++;
                        }
                }
            }
        }
    }
    exit_loop: ;
    writeToFile();
}

// ------------
void setWrongCheckSum() {
    int val = 600;
    write(serial_port, &val, sizeof(val));
}

void setCorrectCheckSum() {
    int val = 60;
    write(serial_port, &val, sizeof(val));
}

void calculateBitwiseCheckSum(int& bin) {
    calculatedCheckSum += __builtin_popcount(bin);
}

void calculateCS() {
    receivedCheckSum = 0;
    int shift = 6;

    for (int i=0; i<csVec.size(); i++) {
        receivedCheckSum = receivedCheckSum | (csVec[i] <<shift);
        shift -= 2;
    }
}

bool checkSumsAreEqual() {
    std::cerr
        << "\tberechnete Prüfsumme: "
        << calculatedCheckSum
        << " - erhaltene Prüfsumme: "
        << receivedCheckSum
    << std::endl;
    return receivedCheckSum == calculatedCheckSum;
}

// ------------

void clearBufferAndReadData() {
    // Normally you wouldn't do this memset() call, but since we will just receive
    // ASCII data for this example, we'll set everything to 0 so we can
    // call printf() easily.
    memset(&read_buf, '\0', sizeof(read_buf)); // Buffer leeren
    num_bytes = read(serial_port, &read_buf, sizeof(read_buf)); // bytes einlesen
}

void writeToFile() {
    std::cerr << "VEKTOR:"<<std::endl;
    for (std::vector<int> i : valVec) {
        for (int j=0; j<i.size(); j+=4) {
            unsigned char val = (i[j] << 6)
                            | (i[j+1] << 4)
                            | (i[j+2] << 2)
                            | i[j+3];
            std::cout << val;
            //std::cout.put(val); // terminal ausgabe (./lesen > empfangeneDaten.bin)
        }
    }
}   

void writeToBin(unsigned char val) {
    std::ofstream outfile("output.bin", std::ios::binary | std::ios::app);

    // Überprüfen, ob die Datei erfolgreich geöffnet wurde
    if (!outfile) {
        std::cerr << "Fehler beim Öffnen der Datei!" << std::endl;
    }

    // Schreibe den unsigned char in die Datei
    outfile.write(reinterpret_cast<const char*>(&val), sizeof(val));

    // Überprüfen, ob das Schreiben erfolgreich war
    if (outfile.fail()) {
        std::cerr << "Fehler beim Schreiben in die Datei!" << std::endl;
    }

    // Schließe die Datei
    outfile.close();
}