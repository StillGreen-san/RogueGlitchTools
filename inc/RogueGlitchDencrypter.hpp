#pragma once

#include <string>
#include <vector>

std::string decrypt(const unsigned char* data, unsigned size);
std::vector<unsigned char> encrypt(const std::string& inString);
std::vector<unsigned char> upgrade(const unsigned char* data, unsigned size);
