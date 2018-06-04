#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class converter
{
public:

    static uint64_t hex_to_64(const std::string&);
    //static bool hex_to_bin(char ch, unsigned& val);
    //static std::vector<uint8_t> hex_to_8_array(const std::string&);

    static uint16_t make_word(uint8_t lsb, uint8_t msb);
    static uint32_t make_dword(uint16_t lsw, uint16_t msw);

    static void split_word(uint16_t word, uint8_t& lsb, uint8_t& msb);
    static void split_dword(uint32_t dword, uint16_t& lsw, uint16_t& msw);


    static bool make_word(const std::vector<uint8_t>& lsb_vector, const  std::vector<uint8_t>& msb_vector, std::vector<uint16_t>& word_vector);


};

template <typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& vec);

