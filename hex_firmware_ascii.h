#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <iterator>

#include "hex_firmware.h"
#include "converter.h"


template <typename DataType>
class hex_firmware_ascii : public hex_firmware<DataType>
{
    static const char STX = 0x02; //"\x02"
    static const char ETX = 0x03; //"\x03"
    static const uint8_t first = 0xAA;
    static const uint8_t second = 0x08;

public:
    bool load(const std::string& file_name) override;
    bool save(const std::string& file_name) override;

    static bool read(const std::string & file_path, std::vector<uint8_t>& data);
    static bool parse(const std::vector<uint8_t>& bytes, std::map<uint32_t, std::vector<DataType>> &data_map);
};


template <typename DataType>
bool hex_firmware_ascii<DataType>::load(const std::string& file_name)
{
    std::vector<uint8_t> bytes;
    auto result = read(file_name, bytes);
    if (result)
        result = parse(bytes, m_data);

    return result;
}

template <typename DataType>
bool hex_firmware_ascii<DataType>::save(const std::string& file_name)
{
    throw;
}


template <typename DataType>
bool hex_firmware_ascii<DataType>::read(const std::string& file_path, std::vector<uint8_t>& data)
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


template<typename DataType>
bool hex_firmware_ascii<DataType>::parse(const std::vector<uint8_t>& bytes, std::map<uint32_t, std::vector<DataType>>& data_map)
{
    if (bytes.empty())
        return false;

    m_data.clear();
    size_t offset = 0;
    auto sgn = converter::make_word(bytes[0], bytes[1]);
    offset += 2;
    offset += 8 * 2;

    auto read_block = true;
    while (read_block)
    {
        read_block = false;

        // read address
        const auto address_msw = converter::make_word(bytes[offset], bytes[offset + 1]);
        offset += 2;
        if (offset == bytes.size())
            break;
        const auto address_lsw = converter::make_word(bytes[offset], bytes[offset + 1]);
        offset += 2;
        const auto address = converter::make_dword(address_lsw, address_msw);

        // read length data
        const auto length = converter::make_word(bytes[offset], bytes[offset + 1]);
        offset += 2;
        if (offset == bytes.size())
            break;

        // read data
        if (offset + length * 2 >= bytes.size())
            break;
        std::vector<uint16_t> data;
        data.resize(length);
        for (auto i = 0; i < length; ++i)
        {
            const auto lsb = bytes[offset + i * 2];
            const auto msb = bytes[offset + i * 2 + 1];
            auto d = converter::make_word(lsb, msb);
            data.push_back(d);
        }
        offset += length * 2;

        m_data.try_emplace(address, data);

        read_block = true;
    }
    return read_block;
}

