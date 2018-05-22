/*
 * IntelHexFileEntry.h
 *
 * A IntelHexFileEntry represents a textually parsed entry in a Intel hex file.
 */

#ifndef INTEL_HEX_FILE_ENTRY_H
#define INTEL_HEX_FILE_ENTRY_H

#include <string>
#include <utility>
#include <vector>
#include <cstdint>
#include "converter.h"


class IntelHexFileEntry
{
public:
    enum class Record_Type
    {
        data = 0x00,
        eof = 0x01,
        extended_segment_address = 0x02,
        start_segment_address = 0x03,
        extended_linear_address = 0x04,
    };

    const int count_pos = 1;
    const int high_addr_pos = 3;
    const int low_addr_pos = 5;
    const int rec_type_pos = 7;

    explicit IntelHexFileEntry(const size_t address, const Record_Type record_type, std::vector<uint8_t> data) :
        address_(address), record_type_(record_type), data_(std::move(data))
    { }

	explicit IntelHexFileEntry(const std::string& entry);

    bool parse_from_string(const std::string& entry);

	size_t get_address() const;
	size_t get_end_address() const;
    IntelHexFileEntry::Record_Type get_record_type() const;
	std::vector<uint8_t> get_data() const;
	
private:
    size_t address_ = 0;
    Record_Type record_type_ = static_cast<Record_Type>(0);
    std::vector<uint8_t> data_{};
    //uint8_t checksum_ = 0;

    uint8_t calc_checksum();

};

#endif // INTEL_HEX_FILE_ENTRY_H
