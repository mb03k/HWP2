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
int received = 0;

bool sendCS = true;

void loop() {
    unsigned long currentTime = millis();
    int input = digitalRead(CS_PIN);
    
    if (Serial.available() > 0) {  // Prüfen, ob Daten verfügbar sind
        received = Serial.read();
        int value = received - '0';  // ASCII-Wert in Integer umwandeln
        

        if (alterWert != value) {
            alterWert = value;
            
            // ich prüfe nicht auf doppelt gesendete Zeichen da das alles das B15 Board macht
            digitalWrite(CLK_PIN, (value & 0b100));
            digitalWrite(MSB_PIN, (value & 0b010));  // MSB
            digitalWrite(LSB_PIN, (value & 0b0001));  // LSB
        }
    }
    
    if (input == HIGH) {
      if (sendCS) {
        Serial.write(64);
        sendCS = false;
      }
    }
    else {
        sendCS = true;
    }
}
