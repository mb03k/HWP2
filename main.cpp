#include <iostream>
#include <iomanip>
#include <b15f/b15f.h>

using namespace std;
void sendHex(vector<string> sc, B15F& drv);
void sendChecksum(vector<string> hexVec, B15F& drv);
unsigned char charToHex(char hexChar);
void send64bit();
void awaitResponse(B15F& drv);
int ard_calculateChecksum(int block64bit);
bool ard_compareChecksum(int geleseneChecksum, int calculatedChecksum);

int main() {
    send64bit();
}

void send64bit() {
    B15F& drv = B15F::getInstance();
    drv.setRegister(&DDRA, 0xFF); // Bit 0-7 Ausgabe
    drv.setRegister(&PORTA, 0x00);

    std::string sendingText = "mein String den du senden sollen tust denn ich bin matthes muhahahaha ja das ist ein"
                              "Langer Text von mir aber mit absicht denn ich achte nicht darauf was ich schreibe";

    /* convert char to hex and save a STRING with the hex value */
    vector<string> hexVec;
    for (char st : sendingText) { // zeichen für zeichen
        std::stringstream sendingText_characters;

        // char zu hex
        sendingText_characters << std::hex << std::uppercase << (int)st;
        string hexStr = sendingText_characters.str();
        hexVec.push_back(hexStr);

        /*
            !!!!!!
            wenn vektor nicht bis 64 bit belegt wird (Textdatei hat bspw. nicht mehr so viele Zeichen)
            wird der Code nicht mehr ausgeführt
        */
        if (hexVec.size() == 16) { // 64-bit sind im Vektor gespeichert
            sendHex(hexVec, drv); // schritt 1
            sendChecksum(hexVec, drv); // schritt 2

            awaitResponse(drv);

            hexVec.clear();
        }
    }
}

void awaitResponse(B15F& drv) {
    bool wertWurdeGelesen = false;
    //while (!wertWurdeGelesen) {
    uint8_t value1 = 0x0F;
    uint8_t val = drv.getRegister(&value1); // rechte(?) Seite der ports mit den gesendeten Werten lesen
    cout << "Wert: " << val << endl;
    //if ( /* die 4 Ports einem Muster entsprechen? bzw. einfach den richtigen Wert ergeben */ ) {

    //}

    //if ( /* response ist ein richtiger wert -> block neu senden oder nächsten senden */ ) {
    //wertWurdeGelesen = true;
    //}
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






/**** ARDUINO ****/

// liest alles ein, was gesendet wird
void ard_readBitBlocks() {
    int gelesener64BitBlock;
    // 64 Bit einlesen
    //drv.getRegister(wert);

    // 8 Bit Prüfsumme einlesen
    int geleseneChecksum;

    // Prüfsumme bilden und danach vergleichen
    int calculatedChecksum;
    calculatedChecksum = ard_calculateChecksum(gelesener64BitBlock);

    // Prüfsummen sind gleich (weiteren 64-Bit-Block senden)
    if (ard_compareChecksum(geleseneChecksum, calculatedChecksum)) {

    } else { // Prüfsummen sind unterschiedlich (nochmals 64-Bit-Block schicken)

    }
}

int ard_calculateChecksum(int block64bit) {
    int checksum;

    /*
     * Prüfsumme der 64 Bit berechnen
     */

    std::cout << "Die Prüfsumme ist: " << checksum << endl;

    return 0;
}

bool ard_compareChecksum(int geleseneChecksum, int calculatedChecksum) {
    // empfangene mit der berechneten Prüfsumme vergleichen
    return geleseneChecksum == calculatedChecksum;
}
