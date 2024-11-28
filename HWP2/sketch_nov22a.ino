
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

  // wert hat sich geändert
  if (alterWert != neuerWert) {
      alterWert = neuerWert;

      /*
     * Arduino
     * Bearbeitet DDRD und DDRB zum verwenden - maskiert alle relevanten Pins
     * -> auch für DDRB, welche zwei aus DDRD und DDRB benutzt
     */
      // Liest beide Register ein
      uint8_t DDRD_READIN = 0b00111100;
      uint8_t DDRB_READIN = 0b00000011;

      // maskiert die relevanten Bits - die man auch braucht
      uint8_t DDRD = (DDRD_READIN >> 2) & 0b1111;
      uint8_t DDRB_INPUT = DDRB_READIN & 0b11;

      // DDRB besteht aus zwei Teilen -> DDRD (Pin 6 und 7), DDRB (Pin 8 und 9)
      uint8_t DDRB_MSB = DDRB_INPUT & 0b11; // ersten zwei Pins
      uint8_t DDRB_LSB = (DDRD_READIN >> 6) & 0b11; // letzte zwei Pins
      uint8_t DDRB = (DDRB_MSB << 2) | DDRB_LSB;

      std::cout << "DDRB: " << std::bitset<4>(DDRB) << std::endl;
      std::cout << "DDRD: " << std::bitset<4>(DDRD) << std::endl;

      // CLK
      uint8_t CLK_DDRD = (DDRD >> 3) & 0b1;
      uint8_t CLK_DDRB = (DDRB >> 3) & 0b1;
      std::cout << "CLK DDRD: " << std::bitset<1>(CLK_DDRD) << std::endl;
      std::cout << "CLK DDRB: " << std::bitset<1>(CLK_DDRB) << std::endl;

      // IMPORTANT BITS (letzten zwei)
      uint8_t DDRD_impPins = DDRD & 0b11;
      uint8_t DDRB_impPins = DDRB & 0b11;
      std::cout << "impPins DDRD: " << std::bitset<2>(DDRD_impPins) << std::endl;
      std::cout << "impPins DDRB: " << std::bitset<2>(DDRB_impPins) << std::endl;


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
  digitalWrite(9, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(7, LOW);
  digitalWrite(6, HIGH);
  
  digitalWrite(9, LOW);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(6, LOW);

  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);
}
