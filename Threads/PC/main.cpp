#include <iostream>
#include <thread>

#include "senden_PC.h"
#include "empfangen_PC.h"

int main() {
    std::thread thread1(init_reading);
    std::thread thread2(init_sending);

    thread1.join();
    thread2.join();
    return 0;
}