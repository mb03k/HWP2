#include <iostream>
#include <b15f/b15f.h>

void handleInput();

void setCorrectCheckSum();
void setWrongCheckSum();
void calculateBitwiseCheckSum(int& bin);
void calculateCS();
bool checkSumsAreEqual();
void writeToFile();
void writeToBin(unsigned char val);

B15F& drv = B15F::getInstance();
bool LASTBLOCK = false;
bool LASTBLOCK_CHECK = false;
bool ENDOFSENDING = false;
bool READCS = false;
bool EXIT_WHILE = false;

std::vector<std::vector<int>> werteVec;
std::vector<std::vector<int>> valVec;
std::vector<int> checkSumVec;
std::vector<int> csVec;
int calculatedCheckSum = 0; // die man mit jedem Wert eingelesen hat
int receivedCheckSum = 0; // die man zum Ende bekommen hat
bool readCheckSum = false;
int offset = 0;
int counter = 1;

void handleInput();
int binary = 0;
int bin = 0;
int switch_val = 0;

int main() {
    drv.setRegister(&DDRA, 0x47);
    drv.setRegister(&PORTA, 0x00);
    // aktueller Test:
        // PINS so anpassen, dass wir auf welchen lesen können und auf den anderen schreiben
        // -> 0b01000111
        // 7: für empfangene Daten PS schreiben
        // 6: für geschriebene Daten PS empfangen
        // 5, 4, 3: Daten von anderen PC empfangen 
        // 2, 1, 0: Daten an anderen PC senden
        //  (von MSB zu LSB)
        //  1: schreiben, 0: lesen

    int alterWert = -1;
    valVec.resize(offset+1);



    //EXIT_WHILE = true;
    while (!EXIT_WHILE) {
        uint8_t pina = drv.getRegister(&PINA);
        binary = (pina >> 3) & 0b0111;
        bin = binary & 0b011;
        switch_val = binary & 0b0111;

        if (alterWert!=binary) { // nur einen Wert einlesen
            alterWert = (int)binary;
            if ((pina > 0)) {
                //std::cerr << "port: " << std::bitset<4>(bin) << std::endl; 
                handleInput();
            }
        }
    }

    writeToFile();
}

void handleInput() {
    switch(switch_val) {
        case 1:
            // unnötig -> ABER
            // testen ob in einem Block mehr als 64 Zeichen gesendet wurde
            // -> es kann sein dass in dem Block eine '0100' gesendet wurde
            //    also dass keine neuen positiven Bits hinzugefügt wurden.
            //    Das würde aber trotzdem den Wert ändern -> wird hiermit abgefangen
            if (LASTBLOCK_CHECK) {
                LASTBLOCK = true;
            } else {
                LASTBLOCK = false;
            }
            std::cerr << "***ANFANG BLOCK***" << std::endl;
            setCorrectCheckSum();
            calculatedCheckSum = 0;
            receivedCheckSum = 0;
            counter = 0;
            csVec.clear();
            ENDOFSENDING = false;
            LASTBLOCK_CHECK = true;
            break;

        case 2:
            std::cerr << "---JETZT PS---" << std::endl;
            READCS = true;
            ENDOFSENDING = false;
            LASTBLOCK_CHECK = false;
            break;
        
        case 3: 
            if (ENDOFSENDING) {
                std::cerr << "!!!ENDE SENDEN!!!" << std::endl;
                EXIT_WHILE = true;
                break;
            }

            calculateCS();

            if (!checkSumsAreEqual() || (counter != 64 && !LASTBLOCK)) {
                std::cerr << "\tFALSCHE PS counter "<< counter << std::endl;
                valVec[offset].clear();
                std::cerr << "valVec Werte: "<<std::endl;
                for (int i : valVec[offset]) {
                    std::cerr << i << " ";
                }
                std::cerr << std::endl;
                counter = 0;
                calculatedCheckSum = 0;
                setWrongCheckSum();
            } else {
                offset++;
                valVec.resize(offset+1);
            }

            READCS = false;
            ENDOFSENDING = true;
            LASTBLOCK_CHECK = false;
            break;

        default:
            if (READCS) {
                csVec.push_back(bin);
                std::cerr << "PS: " << std::bitset<4>(binary) << " - " << std::bitset<2>(bin) << std::endl;
            } else if (binary >= 4) {

                std::cerr
                    << std::bitset<4>(binary)
                    << " - pc: "
                    << __builtin_popcount(bin)
                    << " - " 
                    << counter
                    << " - pb: " 
                    << bin 
                    << " - offset: "
                    << offset
                << std::endl;
                
                valVec[offset].push_back(bin);
                calculateBitwiseCheckSum(bin);
                ENDOFSENDING = false;
                LASTBLOCK_CHECK = false;
                counter++;
            }
    }
}

void setCorrectCheckSum() {
    std::cerr << "RICHTIGE CS SETZEN" << std::endl;
    drv.setRegister(&PORTA, 0);
}

void setWrongCheckSum() {
    std::cerr << "FALSCHE CS SETZEN" << std::endl;
    drv.setRegister(&PORTA, 64); // 64 -> 0b01000000
}

void calculateBitwiseCheckSum(int& bin) {
    calculatedCheckSum += __builtin_popcount(bin);
}

void calculateCS() {
    receivedCheckSum = 0;
    int shift = 6;

    for (int i=0; i<csVec.size(); i++) {
        receivedCheckSum = receivedCheckSum | (csVec[i] <<shift);
        shift -= 2;
    }
}

bool checkSumsAreEqual() {
    std::cerr
        << "\tberechnete Prüfsumme: "
        << calculatedCheckSum
        << " - erhaltene Prüfsumme: "
        << receivedCheckSum
    << std::endl;
    return receivedCheckSum == calculatedCheckSum;
}

void writeToFile() {
    for (std::vector<int> i : valVec) {
        for (int j=0; j<i.size(); j+=4) {
            unsigned char val = (i[j] << 6)
                            | (i[j+1] << 4)
                            | (i[j+2] << 2)
                            | i[j+3];
            std::cout.put(val);
        }
    }
}   

void writeToBin(unsigned char val) {
    std::ofstream outfile("output.bin", std::ios::binary | std::ios::app);

    // Überprüfen, ob die Datei erfolgreich geöffnet wurde
    if (!outfile) {
        std::cerr << "Fehler beim Öffnen der Datei!" << std::endl;
    }

    // Schreibe den unsigned char in die Datei
    outfile.write(reinterpret_cast<const char*>(&val), sizeof(val));

    // Überprüfen, ob das Schreiben erfolgreich war
    if (outfile.fail()) {
        std::cerr << "Fehler beim Schreiben in die Datei!" << std::endl;
    }

    // Schließe die Datei
    outfile.close();
}