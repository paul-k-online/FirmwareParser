#pragma once
#include <list>
#include <string>
#include <map>
#include <list>
#include <fstream>
#include <sstream>
#include <iostream>

//#define NOMINMAX
//#include <algorithm>

#include "hex_entry_intel.h"
#include "hex_firmware.h"


template <typename DataType>
class hex_firmware_intel : protected hex_firmware<DataType>
{
    std::list<hex_entry_intel> m_entry_list {};

public:
    std::list<hex_entry_intel> & entry_list();
    const std::list<hex_entry_intel> & const_entry_list() const;
    
    bool parse(const std::string& filename);
    bool load(const std::list<hex_entry_intel> & entry_list);
    bool load(const std::string& filename) override;
    bool save(const std::string& filename) override;
};



template <typename DataType>
std::list<hex_entry_intel>& hex_firmware_intel<DataType>::entry_list()
{
    return m_entry_list;
}

template <typename DataType>
const std::list<hex_entry_intel>& hex_firmware_intel<DataType>::const_entry_list() const
{
    return m_entry_list;
}

template<typename DataType>
bool hex_firmware_intel<DataType>::parse(const std::string & filename)
{
    auto result = true;
    m_entry_list.clear();
    std::ifstream file(filename);
    
    if (result) { //"Cannot load file " filename;
        result = file.is_open();
    }

    while (result && file) {
        std::string line;
        std::getline(file, line);
        if (!line.empty()) {
            hex_entry_intel entry(line);
            result = entry.is_valid();
            if (result)
                m_entry_list.push_back(entry);
        }
    }
    if (!result)
        m_entry_list.clear();
    return result;
}

template <typename DataType>
bool hex_firmware_intel<DataType>::load(const std::list<hex_entry_intel> & entry_list)
{
    auto result = true;
    uint32_t address_msw = 0;

    for (const auto& entry : entry_list) {
        switch (entry.record_type()) {

        case hex_entry_intel::Record_Type::extended_linear_address:
            if (entry.address() == 0 && entry.const_data().size() == 2)
                address_msw = converter::make_word(entry.const_data()[1], entry.const_data()[0]);
            break;

        case hex_entry_intel::Record_Type::data: 
        {
            auto address = converter::make_dword(entry.address(), address_msw);
            std::vector<DataType> data = convert_vector(entry.const_data());
            //TODO:
            //auto dm = m_data_map;
            m_data_map.push_back(address, data);
            break;
        }
        default:
            break;
        }
    }
    result = true;
    return result;
}

template <typename DataType>
bool hex_firmware_intel<DataType>::load(const std::string& filename)
{
    auto result = parse(filename);

    if (result) {
        load(m_entry_list);
    }

    if (result) {
        result = compact();
    }

    m_load_valid = result;
    return result;
}

template <typename DataType>
bool hex_firmware_intel<DataType>::save(const std::string& filename)
{
    throw;
}


