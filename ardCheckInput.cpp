void setup() {
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
}

int alterWert = -1;
int neuerWert;
int counter = 1;

void loop() {
    if (digitalRead(2) == HIGH
        && digitalRead(3) == HIGH
        && digitalRead(4) == HIGH
        && digitalRead(5) == HIGH)
    {
        Serial.println("")
    }

    /*
     * Was muss das Arduino können?
     *
     * - 64 bit-block einlesen
     * - 8 bit Prüfsumme einlesen
     * - Prüfsumme berechnen
     * - Prüfsummen vergleichen (eingelesene und berechnete)
     * */

    string testText = "eins 1111B11  Bbb";
    bool einsDavor = false;

    for (char ch : testText) {
        char neuerWert = 'a';
        switch (ch) {
            case '1':
                if (einsDavor) {
                    cout << "1";
                } else {
                }
                einsDavor = true;
                break;
            case 'B':
                if (!einsDavor) {
                    //writeValuesToOtherB15('B');
                    cout << "B";
                }
                einsDavor = false;
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
    }
}
