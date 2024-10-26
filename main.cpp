#include <iostream>
#include <iomanip>
#include <b15f/b15f.h>

using namespace std;
void sendHex(vector<string> sc, B15F& drv);
void sendChecksum(vector<string> hexVec, B15F& drv);
unsigned char charToHex(char hexChar);

int main() {
    B15F& drv = B15F::getInstance();
    drv.setRegister(&DDRA, 0xFF); // Bit 0-7 Ausgabe
    drv.setRegister(&PORTA, 0x00);

    std::string sendingText = /*"mein String den du senden sollen tust denn ich bin matthes muhahahaha ja das ist ein"
                              "Langer Text von mir aber mit absicht denn ich achte nicht darauf was ich schreibe";*/
    "Abcdef ---- 1000000000000000000";

    /* convert char to hex and save a STRING with the hex value */
    cout << "\n----------------------\nString (char) zu Hex" << endl;
    vector<string> hexVec;
    for (char st : sendingText) {
        std::stringstream sendingText_characters;

        // char zu hex
        sendingText_characters << std::hex << std::uppercase << (int)st;
        string hexStr = sendingText_characters.str();
        hexVec.push_back(hexStr);

        if (hexVec.size() == 16) { // 64-bit (one package - saves as HEX)
            //sendHex(hexVec, drv);
            sendChecksum(hexVec, drv);

            hexVec.clear();
        }

        //sendHex(hexStr, drv);
    }
}

void sendHex(vector<string> hexVec, B15F& drv) { // only sends if 64 bit were written down
    for (string hexV : hexVec) { // e.g. {"1A", "FD", "4B"}
        for (char hexAsChar : hexV) { // e.g. "1A"
            drv.setRegister(&PORTA, 0x00); // LEDs to null
            drv.delay_ms(100);
            drv.setRegister(&PORTA, hexAsChar &0x0f); // right LEDs (for hex)
            drv.delay_ms(200);
        }
    }
}

void sendChecksum(vector<string> hexVec, B15F& drv) {
    unsigned int quersumme = 0;

    for (string hexV : hexVec) {
        for (char z : hexV) {
            cout<<"Char: "<<z;
            if (isalpha(z)) { // is a letter (a-f)
                string s = to_string(z);
                int ans = stoi(s, 0, 16);
                cout << " - ans: "<<ans<< " --- mit: "<<z << endl;
                quersumme += ans;
            } else { // is a number
                int x = (int)z - 48;
                cout << " - Zahl mit: "<<x<<endl;
                quersumme += x;
            }
        }
    }
    cout << "ergebnis: "<<quersumme<<endl;
}

unsigned char charToHex(char hexChar) {
    if (hexChar >= '0' && hexChar <= '9') {
        return hexChar - '0';  // '0' hat den ASCII-Wert 48, daher hexChar - '0' gibt den numerischen Wert
    } else if (hexChar >= 'A' && hexChar <= 'F') {
        return hexChar - 'A' + 10;  // 'A' ist 10 in Hex
    } else if (hexChar >= 'a' && hexChar <= 'f') {
        return hexChar - 'a' + 10;  // 'a' ist auch 10 in Hex
    }
    return 0;
}


// ---------------------------------------------------------------------------------------------------------

void schrittEins() { // 64-Bit-Block an Dateien senden

}

void schrittZwei() { // Arduino berechnet Prüfsumme (Code für ArduinoIDE)

}

void schrittDrei() { // 8-Bit-Block Prüfsumme an Arduino schicken

}







