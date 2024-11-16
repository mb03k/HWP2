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

void checkStringOnSpecialCharacters();

string testText = "eins 1111B11  Bbb";
int alterWert = -1;
int neuerWert;
int counter = 1;

void loop() {
    int s2 = digitalRead(2);
    int s3 = digitalRead(3);
    int s4 = digitalRead(4);
    int s5 = digitalRead(5);
    int s6 = digitalRead(6);
    int s7 = digitalRead(7);
    int s8 = digitalRead(8);
    int s9 = digitalRead(9);

    neuerWert = (1<<s6) | (2<<s7) | (3<<s8) | (4<<s9); // die letzten Pins einlesen und per Binär in int umwandeln

    // wird nur ausgeführt falls sich der Wert geändert hat (nicht der selbe eingelesen wurde)
    if (neuerWert != alterWert) {
        alterWert = neuerWert;
        Serial.print("Counter: ");
        Serial.print(counter);
        Serial.print(" - ");

        Serial.print(s2);
        Serial.print(s3);
        Serial.print(s4);
        Serial.print(s5);

        Serial.print(" ");

        Serial.print(s6);
        Serial.print(s7);
        Serial.print(s8);
        Serial.print(s9);

        Serial.println();
        counter++;
    }

    checkStringOnSpecialCharacters();
}

void checkStringOnSpecialCharacters() {
    bool einsDavor = false;

    for (char ch : testText) {
        switch (ch) {
            case '1':
                if (einsDavor) {
                    writeValuesToOtherB15('1');
                    //cout << "1";
                }
                einsDavor = true;
                break;
            case 'B':
                if (!einsDavor) {
                    writeValuesToOtherB15('B');
                    //cout << "B";
                }
                einsDavor = false;
                break;
            default:
                if (einsDavor) {
                    writeValuesToOtherB15('1');
                    //cout << "1";
                }
                writeValuesToOtherB15(ch);
                //cout << ch;
                einsDavor = false;
        }
    }
}



void loop() {
    int s2 = digitalRead(2);
    int s3 = digitalRead(3);
    int s4 = digitalRead(4);
    int s5 = digitalRead(5);
    int s6 = digitalRead(6);
    int s7 = digitalRead(7);
    int s8 = digitalRead(8);
    int s9 = digitalRead(9);

    neuerWert = (1<<s6) | (2<<s7) | (3<<s8) | (4<<s9);

    /*Serial.print("alter Wert: ");
    Serial.print(alterWert, BIN);*/

    if (neuerWert != alterWert) {
        Serial.print("Counter: ");
        Serial.print(counter);
        Serial.print(" - ");
        alterWert = neuerWert;
        Serial.print(s2);
        Serial.print(s3);
        Serial.print(s4);
        Serial.print(s5);
        Serial.print(" ");
        Serial.print(s6);
        Serial.print(s7);
        Serial.print(s8);
        Serial.print(s9);
        Serial.println();
        counter++;
    }

    /*Serial.print("\nneuer Wert: ");
    Serial.print(neuerWert, BIN);
    Serial.println("\n");*/
}

