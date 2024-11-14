#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace std;

std::string inputText = "Hallo Welt.   Bitte sende mich korret dqs ensdnsjknvkjdsnjcdsjcdskjnks";
bool sendinginputText = true;
int txtLength = inputText.length();
uint32_t checkSumValue;

void send64bit(string& hexStr);
void sendCheckSum(uint32_t &value);
bool arduinoSaysNextBlock();
void addCheckSumBits(unsigned int hexChar);
string get64bit(int& start);
void startProgram();

int main() {
    startProgram();
}

void startProgram() {
    bool sendPakets = true;
    int start = 0;

    while (sendPakets) {
        string inputText16chars;
        inputText16chars = get64bit(start);
        send64bit(inputText16chars);
        sendCheckSum(checkSumValue);
        cout << "Checksum: "<<checkSumValue<<endl;
        cout << "\n--------------------\n\n\n"<<endl;

        // if-Bedingung ist irrelevant -> bricht bei Rückgabe des Nullpointers von selbst ab
        if (inputText16chars.empty()) {
            cerr << "abbruch"<<endl;
            cout << "gesendet wurde: '"<< inputText16chars<<"'"<<endl;
            sendPakets = false;
        }

        // nur wenn Prüfsumme korrekt ist wird der nächste Block eingelesen
        if (arduinoSaysNextBlock()) {
            start += 16;
        }
        checkSumValue = 0;
    }
}

string get64bit(int& start) {
    if (start > inputText.length()) {
        return "";
    }

    stringstream ss;
    int readableinputTextLength = (inputText.length() - start);

    // man kann 16 Zeichen einlesen
    if (readableinputTextLength > 15) {
        int end = start + 16;

        for (int i = start; i < end; i++) {
            uint8_t v = inputText[i];
            addCheckSumBits(v);
            // einser Bits zählen
            char ch = inputText[i];
            //ss << hex << uppercase << static_cast<int>(ch);
            ss << ch;
        }
    } else { // else falls man am ende des Strings angekommen ist und keine 16 Zeichen mehr gelesen werden können
        cerr << "kleiner als 16" << endl;
        int end = (start + readableinputTextLength);
        for (int i=start; i<end; i++) {
            uint8_t v = inputText[i];
            addCheckSumBits(v);
            // einser Bits zählen
            char ch = inputText[i];
            //ss << hex << uppercase << static_cast<int>(ch);
            ss << ch; // zum Testen ob alles in richtigen Blöcken ausgegeben wird
        }
    }
    cout << ss.str() << endl;
    return ss.str();
}

bool arduinoSaysNextBlock() {
    return true;
}

void send64bit(string& hexStr) {
    int counter = 1;
    char oldCh;

    for (char ch : hexStr) {
        /*
         * Idee:
         * wenn zwei zeichen hintereinander gleich sind und gesendet werden, erst ein escape zeichen schicken
         */
        if (oldCh != ch) { // zwei unterschiedliche zeichen werden nacheinander gesendet
            oldCh = ch;
            cout << "Zeichen: \""<< ch << "\" - binär: "<< bitset<8>(ch) << " - hex: " << hex << uppercase << static_cast<int>(ch) << endl;
        } else { // zwei gleiche Zeichen werden nacheinander gesendet
            cout << "\tdoppelt - " << "1B gesendet" << endl;
            cout << "Zeichen: \""<< ch << "\"- binär: "<< bitset<8>(ch) << " - hex: "<< hex << uppercase << static_cast<int>(ch) << endl;
        }
        counter++;
    }
}

void addCheckSumBits(unsigned int hexChar) {
    std::bitset<8> bits(hexChar);
    checkSumValue += bits.count();
}

void sendCheckSum(uint32_t &value) {
    stringstream ss;
    ss << std::hex << std::uppercase << setw(2) << std::setfill('0') << value;
    string s = ss.str();
    for (char ch : s) {
        cout << "Checksum senden: '" << ch << "'" << endl;
    }
}