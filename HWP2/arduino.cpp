/*void start() {
    pinMode(2, INPUT); // vom B15 lesen
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);

    pinMode(6, OUTPUT); // zum B15 schreiben
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
}

void loop() {
    /*
     * Arduino
     * Bearbeitet DDRD und DDRB zum verwenden - maskiert alle relevanten Pins
     * -> auch für DDRB, welche zwei aus DDRD und DDRB benutzt
     *//*
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

}*/