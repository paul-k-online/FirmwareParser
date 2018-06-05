#pragma once
#include <string>
#include <map>
#include <vector>

#include "hex_firmware_intel.h"

class hex_firmware_c2000 : public hex_firmware<uint16_t>
{

public:
    static bool join_endianness(
        const std::map<uint32_t, std::vector<uint8_t>>& lsb_blocks,
        const std::map<uint32_t, std::vector<uint8_t>>& msb_blocks,
        std::map<uint32_t, std::vector<uint16_t>>& blocks);

    bool load_ascii(const std::string& filename)
    {
        return false;
    }


    bool load_intel_width8(const std::string& filename1, const std::string& filename2)
    {
        auto result = true;
        hex_firmware_intel<uint8_t> fw1;
        if (result) {
            result = fw1.load(filename1);
        }

        hex_firmware_intel<uint8_t> fw2;
        if (result) {
            result = fw2.load(filename2);
        }

        if (result) { // equals by size and type
            //result = std::equal(i1.const_data().cbegin(), i1.const_data().cend(), 
            //                    i2.const_data().cbegin(), i2.const_data().cend(), 
            //                    [](std::pair<uint32_t, std::vector<uint8_t>> a, std::pair<uint32_t, std::vector<uint8_t>> b)
            //                    {
            //                        return a.first == b.first && a.second.size() == b.second.size();
            //                    });            result = std::equal(i1.const_entry_list().cbegin(), i1.const_entry_list().cend(),
            result = std::equal(fw2.entry_list().cbegin(), fw2.entry_list().cend(), hex_entry_intel::equals_without_data);
        }
        
        if (result) {
            //TODO:
            //auto fw_data1 = fw1.const_data_map();
            //join_endianness(fw1.data_map(), fw2.data_map(), m_data_map);
        }
        m_load_valid = result;
        return m_load_valid;
    }

    bool load_intel_width16(const std::string& filename)
    {
        hex_firmware_intel<uint16_t> fw;
        auto result = fw.load(filename);
        if (result) {
            //TODO:
            //m_data_map = (fw.data_map());
        }
        return result;
    }
};



bool hex_firmware_c2000::join_endianness(
    const std::map<uint32_t, std::vector<uint8_t>>& lsb_blocks,
    const std::map<uint32_t, std::vector<uint8_t>>& msb_blocks, 
    std::map<uint32_t, std::vector<uint16_t>>& blocks)
{
    auto result = lsb_blocks.size() == msb_blocks.size();

    if (result) {
        auto lsb_iter = lsb_blocks.cbegin();
        auto msb_iter = msb_blocks.cbegin();

        while (lsb_iter != lsb_blocks.cend() || msb_iter != msb_blocks.cend()) {
            if ((*lsb_iter).first != (*msb_iter).first) {
                result = false;
                break;
            }

            std::vector<uint16_t> word_vector;
            result = converter::make_word((*lsb_iter).second, (*msb_iter).second, word_vector);
            if (!result)
                break;
            blocks.try_emplace((*msb_iter).first, word_vector);

            ++msb_iter;
            ++lsb_iter;
        }
    }
    return result;
}
