#include "hex_entry_intel.h"

bool hex_entry_intel::parse(const std::string & string, hex_entry_intel & entry)
{
    if (string[0] != record_mark) {
        return false; // "Invalid hex entry: " + string;
    }

    const auto count = converter::hex_to_64(string.substr(count_pos, 2));
    entry.m_address = converter::hex_to_64(string.substr(high_addr_pos, 4));
    entry.m_record_type = static_cast<Record_Type>(converter::hex_to_64(string.substr(rec_type_pos, 2)));

    const auto type_size = sizeof uint8_t; // 1
    const auto type_length = type_size * 2;
    const auto offset = 1 + 2 + 4 + 2;
    if (string.length() != offset + count * type_length + 2) {
        return false; // "length provided != length of data | "  << static_cast<size_t>(count) << " != " << (string.length() - (9 + 2 + 1));
    }

    entry.m_data_map.reserve(count);
    entry.m_data_map.clear();
    for (size_t i = 0; i < count; ++i) {
        auto v = converter::hex_to_64(string.substr(offset + i * type_length, 2));
        entry.m_data_map.emplace_back(v);
    }

    entry.m_checksum = converter::hex_to_64(string.substr(offset + count * type_length, 2));
    const auto checksum_calc = entry.calc_checksum();

    if (entry.m_checksum != checksum_calc) {
        std::cout << std::hex << static_cast<size_t>(entry.m_address) << std::endl << std::dec;
        return false; // << std::hex << "checksum_calc = 0x" << static_cast<size_t>(checksum_calc)  << ", checksum_read = 0x" << static_cast<size_t>(entry.m_checksum) << std::endl;
    }
    return true;
}


bool hex_entry_intel::to_string(const hex_entry_intel& entry, std::string& string)
{
    std::stringstream ss;
    ss.setf(std::ios::hex, std::ios::basefield);	//Set the stream to ouput hex instead of decimal
    ss.setf(std::ios::uppercase);			//Use uppercase hex notation
    ss.fill('0');							//Pad with zeroes
    
    ss << record_mark;
    ss.width(2);
    ss << unsigned(entry.m_data_map.size() & 0xFF);
    ss.width(4);
    ss << unsigned(entry.m_address & 0xFFFF);
    ss.width(2);
    ss << unsigned(uint16_t(entry.m_record_type) & 0xFF);

    if(entry.m_record_type == Record_Type::data)
    {
        for (const auto& data : entry.m_data_map)
        {
            ss.width(2);
            ss << unsigned(data);
        }
    }
    ss.width(2);
    ss << unsigned(entry.calc_checksum());
    
    string = ss.str();
    return true;
}


uint8_t hex_entry_intel::calc_checksum() const
{
    const auto size = static_cast<uint8_t>(m_data_map.size());
    uint8_t checksum = size + 
        converter::hi_byte(m_address) + 
        converter::lo_byte(m_address) + 
        static_cast<uint8_t>(m_record_type);
    for (const auto i : m_data_map) 
    {
        checksum += i;
    }
    checksum = 0x01 + ~checksum;
    return checksum;
}


hex_entry_intel::hex_entry_intel(const std::string& entry)
{
    m_valid = parse(entry, *this);
}


uint16_t hex_entry_intel::address() const
{
	return m_address;
}


uint16_t hex_entry_intel::end_address() const
{
	return m_address + m_data_map.size();
}


hex_entry_intel::Record_Type hex_entry_intel::record_type() const
{
	return m_record_type;
}


std::vector<uint8_t>& hex_entry_intel::data()
{
    return m_data_map;
}


const std::vector<uint8_t>& hex_entry_intel::const_data() const
{
	return m_data_map;
}


std::string hex_entry_intel::to_string() const
{
    std::string string;
    const auto result = to_string(*this, string);
    if (result)
        return string;
    return "";
}


bool hex_entry_intel::valid() const
{
    return m_valid && m_checksum == calc_checksum();
}


bool hex_entry_intel::equals_without_data(const hex_entry_intel& r, const hex_entry_intel& l)
{
    return r.m_address == l.m_address
        && r.m_record_type == l.m_record_type
        && r.m_data_map.size() == l.m_data_map.size();
}
