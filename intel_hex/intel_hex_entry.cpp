#include "intel_hex_entry.h"

using namespace std;

template <typename DataType>
bool intel_hex_entry<DataType>::parse_from_string(const std::string& string, intel_hex_entry &entry)
{
    if (string[0] != record_mark)
    {
        return false;
        std::stringstream o;
        o << "Invalid hex entry: ";
        o << string;
        throw std::ios_base::failure(o.str());
    }

    const auto count = converter::hex_to_64(string.substr(count_pos, 2));
    entry.m_address = converter::hex_to_64(string.substr(high_addr_pos, 4));

    entry.m_record_type = static_cast<Record_Type>(converter::hex_to_64(string.substr(rec_type_pos, 2)));

    const auto data_type_size = sizeof DataType;
    const auto data_type_length = data_type_size * 2;
    const auto data_offset = 1 + 2 + 4 + 2;
    if (string.length() != data_offset + count * data_type_length + 2)
    {
        return false;
        std::stringstream o;
        o << "length provided != length of data | " 
          << static_cast<size_t>(count) << " != " << (string.length() - (9 + 2 + 1));
        throw std::ios_base::failure(o.str());
    }

    entry.m_data.reserve(count);
    entry.m_data.clear();
    for (auto i = 0; i < count; i += 1)
    {
        DataType v = converter::hex_to_64(string.substr(data_offset + i * data_type_length, 2));
        entry.m_data.emplace_back(v);
    }

    entry.m_checksum = converter::hex_to_64(string.substr(data_offset + count * data_type_length, 2));
    const auto checksum_calc = entry.calc_checksum();

    if (entry.m_checksum != checksum_calc)
    {
        std::cout << std::hex << static_cast<size_t>(entry.m_address) << std::endl << std::dec;
        return false;
        std::stringstream o;
        o << std::hex 
            << "checksum_calc = 0x" << static_cast<size_t>(checksum_calc)
            << ", checksum_read = 0x" << static_cast<size_t>(entry.m_checksum)
            << std::endl;
        throw ios_base::failure(o.str());
    }

    return true;
}

template <typename DataType>
bool intel_hex_entry<DataType>::parse_from_stream(const std::stringstream& stream, intel_hex_entry &entry)
{
    char mark;
    //stream >> mark;
    //
    //if (string[0] != record_mark)
    //{
    //    return false;
    //    std::stringstream o;
    //    o << "Invalid hex entry: ";
    //    o << string;
    //    throw std::ios_base::failure(o.str());
    //}

    //const uint8_t count = converter::asciiHexTo64(string.substr(count_pos, 2));
    //entry.address_ = converter::asciiHexTo64(string.substr(high_addr_pos, 4));
    //entry.record_type_ = static_cast<Record_Type>(
    //    converter::asciiHexTo64(string.substr(rec_type_pos, 2)));

    //if (string.length() != 1 + 2 + 4 + 2 + count * 2 + 2)
    //{
    //    return false;
    //    std::stringstream o;
    //    o << "length provided != length of data | "
    //        << static_cast<size_t>(count) << " != " << (string.length() - (9 + 2 + 1));
    //    throw std::ios_base::failure(o.str());
    //}

    //entry.data_.reserve(count);
    //entry.data_.clear();
    //for (auto i = 0; i < count; i += 1)
    //{
    //    uint8_t v = converter::asciiHexTo64(string.substr(9 + i * 2, 2));
    //    entry.data_.push_back(v);
    //}

    //entry.checksum_ = converter::asciiHexTo64(string.substr(1 + 2 + 4 + 2 + count * 2, 2));
    //const auto checksum_calc = entry.calc_checksum();

    //if (entry.checksum_ != checksum_calc)
    //{
    //    std::cout << std::hex << static_cast<size_t>(entry.address_) << std::endl << std::dec;
    //    return false;
    //    std::stringstream o;
    //    o << std::hex
    //        << "checksum_calc = 0x" << static_cast<size_t>(checksum_calc)
    //        << ", checksum_read = 0x" << static_cast<size_t>(entry.checksum_)
    //        << std::endl;
    //    throw ios_base::failure(o.str());
    //}

    return false;
}

template <typename DataType>
bool intel_hex_entry<DataType>::compile_to_string(const intel_hex_entry& entry, std::string& string)
{
    stringstream ss;
    ss.setf(std::ios::hex, std::ios::basefield);	//Set the stream to ouput hex instead of decimal
    ss.setf(std::ios::uppercase);			//Use uppercase hex notation
    ss.fill('0');							//Pad with zeroes
    
    ss << record_mark;
    ss.width(2);
    ss << unsigned(entry.m_data.size());
    ss.width(4);
    ss << unsigned(entry.m_address);
    ss.width(2);
    ss << unsigned(entry.m_record_type);

    if(entry.m_record_type == Record_Type::data)
    {
        for (const auto& data : entry.m_data)
        {
            ss.width(2 * sizeof DataType);
            ss << unsigned(data);
        }
    }
    ss.width(2);
    ss << unsigned(entry.calc_checksum());
    
    string = ss.str();
    return true;
}

template <typename DataType>
uint8_t intel_hex_entry<DataType>::calc_checksum() const
{
    const auto hAddress = m_address >> 8 & 0xFF;
    const auto lAddress = m_address >> 0 & 0xFF;
    const auto size = static_cast<uint8_t>(m_data.size());
    uint8_t checksum = size + hAddress + lAddress + static_cast<uint8_t>(m_record_type);
    for (auto i:m_data) 
    {
        checksum += i;
    }
    checksum = 0x01 + ~checksum;
    return checksum;
}

template <typename DataType>
intel_hex_entry<DataType>::intel_hex_entry(const std::string& entry)
{
    m_valid = parse_from_string(entry, *this);
}

template <typename DataType>
uint16_t intel_hex_entry<DataType>::address() const
{
	return m_address;
}

template <typename DataType>
uint16_t intel_hex_entry<DataType>::end_address() const
{
	return m_address + m_data.size();
}

template <typename DataType>
typename intel_hex_entry<DataType>::Record_Type intel_hex_entry<DataType>::record_type() const
{
	return m_record_type;
}

template <typename DataType>
std::vector<DataType>& intel_hex_entry<DataType>::data()
{
    return m_data;
}

template <typename DataType>
const std::vector<DataType>& intel_hex_entry<DataType>::const_data() const
{
	return m_data;
}

template <typename DataType>
std::string intel_hex_entry<DataType>::to_string() const
{
    std::string string;
    auto result = compile_to_string(*this, string);
    if (result)
        return string;
    return "";
}

template <typename DataType>
bool intel_hex_entry<DataType>::is_valid() const
{
    return m_valid && m_checksum == calc_checksum();
}
