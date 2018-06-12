#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include <gsl-lite.hpp>

class converter
{
public:
    enum class endianness
    {
        unknown,
        little, // first - least, then - most
        big,    // first - most,  then - least
        middle, // words as litle, bytes as big endian
    };

protected:
    endianness m_endian;
    endianness m_local_endian;
    bool m_swap_bytes;
    bool m_swap_words;


public:
    explicit converter(endianness order);

    static endianness get_local_endian();
    static bool need_swap_bytes(endianness from, endianness to);
    static bool need_swap_words(endianness from, endianness to);

    static converter little_order;
    static converter big_order;
    static converter middle_order;

    static uint64_t hex_to_64(const std::string&);
    static uint8_t lo_byte(uint16_t word);
    static uint8_t hi_byte(uint16_t word);
    static uint16_t lo_word(uint32_t word);
    static uint16_t hi_word(uint32_t word);
    static void split_word(uint16_t word, uint8_t& low, uint8_t& high);
    static void split_dword(uint32_t dword, uint16_t& low, uint16_t& high);
    static uint16_t make_word(uint8_t low, uint8_t high);
    static uint32_t make_dword(uint16_t low_byte, uint16_t high_byte);
    static uint16_t swap_bytes(uint16_t x);
    static uint32_t swap_words(uint32_t x);
    
    uint16_t swap(uint16_t & in) const;
    uint32_t swap(uint32_t & in) const;

    bool read_word(gsl::span<const uint8_t> in, uint16_t& out) const;
    bool read_dword(gsl::span<const uint8_t> in, uint32_t& out) const;

    template<typename T>
    bool read(gsl::span<const uint8_t> in, T& out);


    static bool join_vector(gsl::span<const uint8_t> lo_data, 
        gsl::span<const uint8_t> hi_data,
        std::vector<uint16_t>& word_data);

    bool convert_vector(gsl::span<const uint8_t> in,
        std::vector<uint8_t>& out) const;

    bool convert_vector(gsl::span<const uint8_t> in,
        std::vector<uint16_t>& out) const;

    bool convert_vector(gsl::span<const uint8_t> in,
        std::vector<uint32_t>& out) const;

};
