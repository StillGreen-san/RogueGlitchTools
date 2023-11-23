#pragma once

#include <string>
#include <vector>

std::string decrypt(unsigned char* data, unsigned size);
std::vector<unsigned char> encrypt(std::string inString);
