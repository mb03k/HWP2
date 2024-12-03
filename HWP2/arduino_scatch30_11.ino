
void start();
void sendInt(int val);
void weitersenden();

void setup() {
  DDRD &= 0b11110000;

  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);

  Serial.begin(57600);
}

int alterWert = -1;
int neuerWert = 0;
int counter = 0;
int pruefsumme = 0;
bool pruefsummeLesen = false;
int werteArray[40]; // die letzten 4 Plätze sind für Prüfsumme
int pruefsummeArray[4];
int pruefsummeArrayCounter = 0;

void loop() {
  // eingelesenen Wert in 4 Bits umwandeln (nur die relevanten anschauen)
  neuerWert = PIND; // & 0b1111000;
  neuerWert >>= 2;
  neuerWert &= 0b1111;
  
  // wenn CLK auf eins ist einlesen -> funktioniert auch
  //if (neuerWert >= 8) {
  
//Serial.write(43);

  // wert hat sich geändert
  if (alterWert != neuerWert) {
      alterWert = neuerWert;
     //Serial.println(neuerWert, BIN);
     start();
  }
}





void start() {
  // wenn CLK == 1
  if (neuerWert >= 8) {
    if (!pruefsummeLesen) {
      // letzten zwei einsen Zählen und aufaddieren
      int relevanteBits = neuerWert & 0x3;
      int anzahlEinsen = __builtin_popcount(relevanteBits);
      pruefsumme += anzahlEinsen;

      werteArray[counter] = relevanteBits;
      Serial.print("DDRD: ");
      Serial.println(neuerWert, BIN);
      Serial.print("Counter: ");
      Serial.println(counter);
weitersenden();
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
        
        werteArray[counter] = relevanteBits;
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
}

void weitersenden() {

  DDRD |= 0b11000000;     // pin 6, 7 as output, pin 2-5 as input

  DDRB |= 0b00000011;     // pin 8,9 as output

  uint8_t data = (PIND &  0b00111100) >> 2;     // data von pins 2-5 lesen und schift, andere Methoden haben schon data?

  PORTD = (PORTD & 0b00111111) | ((data & 0b0011) << 6);   // writes bit 0, 1 onto pin 6,7

  PORTB = (PORTB & 0b11111100) | ((data & 0b1100) >> 2);   // writes bit 2,3 onto pin 8,9
  
}