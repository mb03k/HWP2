//
// Created by Matthes Böttcher on 24.11.24.
//

#ifndef HWP_15_11_SAVEBININFILE_H
#define HWP_15_11_SAVEBININFILE_H

#include <iostream>
#include <vector>


int saveInVector(const std::string& filepath);
std::string getChunkAsHexString(size_t offset);
int getVecSize();
void setChunk(std::vector<std::vector<unsigned char>>& newChunk);

#endif //HWP_15_11_SAVEBININFILE_H
