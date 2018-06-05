#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class converter
{
public:
    enum class endianness
    {
        little, // first - least, then - most
        big     // first - most,  then - least
    };

protected:
    endianness m_byte_order = endianness::little;
    endianness m_word_order = endianness::little;

public:
    explicit converter(endianness byte_order = endianness::little, endianness word_order = endianness::little)
        : m_byte_order(byte_order), m_word_order(word_order)
    {}

    static converter c2000;// = converter(endianness::little, endianness::big);

    static uint64_t hex_to_64(const std::string&);
    //static bool hex_to_bin(char ch, unsigned& val);
    //static std::vector<uint8_t> hex_to_8_array(const std::string&);

    static uint16_t make_word(uint8_t lsb, uint8_t msb);
    uint16_t make_word(uint8_t arr[]);

    
    static uint32_t make_dword(uint16_t lsw, uint16_t msw);

    static void split_word(uint16_t word, uint8_t& lsb, uint8_t& msb);
    static void split_dword(uint32_t dword, uint16_t& lsw, uint16_t& msw);


    static bool make_word(const std::vector<uint8_t>& lsb_vector, const  std::vector<uint8_t>& msb_vector, std::vector<uint16_t>& word_vector);

    template<typename DataType>
    static bool convert_vector(const std::vector<uint8_t> & in, std::vector<DataType>& out);


};

template <typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& vec);





