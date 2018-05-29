#include "intel_hex_file.h"

template <typename DataType>
bool intel_hex_file<DataType>::parse_file(const std::string& filename, std::list<intel_hex_entry<DataType>> &entry_list)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    {
        return false;
        std::stringstream o;
        o << "Cannot load file " << filename;
        throw std::ios_base::failure(o.str());
    }

    while (file)
    {
        std::string line;
        std::getline(file, line);
        intel_hex_entry<DataType> entry(line);
        if (entry.is_valid())
            entry_list.emplace_back(entry);
        else
            line = "";
    }
    return true;
}

template <typename DataType>
intel_hex_file<DataType>::intel_hex_file(const std::string& filename)
{
    m_entry_list.clear();
    const auto result = parse_file(filename, m_entry_list);
    if (!result)
        m_entry_list.clear();
}

template <typename DataType>
const std::list<intel_hex_entry<DataType>>& intel_hex_file<DataType>::get_enrty_list() const
{
    return m_entry_list;
}

template <typename DataType>
size_t intel_hex_file<DataType>::get_entry_count() const
{
    return m_entry_list.size();
}

template <typename DataType>
std::ostream& operator<<(std::ostream& os, const intel_hex_file<DataType>& file)
{
	//const auto begin_address = file.addressToFileEntries.begin()->first;
	//const auto end_address = file.addressToFileEntries.rbegin()->second.get_end_address();

	//os << "[Address Range: 0x" << std::hex << begin_address
	//	<< "-0x" << std::hex << end_address
	//	<< ", Number of HexFileEntries: " << std::dec << file.addressToFileEntries.size()
	//	<< "]";
	return os;
}
