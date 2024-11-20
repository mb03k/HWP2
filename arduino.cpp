//
// Created by Matthes Böttcher on 18.11.24.
//
#include <iostream>

#include "arduino.h"

void start();

void setup() {
    pinMode(2, INPUT);
    pinMode(3, OUTPUT); // wird zu PS - senden? Also OUTPUT
    pinMode(4, INPUT);
    pinMode(5, INPUT);

    pinMode(6, INPUT);
    pinMode(7, OUTPUT); // wird zu PS - senden? Also OUTPUT
    pinMode(8, INPUT);
    pinMode(9, INPUT);
    Serial.begin(9600);
}

int alterWert = -1;
int neuerWert;
int counter = 1;


void loop() {
    int CLK = digitalRead(2); // CLK
    //int p3 = digitalRead(3); // ist output für PS
    int p4 = digitalRead(4);
    int p5 = digitalRead(5);

    int p6 = digitalRead(6);
    //int p7 = digitalRead(7); // ist output für PS
    int p8 = digitalRead(8);
    int p9 = digitalRead(9);

    // die letzten Pins einlesen und per Binär in int umwandeln
    neuerWert = (p4 << 1) | (p5);

    if (alterWert != neuerWert) {
        alterWert = neuerWert;

        start();
    }
}

void start() { // wird dauerhaft aufgerufen
    if (CLK == 1) { // hier werden die eigentlichen Werte gesendet

    }

    else {
        /* !!! Könnte man bestimmt mit einem int ersetzen !!! */
        if (p5 == 1) { // Anfang vom Block -> Wert ignorieren und ab jetzt "aufpassen"

        } else if (p4 == 1) { // Ende vom Block -> jetzt kommt die Prüfsumme

        }
    }
}

void sendBuffer() {
    // Den Buffer senden, wenn Prüfsumme richtig
    // Buffer sollte ein Array.. oder bitset sein?
}