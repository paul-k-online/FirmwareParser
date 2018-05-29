/*
 * intel_hex_file.h
 *
 * A HexFile represents the
 * *.hex file provided to the library.
 */

#ifndef _HEX_PARSER_HEX_FILE_H_
#define _HEX_PARSER_HEX_FILE_H_

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <fstream>
#include <sstream>
#include <iostream>
#include "intel_hex_entry.h"

#define INTEL_HEX_FILE_MAX_ENTRY_SIZE 524

template <typename DataType>
class intel_hex_file
{
    std::list<intel_hex_entry<DataType>> m_entry_list;
public:
	explicit intel_hex_file(const std::string& filename);

    const std::list<intel_hex_entry<DataType>>& get_enrty_list() const;
    size_t get_entry_count() const;

    static bool parse_file(const std::string& filename, std::list<intel_hex_entry<DataType>> &entry_list);

    friend std::ostream& operator<<(std::ostream& os, const intel_hex_file& file);
};

#endif
