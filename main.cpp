#include <iostream>

using namespace std;

void send64bit(string& txt);

int main() {
    string hexText = "1Aaaa2B3C4D5EFF";
    send64bit(hexText);

    return 0;
}

void send64bit(string& hexStr) {
    int counter = 1;
    char oldCh;
    // neuerer Ansatz
    for (char ch: hexStr) {
        //drv.setRegister(&PORTA, 0x00); // LEDs to null
        //drv.delay_ms(20);

        /*
         * Idee:
         * wenn zwei zeichen hintereinander gleich sind und gesendet werden, erst ein escape zeichen schicken
         */
        if (oldCh == ch) { // zwei identische zeichen werden nacheinander gesendet
            cout << "ESC wurde gesendet" << endl;
            //drv.setRegister(&PORTA, escapeInHex &0x0F);
            //drv.delay_ms(50);
            cout << "normales Zeichen gesendet: " << ch << endl;
            //drv.setRegister(&PORTA, ch &0x0F);
            //drv.delay_ms(50);
        } else { // der Regelfall (einfach das Zeichen senden)
            oldCh = ch;
            cout << "Counter: "<< counter << " - hex: "<< bitset<4>(ch) << " - " << ch << endl;
            //drv.setRegister(&PORTA, ch &0x0F);
            //drv.delay_ms(50);
        }

        counter++;
    }
}