#include "intel_hex_firmware.h"


bool intel_hex_firmware::parse_intelhex_file(const std::string& filename, std::list<intel_hex_entry> &entry_list)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        return false;
        //std::stringstream o;
        //o << "Cannot load file " << filename;
        //throw std::ios_base::failure(o.str());
    }

    while (file) {
        std::string line;
        std::getline(file, line);
        if (!line.empty()) {
            intel_hex_entry entry(line);
            if (!entry.is_valid())
                return false;
            entry_list.push_back(entry);
        }
    }
    return true;
}


bool intel_hex_firmware::join_endianness(const std::string & lsb_file, const std::string & msb_file,
                                         std::map<uint32_t, std::vector<uint16_t>>& word_blocks)
{
    bool result = true;
    
    std::list<intel_hex_entry> entries_most;
    if (result) {
        result = parse_intelhex_file(msb_file, entries_most);
    }
    
    std::list<intel_hex_entry> entries_least;
    if (result) {
        result = parse_intelhex_file(lsb_file, entries_least);
    }

    if (result) {
        result = join_endianness(entries_least, entries_most, word_blocks);
    }

    return result;
}

bool intel_hex_firmware::join_endianness(
    const std::list<intel_hex_entry>& lsb_entries,
    const std::list<intel_hex_entry>& msb_entries,
    std::map<uint32_t, std::vector<uint16_t>>& word_blocks)
{
    auto result = std::equal(msb_entries.begin(), msb_entries.end(), 
        lsb_entries.begin(), lsb_entries.end(), 
        intel_hex_entry::equals_without_data);
    
    std::map<uint32_t, std::vector<uint8_t>> msb_blocks;
    if (result) {
        result = to_data_map(msb_entries, msb_blocks);
    }
    
    std::map<uint32_t, std::vector<uint8_t>> lsb_blocks;
    if (result) {
        result = to_data_map(lsb_entries, lsb_blocks);
    }
    
    return join_endianness(lsb_blocks, msb_blocks, word_blocks);
}


bool intel_hex_firmware::join_endianness(const std::map<uint32_t, std::vector<uint8_t>>& lsb_blocks,
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


template <typename DataType>
bool intel_hex_firmware::to_data_map(const std::list<intel_hex_entry>& entries, std::map<uint32_t, std::vector<DataType>>& blocks)
{
    //std::map<uint32_t, std::vector<DataType>> blocks;
    uint32_t address_msw = 0;

    for (const auto& entry : entries) {
        switch (entry.record_type()) {

        case intel_hex_entry::Record_Type::extended_linear_address:
            if (entry.address() == 0 && entry.const_data().size() == 2) 
                address_msw = ((entry.const_data()[0] & 0xFF << 8) | (entry.const_data()[1] & 0xFF << 0)) << 16;
            break;

        case intel_hex_entry::Record_Type::data: {
            auto find_block = false;
            const auto address = address_msw | entry.address();

            for (auto& block : blocks) {
                // start of entry is inside to an block
                if (address >= block.first  &&  address <= block.first + block.second.size()) {
                    find_block = true;
                    join_vectors(block.second, address - block.first, entry.const_data(), uint8_t(0xff));
                    break;
                }
                // end of entry is inside to an block
                if (address < block.first  &&  address + entry.const_data().size() >= block.first) {
                    find_block = true;
                    join_vectors(block.second, address - block.first, entry.const_data(), uint8_t(0xff));
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
    ///return blocks;
    return true;
}


template <typename DataType>
bool intel_hex_firmware::join_vectors(std::vector<DataType>& dest, int index, const std::vector<DataType>& src, DataType fill)
{
    if (index >= 0) {
        auto add_size = int(src.size()) - (int(dest.size()) - index);
        if (add_size > 0)
            dest.insert(dest.end(), add_size, fill);
        std::copy(src.begin(), src.end(), dest.begin() + index);
    } else {
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
