#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class converter
{
public:

    static uint64_t asciiHexTo64(const std::string&);
    static bool hex2bin(char ch, unsigned& val);
    static std::vector<uint8_t> hex2bin(const std::string&);
};


