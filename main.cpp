#include <iostream>
#include <iomanip>
#include <b15f/b15f.h>

using namespace std;
void sendHex(vector<string> sc, B15F& drv);
void sendChecksum(vector<string> hexVec, B15F& drv);
unsigned char charToHex(char hexChar);
void send64bit(B15F& drv, vector<string>& hexVec);
bool awaitResponse(B15F& drv);
int ard_calculateChecksum(int block64bit);
bool ard_compareChecksum(int geleseneChecksum, int calculatedChecksum);
void setAndSend64bitBlock(B15F& drv, string& textToSend);

int main() {
    B15F& drv = B15F::getInstance();
    drv.setRegister(&DDRA, 0xFF); // Bit 0-7 Ausgabe
    drv.setRegister(&PORTA, 0x00);

    // text einlesen
    string testText = "Hallo ich bin Matthes, sachen machen sachen und so. Bitte funktionier, das wäre super duper tolli frolli";

    setAndSend64bitBlock(drv, testText);
}

void setAndSend64bitBlock(B15F& drv, std::string& textToSend) {
    std::stringstream sendingTextCharacters;
    vector<string> hexVec;
    for (char st : textToSend) { // zeichen für zeichen
        // char zu hex
        sendingTextCharacters << std::hex << std::uppercase << (int)st;
        string hexStr = sendingTextCharacters.str();
        hexVec.push_back(hexStr);

        if (hexVec.size() == 16) { // 64-bit sind im Vektor gespeichert (16 Zeichen)
            send64bit(drv, hexVec); // an Arduino schicken; arduino ließt ein
            sendChecksum(hexVec, drv); // an Arduino schicken; arduino ließt ein

            if (awaitResponse(drv)) { // B15 ließt ports und schaut ob Arduino zufrieden ist
                // nächstes Paket schicken
            } else {
                // Paket nochmal schicken
            }

            hexVec.clear();
        }
    }
    sendingTextCharacters.str(std::string()); // stringstream leeren
}

void send64bit(B15F& drv, vector<string>& hexVec) {
    for (string hexV : hexVec) { // e.g. "1A"
        for (char hexAsChar : hexV) { // e.g. "1" and "A"
            // hier prüfen ob das selbe hintereinander gesendet wird
            // falls ja: den Wert invertieren

            drv.setRegister(&PORTA, 0x00); // LEDs to null

            cout << "hex: "<<hexAsChar<<endl;
            drv.setRegister(&PORTA, hexAsChar &0x0F); // right LEDs (for hex)
        }
    }
}

bool awaitResponse(B15F& drv) {
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

int hexTo4BitBinary(char hex) {
    // Konvertiere Hexadezimalzeichen in eine Ganzzahl
    int value;
    if (hex >= '0' && hex <= '9') {
        return value = hex - '0';
    } else if (hex >= 'A' && hex <= 'F') {
        return value = hex - 'A' + 10;
    } else if (hex >= 'a' && hex <= 'f') {
        return value = hex - 'a' + 10;
    } else {
        std::cerr << "Ungültiges Hexadezimalzeichen: " << hex << std::endl;
        return 0;
    }
}