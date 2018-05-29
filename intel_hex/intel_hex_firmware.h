#pragma once
#include <list>
#define NOMINMAX
#include <algorithm>

#include "intel_hex_entry.h"
#include "firmware_block.h"
#include <map>

template <typename DataType>
class intel_hex_firmware
{
public:

    static std::map<uint32_t, std::vector<DataType>>& 
        intel_entry_list_to_data_map(
            const std::list<intel_hex_entry<DataType>>& entry_list);
    
    static void 
        join_vectors(
            std::vector<DataType>& dest, 
            int index,
            const std::vector<DataType>& src, 
            DataType fill = DataType(-1));
};
