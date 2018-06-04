#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <list>

template<typename DataType>
class hex_firmware
{
protected:
    bool m_load_valid = false;
    std::map<uint32_t, std::vector<DataType>> m_data;

    //struct hex_entry
    //{
    //    uint32_t m_address = 0;
    //    std::vector<DataType> m_data{};
    //};

public:
    virtual bool load(const std::string & filename) = 0;
    virtual bool save(const std::string & filename) = 0;

    const std::map<uint32_t, std::vector<DataType>>& const_data() const;
    bool compact();
    bool shrink();
    size_t total_size();

    static bool join_vectors(std::vector<DataType>& dest, int index,
        const std::vector<DataType>& src, DataType fill = DataType(-1));
    
    static bool compact(std::map<uint32_t, std::vector<DataType>>& blocks);
    static bool shrink(std::map<uint32_t, std::vector<DataType>>& blocks);
};


template <typename DataType>
const std::map<uint32_t, std::vector<DataType>>& hex_firmware<DataType>::const_data() const
{
    return m_data;
}

template <typename DataType>
bool hex_firmware<DataType>::compact()
{
    return compact(m_data);
}

template <typename DataType>
bool hex_firmware<DataType>::shrink()
{
    return shrink(m_data);
}

template <typename DataType>
size_t hex_firmware<DataType>::total_size()
{
    size_t sum = 0;
    for (const auto& data : m_data) {
        sum += data.second.size();
    }
    return sum;
}


template <typename DataType>
bool hex_firmware<DataType>::join_vectors(std::vector<DataType>& dest, int index, const std::vector<DataType>& src, 
    DataType fill)
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

template <typename DataType>
bool hex_firmware<DataType>::compact(std::map<uint32_t, std::vector<DataType>>& blocks)
{
    //for (auto& block : blocks) {
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

    auto prev = blocks.begin();
    auto curr = prev;

    //for (++curr; curr != blocks.end(); ++curr)
    while (++curr != blocks.end()) {
        if (curr->first == prev->first + prev->second.size()) {
            prev->second.insert(prev->second.end(), curr->second.begin(), curr->second.end());
            blocks.erase(curr);
            curr = prev;
        }
        prev = curr;
    }


    return true;
}

template<typename DataType>
bool hex_firmware<DataType>::shrink(std::map<uint32_t, std::vector<DataType>>& blocks)
{
    throw;
    return false;
}

