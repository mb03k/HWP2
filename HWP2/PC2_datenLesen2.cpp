#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <bitset>

int main() {
    int fd = open("/dev/ttyUSB2", O_RDWR | O_NOCTTY | O_SYNC);
    if (fd == -1) {
        std::cerr << "Error opening serial port!" << std::endl;
        return 1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Error getting terminal attributes!" << std::endl;
        close(fd);
        return 1;
    }

    cfsetospeed(&tty, B9600); // Baudrate einstellen
    cfsetispeed(&tty, B9600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 Datenbits
    tty.c_iflag &= ~IGNBRK;                     // Kein Break ignorieren
    tty.c_lflag = 0;                            // Kein Echo, keine Signals
    tty.c_oflag = 0;                            // Keine Nachbearbeitung
    tty.c_cc[VMIN] = 1;                         // Minimale Anzahl an Zeichen
    tty.c_cc[VTIME] = 1;                        // Timeout

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error setting terminal attributes!" << std::endl;
        close(fd);
        return 1;
    }

    char buffer[128]; // Definiert einen Puffer für die gelesenen Daten.

    while (true) {
        int n = read(fd, buffer, sizeof(buffer)); // Liest bis zu 128 Zeichen aus der seriellen Schnittstelle in den Puffer.
        if (n > 0) { // Wenn mehr als 0 Zeichen gelesen wurden, bedeutet das, dass Daten verfügbar sind.
            for (int i = 0; i < n; ++i) { // Gehe durch jedes gelesene Byte.
                std::cout << std::bitset<8>(buffer[i]) << " "; // Gibt das Byte als 8-Bit-Binärzahl aus.
            }
            std::cout << std::endl; // Zeilenumbruch nach den Ausgaben.
            std::cout.flush(); // Stellt sicher, dass die Ausgabe sofort angezeigt wird.
        }
    }

    close(fd);
    return 0;
}
