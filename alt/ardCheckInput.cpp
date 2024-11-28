#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

/*void setup() {
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);
    pinMode(6, INPUT);
    pinMode(7, INPUT);
    pinMode(8, INPUT);
    pinMode(9, INPUT);
    pinMode(10, INPUT);
    Serial.begin(9600);
}*/

string testText = "eins 1111B11  Bbb1";
int alterWert = -1;
int neuerWert;
int counter = 1;
std::stringstream inputTextESC;

void loop() {
}

void writeToB15() {
    bool einsDavor = false;

    for (char ch : testText) {
        char neuerWert = 'a';
        switch (ch) {
            case '1':
                if (einsDavor) {
                    //writeValuesToOtherB15('1');
                    inputTextESC << ch;
                    cout << "1";
                }
                einsDavor = true;
                break;
            case 'B':
                if (!einsDavor) {
                    //writeValuesToOtherB15('B');
                    inputTextESC << ch;
                    cout << "B";
                }
                einsDavor = false;
                break;
            case 'C':
                if (einsDavor) {
                    // hier prüfsumme einlesen
                } else {
                    //writeValuesToOtherB15('1');
                    inputTextESC << ch;
                }
                break;
            default:
                if (einsDavor) {
                    //writeValuesToOtherB15('1');
                    cout << "1";
                }
                //writeValuesToOtherB15(ch);
                cout << ch;
                einsDavor = false;
        }
        if (einsDavor) {
            //writeValuesToOtherB15(ch);
            cout << "1";
        }
    }
}

void schreiben(char x);

void aktuellsterVersuch() {
    bool einsDavor = false;

    for (char ch : testText) {
        switch (ch) {
            case '1':
                if (einsDavor) {
                    schreiben('1');
                } else {
                    einsDavor = true;
                }
                break;

            case 'B':
                if (einsDavor) { // doppeltes Zeichen gelesen
                    einsDavor = false;
                } else {
                    schreiben('B');
                }
                break;

            case 'C': // jetzt kommt die Prüfsumme
                if (einsDavor) {
                    einsDavor = false;
                } else {
                    schreiben('C');
                }
                break;

            default:
                if (einsDavor) {
                    einsDavor = false;
                    schreiben('1');
                    schreiben(ch);
                } else {
                    schreiben(ch);
                }
                break;
        }
    }
    if (einsDavor) {
        cout << '1';
    }
}

void schreiben(char x) {
    cout << x;
}