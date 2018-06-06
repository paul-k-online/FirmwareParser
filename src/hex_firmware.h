#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <list>
#include "converter.h"

template<typename DataType>
class hex_firmware
{
protected:
    bool m_load_valid = false;
    std::map<uint32_t, std::vector<DataType>> m_data_map;

public:
    virtual ~hex_firmware() = default;
    virtual bool load(const std::string & filename) = 0;
    virtual bool save(const std::string & filename) = 0;

    std::map<uint32_t, std::vector<DataType>> & data_map();
    const std::map<uint32_t, std::vector<DataType>> & const_data_map() const;
    size_t total_size();

    bool compact();
    bool shrink();

    static bool join_vectors(std::vector<DataType> & dest, int index,
        const std::vector<DataType> & src, DataType fill = DataType(-1));
};


template <typename DataType>
std::map<uint32_t, std::vector<DataType>>& hex_firmware<DataType>::data_map()
{
    return m_data_map;
}

template <typename DataType>
const std::map<uint32_t, std::vector<DataType>>& hex_firmware<DataType>::const_data_map() const
{
    return m_data_map;
}

template <typename DataType>
size_t hex_firmware<DataType>::total_size()
{
    size_t sum = 0;
    for (const auto& data : m_data_map) {
        sum += data.second.size();
    }
    return sum;
}

template <typename DataType>
bool hex_firmware<DataType>::compact()
{
    //for (auto& block : m_data_map) {
    //    // start of entry is inside an block
    //    if (address >= block.first  &&  address <= block.first + block.second.size()) {
    //        find_block = true;
    //        join_vectors(block.second, address - block.first, entry.const_data());
    //        break;
    //    }
    //    // end of entry is inside an block
    //    if (address < block.first  &&  address + entry.const_data().size() >= block.first) {
    //        find_block = true;
    //        join_vectors(block.second, address - block.first, entry.const_data());
    //        break;
    //    }
    //}

    auto prev = m_data_map.begin();
    auto curr = prev;

    while (++curr != m_data_map.end()) {
        if (curr->first == prev->first + prev->second.size()) {
            prev->second.insert(prev->second.end(), curr->second.begin(), curr->second.end());
            m_data_map.erase(curr);
            curr = prev;
        }
        prev = curr;
    }
    return true;
}

template <typename DataType>
bool hex_firmware<DataType>::shrink()
{
    throw;
}

template <typename DataType>
bool hex_firmware<DataType>::join_vectors(std::vector<DataType>& dest, int index, 
    const std::vector<DataType>& src, DataType fill)
{
    if (index >= 0) {
        auto add_size = int(src.size()) - (int(dest.size()) - index);
        if (add_size > 0)
            dest.insert(dest.end(), add_size, fill);
        std::copy(src.begin(), src.end(), dest.begin() + index);
    }
    else {
        auto last = int(src.size()) + index - int(dest.size());
        if (last < 0)
            last = 0;

        auto add_size = -index + last;
        if (add_size > 0)
            dest.insert(dest.begin(), add_size, fill);
        std::copy(src.begin(), src.end(), dest.begin());
    }
    return true;
}


