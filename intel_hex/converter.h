#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class converter
{
public:

    static uint64_t hex_to_64(const std::string&);
    static bool hex_to_bin(char ch, unsigned& val);
    static std::vector<uint8_t> hex_to_8_array(const std::string&);
};


