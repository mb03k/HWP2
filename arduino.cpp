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
}

void loop() {
    Serial.begin(9600);

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
}
