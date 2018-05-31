#pragma once
#include <map>
#include <vector>
#include <list>
#include "ascii_hex_block.h"

class ascii_hex_firmware
{
    static const char STX = 0x02; //"\x02"
    static const char ETX = 0x03; //"\x03"

    char first = 0;
    char second = 0;

    std::map<uint32_t, std::vector<uint16_t>> m_data;

public:

    bool parse_ascii(const std::vector<uint8_t> & data);
    bool parse_ascii_file(const std::string & file_path);


    static bool read_ascii_file(const std::string & file_path, std::vector<uint8_t>& data);

    static uint16_t make_word(uint8_t lsb, uint8_t msb);
    static uint32_t make_dword(uint16_t lsw, uint16_t msw);

    static bool split_word(uint16_t word, uint8_t& lsb, uint8_t& msb);
    static bool split_dword(uint32_t dword, uint16_t& lsw, uint16_t& msw);
};

