/*
 * HexFile.h
 * Created by Brent Strysko
 *
 * A HexFile represents the
 * *.hex file provided to the library.
 */

#ifndef _HEX_PARSER_HEX_FILE_H_
#define _HEX_PARSER_HEX_FILE_H_

#include <iostream>
#include <string>
#include <map>
#include "IntelHexFileEntry.h"


#define INTEL_HEX_FILE_MAX_ENTRY_SIZE 524

class IntelHexFile
{
	std::map<size_t, IntelHexFileEntry> addressToFileEntries;

public:
	explicit IntelHexFile(const std::string& filename);
    friend std::ostream& operator<<(std::ostream& os, const IntelHexFile& file);
};

#endif
