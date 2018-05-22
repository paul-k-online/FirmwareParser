#include <fstream>
#include <sstream>
#include <iostream>

#include "IntelHexFile.h"


IntelHexFile::IntelHexFile(const std::string& filename)
{
	std::ifstream file(filename.c_str());
	if(!file.is_open())
	{
		std::stringstream o;
		o << "Cannot load file " << filename;
		throw std::ios_base::failure(o.str());
	}

	char buffer[INTEL_HEX_FILE_MAX_ENTRY_SIZE];

	while(true)
	{
		file.getline(buffer, INTEL_HEX_FILE_MAX_ENTRY_SIZE);
		// End of file
		if(!file)
		{
			break;
		}

		IntelHexFileEntry entry(buffer);
		if((entry.get_record_type() != IntelHexFileEntry::Record_Type::data) 
            && (entry.get_record_type() != IntelHexFileEntry::Record_Type::eof))
		{
			std::stringstream o;
			o << "Unsupported record type: 0x" 
				<< std::hex << static_cast<uint32_t>(entry.get_record_type());
			throw std::ios_base::failure(o.str());
		}
		addressToFileEntries.insert(std::pair<uint16_t, IntelHexFileEntry>(entry.get_address(), entry));
	}
}


std::ostream& operator<<(std::ostream& os, const IntelHexFile& file)
{
	const auto begin_address = file.addressToFileEntries.begin()->first;
	const auto end_address = file.addressToFileEntries.rbegin()->second.get_end_address();

	os << "[Address Range: 0x" << std::hex << begin_address
		<< "-0x" << std::hex << end_address
		<< ", Number of HexFileEntries: " << std::dec << file.addressToFileEntries.size()
		<< "]";
	return os;
}