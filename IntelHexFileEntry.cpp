#include <IntelHexFileEntry.h>

using namespace std;

bool IntelHexFileEntry::parse_from_string(const std::string& entry)
{
    if (entry[0] != ':')
    {
        return false;
        std::stringstream o;
        o << "Invalid hex entry: ";
        o << entry;
        throw std::ios_base::failure(o.str());
    }

    const uint8_t count = converter::asciiHexTo64(entry.substr(count_pos, 2));
    address_ = converter::asciiHexTo64(entry.substr(high_addr_pos, 4));
    record_type_ = static_cast<Record_Type>(
        converter::asciiHexTo64(entry.substr(rec_type_pos, 2)));

    if (entry.length() != 1 + 2 + 4 + 2 + count*2 + 2)
    {
        return false;
        std::stringstream o;
        o << "length provided != length of data | " 
          << static_cast<size_t>(count) << " != " << (entry.length() - (9 + 2 + 1));
        throw std::ios_base::failure(o.str());
    }

    data_.reserve(count);
    data_.clear();
    for (auto i = 0; i < count; i += 1)
    {
        uint8_t v = converter::asciiHexTo64(entry.substr(9 + i*2, 2));
        data_.push_back(v);
    }

    const uint8_t checksum_read = converter::asciiHexTo64(entry.substr(1 + 2 + 4 + 2 + count*2, 2));
    const uint8_t checksum_calc = this->calc_checksum();

    if (checksum_read != checksum_calc)
    {
        std::cout << std::hex << static_cast<size_t>(address_) << std::endl << std::dec;
        return false;
        std::stringstream o;
        o << std::hex 
            << "checksum_calc = 0x" << static_cast<size_t>(checksum_calc)
            << ", checksum_read = 0x" << static_cast<size_t>(checksum_read)
            << std::endl;
        throw ios_base::failure(o.str());
    }

    return false;
}

uint8_t IntelHexFileEntry::calc_checksum()
{
    const auto hAddress = address_ >> 8 & 0xFF;
    const auto lAddress = address_ >> 0 & 0xFF;
    const auto size = static_cast<uint8_t>(data_.size());
    uint8_t checksum = size + hAddress + lAddress + static_cast<uint8_t>(record_type_);
    for (auto i:data_) 
    {
        checksum += i;
    }
    checksum = 0x01 + ~checksum;
    return checksum;
}

IntelHexFileEntry::IntelHexFileEntry(const std::string& entry)
{
    parse_from_string(entry);
}

size_t IntelHexFileEntry::get_address() const
{
	return address_;
}

size_t IntelHexFileEntry::get_end_address() const
{
	return address_ + data_.size();
}

IntelHexFileEntry::Record_Type IntelHexFileEntry::get_record_type() const
{
	return record_type_;
}

std::vector<uint8_t> IntelHexFileEntry::get_data() const
{
	return data_;
}
