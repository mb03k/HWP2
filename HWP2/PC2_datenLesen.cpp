#include <iostream>
#include <bitset>
#include <fcntl.h>    // Für open()
#include <termios.h>  // Für termios
#include <unistd.h>   // Für read() und write()
#include <cstring>    // Für memset()
#include <fstream>
#include <sstream>
#include <vector>

void writeToFile();
void writeToBin(unsigned char val);

std::vector<int> werteVec;
std::vector<int> checkSum;

bool readCheckSum = false;

int main() {
    const char *portname;
    int fd;
    
    portname = "/dev/ttyUSB1"; // Passe dies an den Port deines Arduinos an
    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
    std::remove(portname);

    if (fd == -1) {
        portname = "/dev/ttyUSB2";
        fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
        std::cout << " -- USB1 konnte nicht geöffnet werden -- " << std::endl;
    }

    if (fd == -1) {
        std::cerr << "Fehler: USB slots konnten nicht geöffnet werden.\n";
        return 1;
    }

    // Serielle Schnittstelle konfigurieren
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Fehler: Konnte die Einstellungen des seriellen Ports nicht lesen.\n";
        return 1;
    }

    // Einstellungen für den seriellen Port setzen
    cfsetospeed(&tty, B57600); // Baudrate setzen (muss mit Arduino übereinstimmen)
    cfsetispeed(&tty, B57600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 Datenbits
    tty.c_iflag &= ~IGNBRK;                     // Kein Break ignorieren
    tty.c_lflag = 0;                            // Kein Echo, keine Signals
    tty.c_oflag = 0;                            // Keine Nachbearbeitung
    tty.c_cc[VMIN] = 1;                         // Minimale Anzahl an Zeichen
    tty.c_cc[VTIME] = 1;

    /* VON BEN ALT */
    tty.c_cflag &= ~PARENB;   // No parity
    tty.c_cflag &= ~CSTOPB;   // 1 stop bit
    tty.c_cflag &= ~CSIZE;    // Clear data bits
    tty.c_cflag |= CS8;       // 8 data bits
    tty.c_cflag |= CREAD | CLOCAL; // Enable reading, ignore modem control lines

    // Set input and output flags
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable flow control
    tty.c_oflag &= ~OPOST; // Raw output
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input

    // Apply the settings
    tcsetattr(serial_port, TCSANOW, &tty);

    /* VON BEN ALT -> if(tcsetattr) würde ich erstmal drinnen lassen
     * bzw den Aufruf über diesen Text */

    /*tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 Datenbits
    tty.c_iflag &= ~IGNBRK;                     // Ignoriere keine Breaks
    tty.c_lflag = 0;                            // Keine Echoeingabe
    tty.c_oflag = 0;                            // Keine Ausgabeverarbeitung
    tty.c_cc[VMIN] = 1;                         // Mindestens 1 Zeichen lesen
    tty.c_cc[VTIME] = 0;                        // Zeitüberschreitung 0.1s

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);     // Keine Software-Flow-Steuerung
    tty.c_cflag |= (CLOCAL | CREAD);            // Aktivieren des Empfängers
    tty.c_cflag &= ~(PARENB | PARODD);          // Keine Parität
    tty.c_cflag &= ~CSTOPB;                     // 1 Stop-Bit
    tty.c_cflag |= PARENB;                      // Aktiviert gerade Parität

    tty.c_cflag &= ~CRTSCTS;                    // Keine Hardware-Flow-Steuerung
    */

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Fehler: Konnte die Einstellungen nicht anwenden.\n";
        return 1;
    }

    // Daten lesen
    unsigned char buf[256]; // Größerer Puffer
    int counter = 0;
    while (true) {
        memset(buf, 0, sizeof(buf)); // Buffer leeren
        int n = read(fd, &buf, sizeof(buf) - 1); // Bis zu sizeof(buf)-1 Bytes lesen
        
        if (n > 0) {
            // Byte als Binärwert ausgeben
            int binary(buf[0]); // 2 Bit pro Byte
            int bin(buf[0] & 0b11);

            if (binary>0) {
                if (binary == 1) {
                    std::cout << "!!!ANFANG BLOCK!!!" << std::endl;
                    counter = 0;
                    readCheckSum = false;
                }

                if (binary == 2) {
                    std::cout << "---ENDE BLOCK---" << std::endl;
                    readCheckSum = true;
                    counter = 0;
                }

                if (binary == 3) {
                    break;
                }
                
                if (readCheckSum) {
                    counter++;
                    std::cout << "PS: "<<std::bitset<8>(binary)<< " - " << counter<<std::endl;
                }
                else if (binary >= 8) {
                    counter++;
                    std::cout << std::bitset<8>(binary)<< " - " << counter<<std::endl;
                    werteVec.push_back(bin);
                }
            }

            /*
                JETZT NOCH:
                Prüfsumme berechnen und einlesen -> danach zurückschicken 
                und im anderen Programm prüfen
            */

            /*if (binary == 1) { // Anfang Block
                std::cout << "ANFANG BLOCK cs false" << std::endl;
                readCheckSum = false;
            }

            else if (binary == 2) { // Ende des Blocks
                std::cout << "ENDE BLOCK cs true" << std::endl;
                readCheckSum = true;
            }

            if (readCheckSum) {
                std::cout 
                    << "PRÜFSUMME: " 
                    << bin
                << std::endl;
                checkSum.push_back(bin);
            } 

            else if (binary >= 8) {
                std::cout 
                    << "gespeichert: " 
                    << std::bitset<8>(binary)
                    << " (" << bin << ")" 
                << std::endl;
                werteVec.push_back(bin);
                //std::cout << " --- " << werteVec << "\n\n";
            }

            else if (binary == 3) { // Ende vom senden
                std::cout << "BREAK" << std::endl;
                break;
            }*/

            // neuer Ansatz

            /*if (binary == 1) { // Anfang Block
                std::cout << "ANFANG BLOCK cs false" << std::endl;
                std::cout << "\tgelesen: " << std::bitset<8>(binary)<<std::endl;
                readCheckSum = false;
            } else if (binary == 3) {
                break;
            }
            else {
                if (readCheckSum) {
                    std::cout << "\tCHECKSUM LESEN: " << std::bitset<8>(binary) << std::endl;
                    checkSum.push_back(bin);
                } else {
                    if (binary >= 8) {
                        std::cout 
                            << "gespeichert: " 
                            << std::bitset<8>(binary)
                            << " (" << bin << ")" 
                        << std::endl;
                        werteVec.push_back(bin);
                    } else {
                        if (binary == 2) {
                            std::cout << "---\nENDE BLOCK: cs true - " << std::bitset<8>(binary) <<"\n"<< std::endl;
                            readCheckSum = true;
                        }
                    }
                }
            }*/


        } 
    }

    /*for (int i : werteVec) {
        std::cout << i << " ";
    }*/

    writeToFile();


    close(fd); // Seriellen Port schließen
    return 0;
}

void writeToFile() {

    // Binärdatei schreiben
    //writeBitsToBinaryFile("output.bin");

    unsigned char val = 0;

    for (int i=0; i<werteVec.size(); i+=4) {
        val =    (werteVec[i] << 6) 
                | (werteVec[i+1] << 4) 
                | (werteVec[i+2] << 2) 
                | werteVec[i+3];
        //std::cout << "uchar: " << static_cast<int>(val) << std::endl;
        //std::cout << "binär: " << std::bitset<8>(val) << "\n" << std::endl;
        writeToBin(val);
        val = 0;
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
