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
        unknown,
        little, // first - least, then - most
        big,    // first - most,  then - least
        middle, // 
    };

protected:
    endianness m_endian;
    endianness m_local_endian;
    bool m_swap_bytes;
    bool m_swap_words;


public:
    static endianness get_local_endian() {
        static const uint32_t x = 0x11223344;
        auto xx = *(unsigned char *)&x;
        switch (xx)
        {
        case 0x44: return endianness::little;
        case 0x11: return endianness::big;
        case 0x22: return endianness::middle;
        default: return endianness::unknown;
        }
    }
    static bool need_swap_bytes(endianness e1, endianness e2);
    static bool need_swap_words(endianness e1, endianness e2);



    explicit converter(const endianness order);

    static converter little_order;
    static converter big_order;
    static converter middle_order;

    static uint16_t swap(uint16_t x);
    static uint32_t swap(uint32_t x);
    static uint8_t hi_byte(uint16_t word);
    static uint8_t lo_byte(uint16_t word);
    static uint16_t hi_word(uint32_t word);
    static uint16_t lo_word(uint32_t word);

    void from_local(uint16_t &in);
    void to_local(uint16_t &in);

    void from_local(uint32_t &in);
    void to_local(uint32_t &in);

    static uint64_t hex_to_64(const std::string&);
    //static bool hex_to_bin(char ch, unsigned& val);
    //static std::vector<uint8_t> hex_to_8_array(const std::string&);
    
    uint16_t make_word(const uint8_t arr[2]);
    static uint32_t make_dword(uint16_t lsw, uint16_t msw);
    static uint16_t make_word(uint8_t lsb, uint8_t msb);

    static void split_word(uint16_t word, uint8_t& lsb, uint8_t& msb);
    static void split_dword(uint32_t dword, uint16_t& lsw, uint16_t& msw);

    static bool make_word(const std::vector<uint8_t>& lsb_vector, const  std::vector<uint8_t>& msb_vector, std::vector<uint16_t>& word_vector);

    template<typename DataType>
    static bool convert_vector(const std::vector<uint8_t> & in, std::vector<DataType>& out);


};


template <typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& vec);





