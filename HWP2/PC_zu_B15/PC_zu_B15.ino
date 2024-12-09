void start();
void sendInt(int val);
void weitersenden();

void setup() {

  DDRD = DDRD | 0b11100000; // 111 - Daten schreiben, 000 - Daten empfangen, 00 - irrelevante Pins 
  DDRB = DDRB | 0b00000010; // 000000 - irrelevante Pins, 0 - PS empfangen, 1 - PS senden

  pinMode(8, INPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);

  Serial.begin(57600);
}

int neuerWert = 0;
int alterWert = -1;
int counter = 0;

int CS_PIN = 8;
int CLK_PIN = 7;
int MSB_PIN = 6;
int LSB_PIN = 5;

unsigned long startTime = 0;

void loop() {
  int input = digitalRead(CS_PIN);
  
    if (input == HIGH) {
        Serial.write(64);
    }
  
    if (Serial.available() > 0) {  // Prüfen, ob Daten verfügbar sind
        int received = Serial.read();  // Ein Byte empfangen
        int value = received - '0';  // ASCII-Wert in Integer umwandeln

        // ich prüfe nicht auf doppelt gesendete Zeichen da das alles das B15 Board macht
        digitalWrite(CLK_PIN, (value >> 2) & 0b1);
        digitalWrite(MSB_PIN, (value >> 1) & 0b1);  // MSB
        digitalWrite(LSB_PIN, value & 0b01);  // LSB

        Serial.println(received, BIN);
    }

    int received = Serial.read();
    if (received == 1) {
        digitalWrite(CS_PIN, HIGH);
    }
}
