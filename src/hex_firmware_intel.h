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
class hex_firmware_intel : public hex_firmware<DataType>
{
    std::list<hex_entry_intel> m_entry_list {};

public:
    std::list<hex_entry_intel> & entry_list();
    const std::list<hex_entry_intel> & const_entry_list() const;
    
    bool parse(std::istream& file);
    bool parse(const std::string& filename);
    
    bool load(const std::list<hex_entry_intel> & entry_list);
    bool load(std::istream& file) override;
    bool save(std::ostream& file) override;
    bool valid() override;
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
bool hex_firmware_intel<DataType>::parse(std::istream & file)
{
    auto result = true;
    m_entry_list.clear();
    while (result && file) {
        std::string line;
        std::getline(file, line);
        if (!line.empty()) {
            hex_entry_intel entry(line);
            result = entry.valid();
            if (result)
                m_entry_list.push_back(entry);
        }
    }
    if (!result)
        m_entry_list.clear();
    return result;

}


template<typename DataType>
bool hex_firmware_intel<DataType>::parse(const std::string & filename)
{
    std::ifstream file(filename);
    auto result = file.is_open();
    if (result) { //Cannot open file {filename};
        result = parse(file);
    }
    return result;
}


template <typename DataType>
bool hex_firmware_intel<DataType>::load(const std::list<hex_entry_intel> & entry_list)
{
    auto result = true;
    uint16_t address_msw = 0;
    
    for (const auto& entry : entry_list) {

        switch (entry.record_type()) {
        
        case hex_entry_intel::Record_Type::extended_linear_address:
            if (entry.address() == 0 && entry.const_data().size() == 2) {
                //address_msw = converter::make_word(entry.const_data()[1], entry.const_data()[0]);
                auto data = entry.const_data();
                converter::big_order.read_word(data, address_msw);
            }
            break;
        
        case hex_entry_intel::Record_Type::data: 
        {
            auto address = converter::make_dword(entry.address(), address_msw);
            std::vector<DataType> data;
            converter::big_order.convert_vector(entry.const_data(), data);
            //TODO:
            //auto dm = m_data_map;
            m_data_map.try_emplace(address, data);
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
bool hex_firmware_intel<DataType>::load(std::istream& file)
{
    auto result = parse(file);

    if (result) {
        load(m_entry_list);
    }

    if (result) {
        result = join();
    }

    m_load_valid = result;
    return result;
}


template <typename DataType>
bool hex_firmware_intel<DataType>::save(std::ostream& file)
{
    throw;
}

template <typename DataType>
bool hex_firmware_intel<DataType>::valid()
{
    throw;
}
