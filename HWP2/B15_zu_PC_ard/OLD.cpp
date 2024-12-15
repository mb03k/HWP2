void start();
void sendInt(int val);
void weitersenden();

void setup() {
    /*DDRD &= ~0b11000111; // Pins 6 und 7 ebenfalls auf Eingang setzen
    pinMode(9, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);*/

    DDRD = DDRD | 0b11100000; // 111 - Daten schreiben, 000 - Daten empfangen, 00 - irrelevante Pins
    //DDRB = DDRB | 0b00000001; // 000000 - irrelevante Pins, 0 - PS empfangen, 1 - PS senden

    pinMode(9, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(5, OUTPUT);

    Serial.begin(57600);
}

int neuerWert = 0;
int alterWert = -1;
int counter = 0;

unsigned long startTime = 0;

void loop() {
    unsigned long currentTime = millis();
    neuerWert = PIND & 0b00111100;
    neuerWert = neuerWert >> 2;

    if (neuerWert != alterWert) {
        Serial.write(neuerWert); // PC empfängt die Daten
        alterWert = neuerWert;
    }

    if (Serial.available() >= sizeof(int)) {
        // Lesen eines Integers aus der seriellen Schnittstelle
        int value = 0;
        Serial.readBytes((char*)&value, sizeof(value));

        if (value >= 400) {
            digitalWrite(9, HIGH);
            startTime = currentTime;
        }
    }
    if (currentTime - startTime >= 250) {
        digitalWrite(9, LOW);
    }
}




/*int counter = 0;
int pruefsumme = 0;
bool pruefsummeLesen = false;*/

/*void start() {
  // wenn CLK == 1
  if (neuerWert >= 8) {
    if (!pruefsummeLesen) {
      // letzten zwei einsen Zählen und aufaddieren
      int relevanteBits = neuerWert & 0x3;
      int anzahlEinsen = __builtin_popcount(relevanteBits);
      pruefsumme += anzahlEinsen;

      Serial.print("DDRD: ");
      Serial.println(neuerWert, BIN);
      Serial.print("Counter: ");
      Serial.println(counter);
      Serial.println();

      Serial.println("--------------");
      counter++;
    } else {
        // letzten zwei einsen Zählen und aufaddieren
        int relevanteBits = neuerWert & 0x3;

        Serial.print("DDRD: ");
        Serial.println(neuerWert, BIN);
        Serial.print("PS - zwei Bits: ");
        Serial.println(relevanteBits);
        Serial.print("PS - zwei Bits: ");
        Serial.println(relevanteBits);

        counter++;

        Serial.println();
        Serial.println("--------------");
    }
  }

  else {
    // 0001 -> Anfang von neuem Block
    if (neuerWert == 1) {

      counter = 0;
      pruefsumme = 0;
      pruefsummeLesen = false;
      Serial.println("\n");
      Serial.println("!!!!!!!!!!!!!!!!!!!!!");
      Serial.print("ANFANG BLOCK: ");
      Serial.println(neuerWert);
      Serial.println("!!!!!!!!!!!!!!!!!!!!!");
      Serial.println("\n");
    }

    // 0010 -> Ende vom Block, jetzt Prüfsumme
    else if (neuerWert == 2) {
      Serial.println("\n");
      Serial.println("*********************");
      Serial.print("ENDE BLOCK - gelesene Prüfsumme beträgt: ");
      Serial.println(pruefsumme);
      Serial.println("*********************");
      Serial.println("\n");

      Serial.print("WerteArray:");
      pruefsummeLesen = true;
    }
  }
}*/
