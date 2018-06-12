#pragma once
#include <string>
#include <map>
#include <vector>

#include "hex_firmware_intel.h"

class hex_firmware_c2000 : public hex_firmware<uint16_t>
{
public:
    
    bool load(std::istream& file) override;
    bool load(const std::string & filename) override;

    bool save(std::ostream& file) override;
    bool valid() override;

    bool load_ascii(std::istream& stream);
    bool load_ascii(const std::string& filename);

    bool load_intel_width8(std::istream& file1, std::istream& file2);
    bool load_intel_width8(const std::string& filename1, const std::string& filename2);
    
    bool load_intel_width16(std::istream& file);
    bool load_intel_width16(const std::string& filename);

    static bool join_endianness(
        const std::map<uint32_t, std::vector<uint8_t>>& lsb_blocks,
        const std::map<uint32_t, std::vector<uint8_t>>& msb_blocks,
        std::map<uint32_t, std::vector<uint16_t>>& blocks);
};



bool hex_firmware_c2000::save(std::ostream& file)
{
        throw;
}

bool hex_firmware_c2000::valid()
{
    throw;
}


bool hex_firmware_c2000::load(std::istream& file)
{
    throw;
}

bool hex_firmware_c2000::load(const std::string & filename)
{
    std::ifstream file(filename);
    auto result = file.is_open();
    if (result) { //Cannot open file {filename};
        result = load(file);
    }
    return result;
}


bool hex_firmware_c2000::load_ascii(const std::string& filename)
{
    std::ifstream file(filename);
    auto result = file.is_open();
    if (result) { //Cannot open file {filename};
        result = load_ascii(file);
    }
    return result;
}


bool hex_firmware_c2000::load_ascii(std::istream& stream)
{
    throw;
}


bool hex_firmware_c2000::load_intel_width8(std::istream& stream1, std::istream& stream2)
{
    auto result = true;
    hex_firmware_intel<uint8_t> fw1;
    if (result) {
        result = fw1.load(stream1);
    }

    hex_firmware_intel<uint8_t> fw2;
    if (result) {
        result = fw2.load(stream2);
    }

    if (result) { // equals by size and type
                  //result = std::equal(i1.const_data().cbegin(), i1.const_data().cend(), 
                  //                    i2.const_data().cbegin(), i2.const_data().cend(), 
                  //                    [](std::pair<uint32_t, std::vector<uint8_t>> a, std::pair<uint32_t, std::vector<uint8_t>> b)
                  //                    {
                  //                        return a.first == b.first && a.second.size() == b.second.size();
                  //                    });            result = std::equal(i1.const_entry_list().cbegin(), i1.const_entry_list().cend(),
        result = std::equal(fw1.entry_list().cbegin(), fw1.entry_list().cend(),
                            fw2.entry_list().cbegin(), fw2.entry_list().cend(),
                            hex_entry_intel::equals_without_data);
    }

    if (result) {
        //TODO:
        //auto fw_data1 = fw1.const_data_map();
        //join_endianness(fw1.data_map(), fw2.data_map(), m_data_map);
    }
    m_load_valid = result;
    return m_load_valid;
}


bool hex_firmware_c2000::load_intel_width8(const std::string& filename1, const std::string& filename2)
{
    auto result = true;
        
    std::ifstream file1(filename1);
    if (result) {
        result = file1.is_open();
    }

    std::ifstream file2(filename2);
    if (result) {
        result = file2.is_open();
    }

    if (result) {
        result = load_intel_width8(file1, file2);
    }

    return result;
}


bool hex_firmware_c2000::load_intel_width16(std::istream& file)
{
    hex_firmware_intel<uint16_t> fw;
    auto result = fw.load(file);
    if (result) {
        //TODO:
        //m_data_map = (fw.data_map());
    }
    return result;
}


bool hex_firmware_c2000::load_intel_width16(const std::string& filename)
{
    std::ifstream file(filename);
    auto result = file.is_open();
    if (result) {
        result = load(file);
    }
    return result;
}


bool hex_firmware_c2000::join_endianness(
    const std::map<uint32_t, std::vector<uint8_t>>& lsb_blocks,
    const std::map<uint32_t, std::vector<uint8_t>>& msb_blocks, 
    std::map<uint32_t, std::vector<uint16_t>>& blocks)
{
    auto result = lsb_blocks.size() == msb_blocks.size();

    if (result) {
        auto lo_iter = lsb_blocks.cbegin();
        auto hi_iter = msb_blocks.cbegin();

        while (lo_iter != lsb_blocks.cend() || hi_iter != msb_blocks.cend()) {
            if ((*lo_iter).first != (*hi_iter).first) {
                result = false;
                break;
            }

            std::vector<uint16_t> word_vector;
            auto lo_data = (*lo_iter).second;
            auto hi_data = (*hi_iter).second;
            result = converter::join_vector(lo_data, hi_data, word_vector);
            if (!result)
                break;
            blocks.try_emplace((*hi_iter).first, word_vector);

            ++hi_iter;
            ++lo_iter;
        }
    }
    return result;
}
