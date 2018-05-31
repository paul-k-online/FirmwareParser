#include "ascii_hex_firmware.h"
#include <fstream>
#include <iostream>
#include <iterator>

bool ascii_hex_firmware::read_ascii_file(const std::string& file_path, std::vector<uint8_t>& data)
{
    auto result = false;
    
    std::ifstream fw_file(file_path, std::fstream::in);
    if (fw_file.is_open())
    {
        char stx;
        fw_file >> stx;
        while (fw_file.good())
        {
            unsigned x;
            //fw_file.width(2);
            fw_file >> std::hex >> x;
            if (fw_file.good())
            {
                result = true;
                data.push_back(x);
            }
        }
    }
    return result;
}

bool ascii_hex_firmware::parse_ascii(const std::vector<uint8_t>& raw_data)
{
    m_data.clear();
    size_t offset = 0;
    auto sgn = make_word(raw_data[0], raw_data[1]);
    offset += 2;
    offset += 8*2;

    auto read_block = true;
    while (read_block)
    {
        read_block = false;
        
        // read address
        const auto address_msw = make_word(raw_data[offset], raw_data[offset+1]);
        offset += 2;
        if (offset == raw_data.size())
            break;
        const auto address_lsw = make_word(raw_data[offset], raw_data[offset + 1]);
        offset += 2;
        const auto address = make_dword(address_lsw, address_msw);

        // read length data
        const auto length = make_word(raw_data[offset], raw_data[offset + 1]);
        offset += 2;
        if (offset == raw_data.size())
            break;

        // read data
        if (offset + length*2 >= raw_data.size())
            break;
        std::vector<uint16_t> data;
        data.resize(length);
        for (auto i = 0; i < length; ++i)
        {
            auto d = make_word(raw_data[offset + i * 2], raw_data[offset + i * 2 + 1]);
            data.push_back(d);
        }
        offset += length * 2;

        m_data.try_emplace(address, data);

        read_block = true;
    }
    return read_block;
}


bool ascii_hex_firmware::parse_ascii_file(const std::string& file_path)
{
    std::vector<uint8_t> data;
    auto result = read_ascii_file(file_path, data);
    return parse_ascii(data);
}






template <typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& vec)
{
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(os));
    return os;
}



uint16_t ascii_hex_firmware::make_word(uint8_t lsb, uint8_t msb)
{
    return lsb | msb << 8;
}

uint32_t ascii_hex_firmware::make_dword(uint16_t lsw, uint16_t msw)
{
    return  lsw | msw << 16;
}

bool ascii_hex_firmware::split_word(uint16_t word, uint8_t & lsb, uint8_t & msb)
{
    return false;
}

bool ascii_hex_firmware::split_dword(uint32_t dword, uint16_t & lsw, uint16_t & msw)
{
    return false;
}
