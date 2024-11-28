#include <iostream>
#include <iomanip>
#include <b15f/b15f.h>

using namespace std;
void sendHex(vector<string> sc, B15F& drv);
void sendChecksum(vector<string> hexVec, B15F& drv);
unsigned char charToHex(char hexChar);
void send64bit(B15F& drv, string& hexStr);
int ard_calculateChecksum(int block64bit);
bool ard_compareChecksum(int geleseneChecksum, int calculatedChecksum);
void setAndSend64bitBlock(B15F& drv, string& textToSend);
int countBits(unsigned int hexValue);
void calculateCheckSum();

bool arduinoSaysNextBlock(B15F& drv, int checksum);

string get64bit(int start, string& text);
string escapeInHex = "1B";


int main() {
    B15F& drv = B15F::getInstance();
    drv.setRegister(&DDRA, 0x0F); // Bit 0-7 Ausgabe // !!!! WICHTIG
    /*!!!WICHTIG!!!
     * setRegister(&DDRA, value) -> der Value ist bei 0x00 zum lesen gesetzt. Jetzt kann man auch getRegister benutzen
     *                           -> wenn Value auf 0xFF ist, kann man auf die Pins schreiben
     * */
    drv.setRegister(&PORTA, 0x00);

    drv.setRegister(&PORTA, 0xFF);
    drv.delay_ms(1000);
    drv.setRegister(&PORTA, 0x00);
    drv.delay_ms(500);
    drv.setRegister(&PORTA, 0xFF);
    drv.delay_ms(1000);
    drv.setRegister(&PORTA, 0x00);
    drv.delay_ms(500);

    /*
     * Prüfsumme berechnen - zwei Ansätze:
     * 1.) zyklische Redundanzprüfung
     * 2.) Bits zählen - nur die, die eins sind
     */

    // text einlesen
    string testText = "3333333333ICH HABE ES EINFACH AHAHAHHHA MNAOIFNUDESBVK!!!!!";

    bool sendPakets = true;
    int start = 0;

    /*
     * Logik hinter allem
     * solange man neue Zeichen einlesen kann, werden:
     *  1) 16 oder weniger Zeichen (in hex) eingelesen
     *  2) die Zeichen gesendet
     *  3) gewartet, ob der Arduino das Paket richtig empfangen hat
     *  4) Start bei 1)
     * */
    while (sendPakets) {
        string text16chars;
        text16chars = get64bit(start, testText);
        send64bit(drv, text16chars);
        //sendChecksum();

        // if-Bedingung ist irrelevant -> bricht bei Rückgabe des Nullpointers von selbst ab
        if (text16chars.empty()) {
            cerr << "abrruch"<<endl;
            sendPakets = false;
        }

        // nur wenn Prüfsumme korrekt ist wird der nächste Block eingelesen
        if (arduinoSaysNextBlock(drv, 100)) {
            start += 16;
        }
    }

    arduinoSaysNextBlock(drv, 100);

    drv.setRegister(&PORTA, 0x00);
}

string get64bit(int start, string& text) {
    if (start > text.length()) {
        return nullptr;
    }

    stringstream ss;
    int readableTextLength = (text.length() - start);
    cout << "txt.l() "<<text.length()<<" - start "<<start<<endl;

    // man kann 16 Zeichen einlesen
    if (readableTextLength > 15) {
        int end = start + 16;

        for (int i = start; i < end; i++) {
            // einser Bits zählen
            cout << "i: " << i << endl;
            char ch = text[i];
            //ss << hex << uppercase << static_cast<int>(ch);
            ss << ch;
        }
    } else { // else falls man am ende des Strings angekommen ist und keine 16 Zeichen mehr gelesen werden können
        cerr << "kleiner als 16" << endl;
        int end = (start + readableTextLength);
        for (int i=start; i<end; i++) {
            // einser Bits zählen
            cout << "i: " << i << endl;
            char ch = text[i];
            //ss << hex << uppercase << static_cast<int>(ch);
            ss << ch; // zum Testen ob alles in richtigen Blöcken ausgegeben wird
        }
    }

    cout << ss.str() << endl;
    return ss.str();
}

void send64bit(B15F& drv, string& hexStr) {
    int counter = 1;
    char oldCh;

    for (char ch : hexStr) {
        //drv.setRegister(&PORTA, 0x00);
        //drv.delay_ms(20);

        /*
         * Idee:
         * wenn zwei zeichen hintereinander gleich sind und gesendet werden, erst ein escape zeichen schicken
         */
        if (oldCh != ch) { // zwei unterschiedliche zeichen werden nacheinander gesendet
            oldCh = ch;
            cout << "Counter: "<< counter << " - hex: "<< bitset<4>(ch) <<endl;
            drv.setRegister(&PORTA, ch &0x0F);
            drv.delay_ms(50);
        } else { // zwei gleiche Zeichen werden nacheinander gesendet
            cout << "ESC - ";
            drv.setRegister(&PORTA, escapeInHex[0] &0x0F);
            drv.delay_ms(50);
            drv.setRegister(&PORTA, escapeInHex[1] &0x0F);
            drv.delay_ms(50);

            cout << "Counter: "<< counter << " - hex: "<< bitset<4>(ch) <<endl;
            drv.setRegister(&PORTA, ch &0x0F);
            drv.delay_ms(50);
        }
        counter++;
    }
}

// prüfen ob das Paket richtig empfangen wurde (per Prüfsumme)
bool arduinoSaysNextBlock(B15F& drv, int calculatedChecksum) {
    uint8_t recievedChecksum = drv.getRegister(&PINA);
    cout << "Wert: " << bitset<8>(recievedChecksum) << endl;

    return (calculatedChecksum == recievedChecksum);
}

void sendChecksum() {

}

int countBits(unsigned int hexValue) {
    int count = 0;
    while (hexValue) {
        count += hexValue & 1; // Prüfen, ob das niedrigste Bit 1 ist
        hexValue >>= 1;        // Eine Position nach rechts verschieben
    }
    return count;
}


/*
 * ////Alter Ansatz////
 * void sendChecksum(vector<string> hexVec, B15F& drv) {
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
}*/

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

void calculateCheckSum() {

}







void setAndSend64bitBlock(B15F& drv, std::string& textToSend) {
    std::stringstream sendingTextCharacters;
    int counter = 1;
    for (char st : textToSend) { // zeichen für zeichen
        counter++;
        // char zu hex
        std::cout << "aktuelles Zeichen: " << "'" << st << "'" << endl;

        sendingTextCharacters << std::hex << std::uppercase << static_cast<int>(st) << ' ';
        string hexStr = "";
        hexStr = sendingTextCharacters.str();

        std::cout << "zeichen in Hex: " << "'" << hexStr << "'" << endl;
        std::cout << "--------------------------------------------------" << std::endl;


        if (counter == 16) { // 64-bit sind im Vektor gespeichert (16 Zeichen)
            send64bit(drv, hexStr); // an Arduino schicken; arduino ließt ein
            /*sendChecksum(hexVec, drv); // an Arduino schicken; arduino ließt ein

            if (awaitResponse(drv)) { // B15 ließt ports und schaut ob Arduino zufrieden ist
                // nächstes Paket schicken
            } else {
                // Paket nochmal schicken
            }*/

            counter = 1;
        }
    }
    sendingTextCharacters.str(std::string()); // stringstream leeren
}
