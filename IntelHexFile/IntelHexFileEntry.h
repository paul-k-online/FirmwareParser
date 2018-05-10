/*
 * IntelHexFileEntry.h
 * Created by Brent Strysko
 *
 * A IntelHexFileEntry represents a
 * textually parsed entry in a Intel hex file.
 */

#ifndef _INTEL_HEX_PARSER_INTEL_HEX_FILE_ENTRY_H_
#define _INTEL_HEX_PARSER_INTEL_HEX_FILE_ENTRY_H_


#include <string>
#include <vector>
#include <stdint.h>

using namespace std;

class IntelHexFileEntry
{
    uint16_t address;
    uint8_t recordType;
    vector<uint8_t> data;
    uint8_t checksum = 0;
	
public:
	IntelHexFileEntry() = default;
	explicit IntelHexFileEntry(string entry);
    ~IntelHexFileEntry() = default;
	
	uint8_t getByteCount() const;
    uint16_t getAddress() const;
    uint16_t getEndAddress() const;
    uint8_t getRecordType() const;
    vector<uint8_t> getData() const;
    uint8_t getChecksum() const;
	
	static uint64_t asciiHexTo64(const std::string s);
};

#endif
