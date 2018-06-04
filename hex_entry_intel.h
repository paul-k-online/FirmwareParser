/*
 * IntelHexFileEntry.h
 *
 * A IntelHexFileEntry represents a textually parsed entry in a Intel hex file.
 */

#ifndef __INTEL_HEX_ENTRY_H__
#define __INTEL_HEX_ENTRY_H__

#include <cstdint>
#include <string>
#include <vector>
#include "converter.h"


class hex_entry_intel
{
    static const char record_mark = ':';
    static const int count_pos = 1;
    static const int high_addr_pos = 3;
    static const int low_addr_pos = 5;
    static const int rec_type_pos = 7;

public:
    enum class Record_Type : uint16_t
    {
        data = 0x00,
        eof = 0x01,
        extended_segment_address = 0x02,
        start_segment_address = 0x03,
        extended_linear_address = 0x04,
        start_linear_address = 0x05,
    };

    explicit hex_entry_intel(const uint16_t address, const std::vector<uint8_t>& data, const Record_Type record_type = Record_Type::data) :
        m_address(address), m_data(data), m_record_type(record_type)
    { }
	explicit hex_entry_intel(const std::string& entry);

    uint16_t address() const;
    uint16_t end_address() const;
    Record_Type record_type() const;
    std::vector<uint8_t>& data();
	const std::vector<uint8_t>& const_data() const;
    std::string to_string() const;
    bool is_valid() const;

    static bool equals_without_data(const hex_entry_intel & r, const hex_entry_intel & l);
    static bool parse(const std::string & string, hex_entry_intel & entry);
    static bool to_string(const hex_entry_intel &entry, std::string& string);

private:
    uint16_t m_address = 0;
    Record_Type m_record_type = Record_Type(0);
    std::vector<uint8_t> m_data{};
    uint8_t m_checksum = 0;
    bool m_valid = false;

    uint8_t calc_checksum() const;

};

#endif // __INTEL_HEX_ENTRY_H__
