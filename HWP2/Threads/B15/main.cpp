#include <iostream>
#include <thread>

#include "main.h"
#include "senden.h"
#include "empfaengen.h"

int main() {
    std::thread thread1(init_reading);
    std::thread thread2(init_sending);

    thread1.join();
    thread2.join();
    return 0;
}