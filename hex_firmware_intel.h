#pragma once
#include <list>
#include <string>
#include <map>
#include <list>
#include <fstream>
#include <sstream>
#include <iostream>

#define NOMINMAX
#include <algorithm>

#include "hex_entry_intel.h"
#include "hex_firmware.h"


template <typename DataType>
class hex_firmware_intel : protected hex_firmware<DataType>
{
    std::list<hex_entry_intel> m_entry_list {};

public:
    std::list<hex_entry_intel> const_entry_list() const;
    bool load(const std::string& filename) override;
    bool parse(const std::string& filename);

    static bool parse(const std::string& filename, 
                       std::list<hex_entry_intel> &entry_list);

    static bool to_map(const std::list<hex_entry_intel>& entries,
                       std::map<uint32_t, std::vector<DataType>>& blocks);
};


template <typename DataType>
std::list<hex_entry_intel> hex_firmware_intel<DataType>::const_entry_list() const
{
    return m_entry_list;
}

template <typename DataType>
bool hex_firmware_intel<DataType>::load(const std::string& filename)
{
    auto result = true;

    if (result) {
        result = parse(filename, m_entry_list);
    }

    if (result) {
        result = to_map(m_entry_list, m_data);
    }

    if (result)
        result = compact(m_data);

    m_load_valid = result;
    return result;
}

template<typename DataType>
bool hex_firmware_intel<DataType>::parse(const std::string & filename)
{
    auto result = parse(filename, m_entry_list);

}

template <typename DataType>
bool hex_firmware_intel<DataType>::parse(const std::string& filename,
    std::list<hex_entry_intel>& entry_list)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        return false; //o << "Cannot load file " << filename;
    }

    while (file) {
        std::string line;
        std::getline(file, line);
        if (!line.empty()) {
            hex_entry_intel entry(line);
            if (!entry.is_valid())
                return false;
            entry_list.push_back(entry);
        }
    }
    return true;

}

template <typename DataType>
bool hex_firmware_intel<DataType>::to_map(const std::list<hex_entry_intel>& entries,
                                          std::map<uint32_t, std::vector<DataType>>& blocks)
{
    uint32_t address_msw = 0;
    uint32_t address;

    for (const auto& entry : entries) {
        switch (entry.record_type()) {

        case hex_entry_intel::Record_Type::extended_linear_address:
            if (entry.address() == 0 && entry.const_data().size() == 2)
                address_msw = converter::make_word(entry.const_data()[1], entry.const_data()[0]);
            break;

        case hex_entry_intel::Record_Type::data:
            address = converter::make_dword(entry.address(), address_msw);
            blocks.try_emplace(address, entry.const_data());
            break;

        default:
            break;
        }
    }
    return true;
}