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

    static uint16_t make_word(uint8_t msb, uint8_t lsb);
    static bool make_word(const  std::vector<uint8_t>& msb_vector, const std::vector<uint8_t>& lsb_vector, std::vector<uint16_t>& word_vector);
    //static std::vector<uint16_t> make_word(const  std::vector<uint8_t> msb_vector, const  std::vector<uint8_t> lsb_vector)
};


