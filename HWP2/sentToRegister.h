//
// Created by Matthes Böttcher on 27.11.24.
//

#ifndef HWP24_11_STRINGTOHEX_H
#define HWP24_11_STRINGTOHEX_H

#include <iostream>
#include <b15f/b15f.h>

void convertStringToHex(B15F& drv, std::string& input);
void writeMSBtoRegister(B15F& drv, char ch, int msb);
void writeLSBtoRegister(B15F& drv, char ch, int lsb);
void sendLowCLK(B15F& drv);
void sendBlockStartSequence(B15F& drv);
void sendBlockEndSequence(B15F& drv);
void delay100(B15F& drv);


#endif //HWP24_11_STRINGTOHEX_H
