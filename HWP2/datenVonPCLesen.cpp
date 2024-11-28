#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <iostream>
#include <cstring>

int main() {
    const char *device = "/dev/ttyUSB0"; // Pfad zum seriellen USB-Gerät
    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC); // Öffnen des Geräts

    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return 1;
    }

    // Konfiguration der seriellen Schnittstelle
    cfsetospeed(&tty, B9600); // Baudrate
    cfsetispeed(&tty, B9600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 Datenbits
    tty.c_iflag &= ~IGNBRK;                     // keine Break-Behandlung
    tty.c_lflag = 0;                            // kein Echo, keine Signale
    tty.c_oflag = 0;                            // kein Nachbearbeiten
    tty.c_cc[VMIN] = 1;                         // Minimale Zeichenanzahl zum Lesen
    tty.c_cc[VTIME] = 1;                        // Timeout

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return 1;
    }

    // Daten lesen
    char buffer[256];
    int n = read(fd, buffer, sizeof(buffer));
    if (n > 0) {
        std::cout << "Gelesene Daten: " << std::string(buffer, n) << std::endl;
    } else {
        std::cerr << "Fehler beim Lesen oder keine Daten verfügbar" << std::endl;
    }

    close(fd);
    return 0;
}
