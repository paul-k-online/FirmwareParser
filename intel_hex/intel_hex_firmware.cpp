#include "intel_hex_firmware.h"

template <typename DataType>
std::map<uint32_t, std::vector<DataType>>& 
intel_hex_firmware<DataType>::intel_entry_list_to_data_map(
    const std::list<intel_hex_entry<DataType>>& entries)
{
    std::map<uint32_t, std::vector<DataType>> blocks;

    uint16_t address_MSW = 0;

    for (auto& entry : entries)
    {
        switch (entry.record_type()) {
        case intel_hex_entry<DataType>::Record_Type::extended_linear_address:
            if (entry.address() == 0 && entry.const_data().size() == 2)
                address_MSW = (entry.const_data()[0] & 0xFF << 8) | (entry.const_data()[1] & 0xFF << 0);
            break;
        case intel_hex_entry<DataType>::Record_Type::data:
        {
            auto find_block = false;
            const uint32_t address = (address_MSW << 16 & 0xFFFF) | entry.address();
            const uint32_t end_address = address + entry.const_data().size();

            for (auto& block : blocks)
            {
                // start of entry is inside to an block...
                if (address >= block.first  &&  address <= block.first + block.second.size()) {
                    find_block = true;
                    const auto index = address - block.first;
                    join_vectors(block.second, index, entry.const_data(), uint8_t(0xff));
                    break;
                }
                // end of entry is inside to an block...
                if (address < block.first  &&  end_address >= block.first) {
                    find_block = true;
                    const auto index = address - block.first;
                    join_vectors(block.second, index, entry.const_data(), uint8_t(0xff));
                    break;
                }
            }

            if (!find_block) {
                // Create a new block for entry without cross with blocks
                blocks.try_emplace(address, entry.const_data());
            }
            break;
        }
        default: 
                break;
        }

    }
    return blocks;
}


template <typename DataType>
void intel_hex_firmware<DataType>::join_vectors(std::vector<DataType>& dest, int index, const std::vector<DataType>& src, DataType fill)
{
    if (index >= 0)
    {
        auto add_size = int(src.size()) - (int(dest.size()) - index);
        if (add_size > 0)
            dest.insert(dest.end(), add_size, fill);
        std::copy(src.begin(), src.end(), dest.begin() + index);
    }
    else
    {
        auto last = int(src.size()) + index - int(dest.size());
        if (last < 0)
            last = 0;
        
        auto add_size = -index + last;
        if (add_size > 0)
            dest.insert(dest.begin(), add_size, fill);
        std::copy(src.begin(), src.end(), dest.begin());
    }
    //return dest;
}
