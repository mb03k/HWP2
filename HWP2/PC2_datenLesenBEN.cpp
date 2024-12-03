#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <vector>
#include <bitset>

int main() {
    // Open the serial port
    const char* portname = "/dev/ttyUSB1"; // Replace with your correct port name
    int serial_port = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);

    if (serial_port < 0) {
        std::cerr << "Error opening serial port!" << std::endl;
        return 1;
    }

    // Configure the serial port
    struct termios tty;
    memset(&tty, 0, sizeof(tty));

    if (tcgetattr(serial_port, &tty) != 0) {
        std::cerr << "Error getting port attributes!" << std::endl;
        return 1;
    }

    // Set baud rate, data bits, parity, and stop bits
    cfsetispeed(&tty, B57600); // Input baud rate
    cfsetospeed(&tty, B57600); // Output baud rate
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

    // Buffer to store incoming data
    char read_buf[256]; // Buffer to store incoming data
    memset(read_buf, 0, sizeof(read_buf));

    // To store the message line
    std::string line_buffer;

    // Loop to continuously read data
    while (true) {
        usleep(15000); // Delay 100ms to avoid spamming

        // Read data from the serial port
        int num_bytes = read(serial_port, &read_buf, sizeof(read_buf) - 1);
        std::cout << num_bytes << std::endl;
        if (num_bytes < 0) {
            //perror("Error reading from serial port");
        } else if (num_bytes > 0) {
            for (int i = 0; i < num_bytes; ++i) {
                char current_byte = read_buf[i];

                // Check for end of a line (\r\n or \n)
                if (current_byte == '\n') {
                    // Process the accumulated line, excluding the fixed "read: " part
                    if (line_buffer.size() > 6) {
                        // Skip the "read: " prefix, take the rest
                        std::string dynamic_part = line_buffer.substr(6); // Skip "read: "
                        std::cout << "read: " << dynamic_part << std::endl;
                    }

                    // Clear the line buffer for the next line
                    line_buffer.clear();
                } else if (current_byte != '\r') {
                    // Add characters to the buffer (ignore \r)
                    line_buffer += current_byte;
                }
            }
        } else {
            std::cout << "No data received..." << std::endl;
        }
    }

    // Close the serial port
    close(serial_port);
    return 0;
}
