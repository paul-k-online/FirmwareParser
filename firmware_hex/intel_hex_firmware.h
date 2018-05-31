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

#include "intel_hex_entry.h"
#include "firmware_block.h"

class intel_hex_firmware
{
public:
    static bool parse_intelhex_file(const std::string& filename, 
        std::list<intel_hex_entry> &entry_list);

    template <typename DataType>
    static bool to_data_map(
        const std::list<intel_hex_entry>& entries, 
        std::map<uint32_t, std::vector<DataType>>& blocks);

    static bool join_endianness(
        const std::string& lsb_file,
        const std::string& msb_file,
        std::map<uint32_t, std::vector<uint16_t>>& word_blocks);

    static bool join_endianness(
        const std::list<intel_hex_entry>& lsb_entries,
        const std::list<intel_hex_entry>& msb_entries,
        std::map<uint32_t, std::vector<uint16_t>>& word_blocks);

    static bool join_endianness(
        const std::map<uint32_t, std::vector<uint8_t>>& lsb_blocks,
        const std::map<uint32_t, std::vector<uint8_t>>& msb_blocks,
        std::map<uint32_t, std::vector<uint16_t>>& blocks);

    template <typename DataType>
    static bool join_vectors(std::vector<DataType>& dest, int index, 
        const std::vector<DataType>& src, 
        DataType fill = DataType(-1));
};


