/*
 *    Routines for reading/writing Intel INHX8M and INHX32 files
 */
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

#include "IntelHex.h"


namespace IntelHex
{
	// Array access operator
	template <typename ValueType>
	ValueType& hex_data<ValueType>::operator[](const address_type address)
	{
		// Start at the end of the list and find the first (last) block with an address less than addr
		for(auto i = blocks.rbegin(); i != blocks.rend(); ++i)
		{
			if (i->first <= address)
			{
				// Use the block if address is interior or adjacent to the block
				if ((address - i->first) <= i->second.size())
					return i->second[address - i->first];
				break;
			}
		}
		return blocks[address][0];
	}


	// Return the value at address, or _fill if not set
	template <typename ValueType>
	ValueType hex_data<ValueType>::get(address_type address)
	{
		// Start at the end of the list and find the first (last) block with an address less than addr
		for (auto i = blocks.rbegin(); i != blocks.rend(); ++i)
		{
			if (i->first <= address)
			{
				// Use the block if address is interior to the block
				if ((address - i->first) < i->second.size())
					return i->second[address - i->first];
				break;
			}
		}
		return _fill;
	}


	// Set the value at address or create a new element using value
	template <typename ValueType>
	void hex_data<ValueType>::set(address_type address, ValueType value)
	{
		if (value == fill())	// Handle fill values
		{
			erase(address);	// If the address is already set, erase it
			return;
		}

		// Start at the end of the list and find the first (last) block with an address less than addr
		for (auto i = blocks.rbegin(); i != blocks.rend(); ++i)
		{
			if (i->first <= address)
			{
				// Use the block if address is interior or adjacent to the block
				const auto index = address - i->first;
				if (index < i->second.size())
				{
					i->second[index] = value;
					return;
				}
				if (index == i->second.size())
				{
					i->second.push_back(value);
					return;
				}
				break;
			}
		}
		blocks[address].push_back(value);	// Otherwise create a new block
	}


	// Merge adjacent blocks
	template <typename ValueType>
	void hex_data<ValueType>::compact()
	{
		auto previous = blocks.begin();
		auto i = previous;

		for (++i; i != blocks.end(); ++i)
		{
			if ((previous->first + previous->second.size()) == i->first)
			{
				previous->second.insert(previous->second.end(), i->second.begin(), i->second.end());
				blocks.erase(i);
				i = previous;
			}
			previous = i;
		}
	}

	
	// Delete all allocated memory
	template <typename ValueType>
	void hex_data<ValueType>::clear()
	{
		_fill = 0;
		format = HEX_FORMAT_INHX8M;
		linear_addr_rec = false;
		segment_addr_rec = false;
		blocks.clear();
	}


	// Erase a single element at the given address
	template <typename ValueType>
	void hex_data<ValueType>::erase(const address_type address)
	{
		for (auto i = blocks.begin(); i != blocks.end(); ++i)
		{
			// The blocks are sorted, so if the byte to be deleted is
			//  before the block it must be a blank address that's either
			//  before the first block or after any previous blocks.
			if (address < i->first)
				break;
			// Ignore the block if address is past the end of the block
			const auto ope = i->first + i->second.size();
			if (address >= ope)
				continue;
			// address is now guaranteed to be >= i->first and < ope
			// Copy trailing portion of the old block to a new block
			if ((ope - address) > 1)
			{
				const auto index = address - i->first + 1;
				blocks[address + 1].assign(i->second.begin() + index, i->second.end());
			}
			// Truncate or delete old block
			const auto size = address - i->first;
			if (size)
				i->second.resize(size);
			else
				blocks.erase(i);
			break;
		}
	}


	// Erase [first, last]
	template <typename ValueType>
	void hex_data<ValueType>::erase(address_type first, address_type last)
	{
		if (first > last)
			std::swap(first, last);

		for (auto i = blocks.begin(); (i != blocks.end()) && (first <= last); ++i)
		{
			const auto ope = i->first + i->second.size();
			if (first >= ope)	// Ignore all blocks with addresses < first
				continue;
			// The blocks are sorted, so if the first byte to be deleted is
			//  before the block it must be a blank address that's either
			//  before the first block or after any previous blocks.
			if (first < i->first)
			{
				if (last < i->first)	// If the entire range is before the
					return;		//  block there's nothing left to do
				first = i->first;   // Advance to the next non-blank address
			}
			// first is now guaranteed to be >= i->first and < ope
			if (last < ope)	// Entire range is interior
			{
				// Copy trailing portion of the old block to a new block
				if ((ope - last) > 1)
				{
					const auto index = last - i->first + 1;
					blocks[last + 1].assign(i->second.begin() + index, i->second.end());
				}
				// Truncate or delete old block
				const auto size = first - i->first;
				if (size)
					i->second.resize(size);
				else
					blocks.erase(i);
				return;
			}
			else	// Truncate block
			{
				const auto size = first - i->first;
				if (size)
					i->second.resize(size);
				else
					blocks.erase(i--);
				first = ope;
			}
		}
	}


	template <typename ValueType>
	typename hex_data<ValueType>::size_type hex_data<ValueType>::size()
	{
		size_type s = 0;
		for (auto& block : blocks)
			s += block.second.size();
		return s;
	}


	// Returns the number of populated elements with addresses less than addr
	template <typename ValueType>
	typename hex_data<ValueType>::size_type hex_data<ValueType>::size_below_addr(const address_type addr)
	{
		size_type s = 0;
		for (auto& block : blocks)
		{
			if ((block.first + block.second.size()) < addr)
				s += block.second.size();
			else if (block.first < addr)
				s += addr - block.first;
		}
		return s;
	}


	// number of words in [lo, hi)
	template <typename ValueType>
	typename hex_data<ValueType>::size_type hex_data<ValueType>::size_in_range(const address_type lo, const address_type hi)
	{
		size_type s = 0;
		for (auto& block : blocks) 
		{
			if (block.first < lo)
			{
				const auto a = block.first + block.second.size();
				if (a >= lo)
					s += a - lo;
			}
			else
			{
				if ((block.first + block.second.size()) < hi)
					s += block.second.size();
				else if (block.first < hi)
					s += hi - block.first;
			}
		}
		return s;
	}


	// Return the max address of all of the set words with addresses less than or equal to hi
	template <typename ValueType>
	address_type hex_data<ValueType>::max_addr_below(const address_type hi)
	{
		address_type s = 0;
		for (auto& block : blocks)
		{
			if (block.first <= hi)
			{
				const auto a = block.first + block.second.size() - 1;	//Max address of this block
				if (a > s)
					s = a;
			}
		}
		//if (s > hi) return hi; else return s;
		return std::min(s, hi);

	}


	// Lowest address
	template <typename ValueType>
	address_type hex_data<ValueType>::min_address() const
	{
		return blocks.begin()->first;
	}


	// Highest address
	template <typename ValueType>
	address_type hex_data<ValueType>::max_address() const
	{
		return blocks.rbegin()->first + blocks.rbegin()->second.size() - 1;
	}


	//Return true if an element exists at addr
	template <typename ValueType>
	bool hex_data<ValueType>::is_set(const address_type addr)
	{
		// Start at the end of the list and find the first (last) block with an address less than addr
		auto i = blocks.rbegin();
		while ((i != blocks.rend()) && (i->first > addr))
			++i;
		return (addr - i->first) <= i->second.size();
	}


	// Load from a file
	template <typename ValueType>
	bool hex_data<ValueType>::load(const std::string &path)
	{
		std::ifstream f(path.c_str());
		return read(f);
	}

	
	// Read data from an input stream
	template <typename ValueType>
	bool hex_data<ValueType>::read(std::istream &s)
	{
		address_type	linear_address(0);

		while (s.get() == ':' && s.good())
		{
			std::string line;
			getline(s, line);		    // Read the whole line
			if (line.size() <= 10)	    // Ignore truncated lines
				break;
			std::vector<ValueType> buffer = hex2bin(line);
			if (!buffer.size())  // Ignore lines with bad checksums
				break;

			auto length = buffer[0];
			address_type address = (buffer[1] << 8) | buffer[2];
			const auto type = static_cast<data_type>(buffer[3]);
			auto data = &buffer[4];

			switch (type)
			{
			case data_type::data_block:
			{
				address += linear_address;
				for (auto& block : blocks) // Find a block that includes address
				{
					address_type num = 0;
					// If the start of the new block is interior to an existing block...
					if ((block.first <= address) && ((block.first + block.second.size()) > address))
					{
						// Store the portion of the new block that overlaps the existing block
						const auto index = address - block.first;
						num = block.second.size() - index;
						if (num > length)
							num = length;
						std::copy(data, data + num, &(block.second[index]));
					}
					// If the end of the new block is interior to an existing block...
					if ((address < block.first) && ((address + length) > block.first))
					{
						// Create a new block for the non-overlapping portion
						num = block.first - address;
						if (num > length)
							num = length;
						blocks[address].assign(data, data + num);
					}
					length -= num;
					address += num;
					data += num;
					// Bail out early if there's nothing left to do
					if (0 == length)
						break;
				}
				// Handle any leftover bytes
				if (length)
					blocks[address].assign(data, data + length);
				break;
			}
				
			case data_type::eof_record: 	// Ignore
				break;

			case data_type::segment_address:
				segment_addr_rec = true;
				break;
			
			case data_type::linear_address:
				if (0 == address && 2 == length)
				{
					linear_address = ((buffer[4] << 8) | buffer[5]) << 16;
					linear_addr_rec = true;
				}
				break;
			
			default: 
				break;;
			}
		}
		return true;
	}


	// Write all data to a file
	template <typename ValueType>
	bool hex_data<ValueType>::write(const char *path)
	{
		std::ofstream	ofs(path);
		if (!ofs)	// Bail out on bad files
			return false;
		const auto res = write(ofs);
		ofs.close();
		return res;
	}


	// Write all data to an output stream
	template <typename ValueType>
	bool hex_data<ValueType>::write(std::ostream &os)
	{
		uint8_t		checksum;
		uint16_t	linear_address(0);

		if (!os)	    // Bail out on bad streams
			return false;

		os.setf(std::ios::hex, std::ios::basefield);	//Set the stream to ouput hex instead of decimal
		os.setf(std::ios::uppercase);			//Use uppercase hex notation
		os.fill('0');							//Pad with zeroes

		//If we already know that this is an INHX32M file, start with a segment address record
		//	otherwise check all of the blocks just to make sure
		if (linear_addr_rec)
		{
			os << NEW_SEGMENT << INH32M_HEADER_END << std::endl;
		}
		else
		{
			for (auto& block : blocks)
			{
				if (block.first > 0xFFFF)	//Check the upper 16 bits
				{
					linear_addr_rec = true;
					os << NEW_SEGMENT << INH32M_HEADER_END << std::endl;
					break;	//Only need to find one
				}
			}
		}

		for (auto& block : blocks)
		{
			// Check upper 16 bits of the block address for non-zero,
			//  which indicates that a segment address record is needed
			if (block.first > 0xFFFF)
			{
				const uint16_t addr(block.first >> 16);
				//Has a record for this segment already been emitted?
				if (addr != linear_address)
				{
					//Emit a new segment address record
					os << NEW_SEGMENT;
					os.width(4);
					os << addr;	//Address
					// Create a checksum for the linear address record
					checksum = 0x06 + addr + (addr >> 8);
					checksum = 0x01 + ~checksum;
					os.width(2);
					// OSX (or maybe GCC), seems unable to handle uint8_t arguments to a stream
					os << static_cast<uint16_t>(checksum);	// Checksum byte
					os << std::endl;
					linear_address = addr;
				}
			}
			checksum = 0;
			os << ':';	//Every line begins with ':'
			os.width(2);
			os << block.second.size();				//Length
			checksum += block.second.size();
			os.width(4);
			os << static_cast<uint16_t>(block.first);		//Address
			checksum += static_cast<uint8_t>(block.first);		// Low byte
			checksum += static_cast<uint8_t>(block.first >> 8);	// High byte
			os << "00";											//Record type
			for (auto byte : block.second) //Store the data bytes, LSB first, ASCII HEX
			{
				os.width(2);
				// OSX (or maybe GCC), seems unable to handle uint8_t arguments to a stream
				os << static_cast<uint16_t>(byte);
				checksum += byte;
			}
			checksum = 0x01 + ~checksum;
			os.width(2);
			// OSX (or maybe GCC), seems unable to handle uint8_t arguments to a stream
			os << static_cast<uint16_t>(checksum);	// Checksum byte
			os << std::endl;
		}
		os << EOF_marker;			//EOF marker
		return true;
	}


	// Make things pretty
	//  Truncate blocks to a given length as needed
	template <typename ValueType>
	void hex_data<ValueType>::tidy(size_type length)
	{
		for (auto& block : blocks)
		{
			if (block.second.size() > length)	//If the block is too long...
			{
				//Make an interator that points to the first element to copy out of i->second
				auto k(block.second.begin());
				advance(k, length);

				// Assign the extra elements to a new block and truncate the original
				blocks[block.first + length].assign(k, block.second.end());
				block.second.erase(k, block.second.end());
			}
		}
	}



	// Convert a string from hex to binary and append it to a block
	std::vector<uint8_t> hex2bin(const std::string& from)
	{
		std::vector<uint8_t> to;
		to.reserve(from.size() / 2);
		to.clear();

		auto first = true;
		unsigned bin = 0;
		for (auto& i : from)
		{
			if (!hex2bin(i, bin))
				break;
			uint16_t value;
			if (first)
				value = bin << 4;
			else
			{
				value |= bin;
				to.push_back(value);
			}
			first = !first;
		}
		return to;
	}

	//Compare two sets of hex data
	//	Return true if every word in hex1 has a corresponding, and equivalent, word in hex2
	template <typename T>
	bool compare(hex_data<T>& hex1, hex_data<T>& hex2, const T mask, const address_type begin, const address_type end)
	{
		//Walk block list from hex1
		for (auto& i : hex1)
		{
			//Walk the block
			auto addr(i.first);
			for (auto& j : i.second)
			{
				++addr;
				if ((addr < begin) || (addr > end))
					continue;

				//Compare both sides through the given mask
				if ((j & mask) != (hex2.get(addr) & mask))
					return false;
			}
		}
		return true;
	}


	bool hex2bin(char ch, unsigned& val)
	{
		if (ch >= '0' && ch <= '9')
			val = ch - '0';
		else if (ch >= 'A' && ch <= 'F')
			val = ch - 'A' + 10;
		else if (ch >= 'a' && ch <= 'f')
			val = ch - 'a' + 10;
		else
			return false;
		return true;
	}
}
