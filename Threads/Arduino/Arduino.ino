/*
 * Muss können:
 * - Daten von PC zum B15 senden
 * - Daten von B15 zum PC senden
 * - Prüfsumme - LED anschalten falls PC falsche Daten erkannt hat
 * - PC sagen dass B15 falsche Prüfsumme erkannt hat (LED einlesen)
 */

void start();
void sendInt(int val);

void setup() {
    // 1->output : 0->input
    DDRD = DDRD | 0b11100000; // 111-PC zu B15, 000-B15 zu PC, 00-irrelevante Pins
    DDRB = DDRB | 0b00000010; // 000000 - irrelevante Pins, 1-PS für PC, 0-PS für B15

    pinMode(9, OUTPUT);
    pinMode(8, INPUT);
    pinMode(7, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(5, OUTPUT);

    Serial.begin(57600);
}

/* werte die vom PC gesendet wurden */
int PCtoB15_newValue = 0;
int PCtoB15_oldValue = -1;

/* werte die vom B15 gesendet wurden */
int B15toPC_newValue = 0;
int B15toPC_oldValue = -1;

unsigned long startTime = 0;

// B15 zu PC
int BtoP_CS_PIN = 8;
int BtoP_CLK_PIN = 7;
int BtoP_MSB_PIN = 6;
int BtoP_LSB_PIN = 5;

// PC zu B15
int PtoB_CS_PIN = 9;
int PtoB_CLK_PIN = 4;
int PtoB_MSB_PIN = 3;
int PtoB_LSB_PIN = 2;

bool sendCS = false;

void loop() {
    unsigned long currentTime = millis();

    /*
     * B15 zu PC
     */
    // Daten von B15 an PC senden
    B15_newValue = ((PIND & 0b00011100) >> 2); // einlesen des B15-Board's
    if (B15_oldValue != B15_newValue) { // B15 sendet neue Daten
        Serial.write(B15_newValue); // an PC die Daten des B15's senden
        B15_oldValue = B15_newValue;
    }

    // LED der Prüfsumme aus machen
    if (currentTime - startTime >= 250) {
        digitalWrite(PtoB_CS_PIN, LOW);
    }

    /*
     * PC zu B15
     */
    bool PCtoB15_checkSum = digitalRead(PtoB);
    // prüfen ob der PC die Daten erneut senden soll
    if (PCtoB15_checkSum) {
        if (sendCS) {
            Serial.write(64);
            sendCS = false; // bestenfalls nur ein mal 64 senden
            // ansonsten erkennt die read() funktion mehrmals die falsche PS obwohl das nicht stimmt
        }
    } else {
        sendCS = true;
    }

    if (Serial.available() > 0) { // PC sendet Daten
        // !!! EVENTUELL FEHLER!!!
        // Wert ist ursprüngliche einfach nur Serial.read();
        // mit - '0' könnte er zu klein sein, unwahrscheinlich ABER muss ich testen
        int receivedData = Serial.read() - '0'; // ASCII in Int umwandeln

        // B15 zu PC
        if (receivedData >= 400) { // falsche Prüfsumme wurde erkannt, PC sendet Zeichen
            digitalWrite(9, HIGH); // LED am B15 setzen
            startTime = currentTime; // timer um LED auszuschalten
        }

        // PC zu B15
        PCtoB15_newValue = receivedData - '0';  // ASCII-Wert in Integer umwandeln
        if (PCtoB15_oldValue != PCtoB15_newValue) {
            PCtoB15_oldValue = PCtoB15_newValue;

            digitalWrite(CLK_PIN, (PCtoB15_newValue & 0b100)); // CLK
            digitalWrite(MSB_PIN, (PCtoB15_newValue & 0b010));  // MSB
            digitalWrite(LSB_PIN, (PCtoB15_newValue & 0b0001));  // LSB
        }
    }


}