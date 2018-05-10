#include <IntelHexFileEntry.h>

#include <sstream>
#include <iostream>

IntelHexFileEntry::IntelHexFileEntry(string entry)
{
	if (entry[0] != ':')
	{
		stringstream o;
		o << "Invalid hex entry: ";
		o << entry;
		throw ios_base::failure(o.str());
	}

	const uint8_t count = 2 * asciiHexTo64(entry.substr(1, 2));
	const uint8_t hAddress = asciiHexTo64(entry.substr(3, 2));
	const uint8_t lAddress = asciiHexTo64(entry.substr(5, 2));
	address = (hAddress << 8) | lAddress;
	recordType = asciiHexTo64(entry.substr(7, 2));

	uint8_t c_checksum = (count / 2) + lAddress + hAddress + recordType;

	if (count != entry.length() - (9 + 2 + 1))
	{
		stringstream o;
		o << "length provided != length of data | " << static_cast<size_t>(count) 
			<< " != " << (entry.length() - (9 + 2 + 1));
		throw ios_base::failure(o.str());
	}

	for (auto i = 0; i < count; i += 2)
	{
		uint8_t v = asciiHexTo64(entry.substr(9 + i, 2));
		c_checksum += v;
		data.push_back(v);
	}

	const uint8_t checksum = asciiHexTo64(entry.substr(entry.length() - 3, 2));

	if (static_cast<uint8_t>(c_checksum + checksum) != static_cast<uint8_t>(0))
	{
		cout << hex << static_cast<size_t>(address) << endl << dec;

		stringstream o;
		o << hex;
		o << "(cChecksum + checksum) != 0 | 0x" << static_cast<size_t>(c_checksum + checksum);
		o << " != 0 | cChecksum = 0x" << static_cast<size_t>(c_checksum);
		o << ", checksum = 0x" << static_cast<size_t>(checksum);

		throw ios_base::failure(o.str());
	}
}

uint64_t IntelHexFileEntry::asciiHexTo64(const std::string s)
{
	stringstream o;
	o << hex << s;
	uint64_t v;
	o >> v;
	return v;
}

uint8_t IntelHexFileEntry::getByteCount() const
{
	return data.size();
}

uint16_t IntelHexFileEntry::getAddress() const
{
	return address;
}

uint16_t IntelHexFileEntry::getEndAddress() const
{
	return address + data.size();
}

uint8_t IntelHexFileEntry::getRecordType() const
{
	return recordType;
}

vector<uint8_t> IntelHexFileEntry::getData() const
{
	return data;
}

uint8_t IntelHexFileEntry::getChecksum() const
{
	return checksum;
}
