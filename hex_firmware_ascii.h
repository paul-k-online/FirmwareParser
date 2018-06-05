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
    uint16_t m_singature = 0;
    uint16_t m_reserved[8];

public:
    static const char STX = 0x02; //"\x02"
    static const char ETX = 0x03; //"\x03"
    static const uint8_t FIRST = 0xAA;
    static const uint8_t SECOND = 0x08;

protected:
    std::vector<uint8_t> m_bytes;
public:
    bool read(const std::string & filename);
    bool load(const std::vector<uint8_t> & bytes);
    bool load(const std::string & filename) override;
    bool save(const std::string & filename) override;
};


template <typename DataType>
bool hex_firmware_ascii<DataType>::read(const std::string & filename)
{
    auto result = true;
    std::ifstream file(filename);

    if (result) {
        result = file.is_open();
    }

    if (result) {
        char stx;
        file >> stx;
        result = stx == STX;
    }

    if (result) {
        while (file.good()) {
            unsigned x; //fw_file.width(2);
            file >> std::hex >> x;
            if (file.good()) {
                result = true;
                m_bytes.push_back(x);
            }
        }
    }

    if (result) {
        result = m_bytes[0] == FIRST && m_bytes[1] == SECOND;
    }

    if (result) {
        char etx;
        // TODO: check last char 
        //fw_file >> etx;
        //result = etx == ETX;
    }
    return result;
}

template<typename DataType>
bool hex_firmware_ascii<DataType>::load(const std::vector<uint8_t>& bytes)
{
    if (bytes.empty())
        return false;
    m_data_map.clear();
    
    auto iter = bytes.begin();
    m_singature = converter::make_word(*iter++, *iter++);
    for (auto i = 0; i < sizeof(m_reserved)/sizeof(m_reserved[0]); i++) {
        m_reserved[i] = converter::make_word(*iter++, *iter++);
    }
    

    auto read_block = true;
    while (read_block)
    {
        read_block = false;
        
        // read address
        const auto address_msw = converter::make_word(*iter++, *iter++);
        if (iter == bytes.end())
            break;
        const auto address_lsw = converter::make_word(*iter++, *iter++);
        if (iter == bytes.end())
            break;
        const auto address = converter::make_dword(address_lsw, address_msw);

        // read length data
        const auto length = converter::make_word(*iter++, *iter++);
        if (length == 0 || iter == bytes.end())
            break;

        // read data
        const auto data_size = sizeof DataType;
        const auto data_block_size = length * data_size;
        if (iter + data_block_size >= bytes.end())
            break;
        std::vector<uint8_t> in(iter, iter + data_block_size);
        std::vector<DataType> out;
        if (!converter::convert_vector<DataType>(in, out))
            break;
        m_data_map.try_emplace(address, out);

        read_block = true;
    }
    return read_block;
}

template <typename DataType>
bool hex_firmware_ascii<DataType>::load(const std::string& file_name)
{
    auto result = true;
    if (result) {
        result = read(file_name);
    }
    if (result) {
        result = load(m_bytes);
    }
    return result;
}

template <typename DataType>
bool hex_firmware_ascii<DataType>::save(const std::string& file_name)
{
    throw;
}
