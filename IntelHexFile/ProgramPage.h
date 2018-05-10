/*
 * ProgramPage.h
 * Created by Brent Strysko
 *
 * A ProgramPage represents the data
 * that a processor with a page of size
 * pageSize would hold at address.
 */

#ifndef _INTEL_HEX_PARSER_PROGRAM_PAGE_H_
#define _INTEL_HEX_PARSER_PROGRAM_PAGE_H_

#include "IntelHexFileEntry.h"

//#include <sstream>
#include <vector>
//#include <stdint.h>
//#include <string.h>

using namespace std;

class ProgramPage
{
    size_t address_;
    size_t page_size_;
    vector<uint8_t> data_;
public:
    ProgramPage(size_t address, size_t pageSize, const vector<uint8_t>& pageData);
    ~ProgramPage() {}

    size_t getAddress() const;
    size_t getPageSize() const;
    size_t getEndAddress() const;

    vector<uint8_t> getData() const;

    friend ostream& operator<<(ostream& os, const ProgramPage& rhs);
};

#endif
