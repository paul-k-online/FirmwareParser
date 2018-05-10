/*  Routines for reading/writing Intel INHX8M and INHX32 files

    Copyright 2002 Brandon Fosdick (BSD License)
*/

#ifndef INTELHEXH
#define INTELHEXH


#include <map>
#include <vector>

//#include <unistd.h>

namespace IntelHex
{
#define	HEX_FORMAT_INHX8M	0x01
#define	HEX_FORMAT_INHX32	0x02
#define NEW_SEGMENT			":02000004"
#define	INH32M_HEADER_END	"0000FA"
	//#define	INH32M_HEADER	"02000004"+"0000FA"
#define EOF_marker			":00000001FF\n"

	//class   hex_data;
	//typedef hex_data<value_type> container;
	//typedef uint8_t value_type;
	typedef uint32_t address_type;

	//The data set that results from parsing a hex file
	template <typename ValueType>
	class hex_data
	{
	public:
		//Each line of the hex file generates a block of memory at a particular address
		typedef	std::vector<ValueType>	data_container;		//Element container
		//using data_container = std::vector<value_type>;
		typedef	std::map<address_type, data_container> container;   //List of data blocks
		typedef typename data_container::size_type	size_type;

		enum class data_type
		{
			data_block = 0,
			// EOF record
			eof_record = 1,
			// Segment address record (INHX32)
			segment_address = 2,
			//Linear address record (INHX32)
			linear_address = 4,
		};

	private:
		// Value returned for unset addresses
		ValueType  _fill = 0;
		//Format of the parsed file (necessary?)
		char	format = 0;
		// Uses/Has a segment address record
		bool	segment_addr_rec = false;
		// Uses/Has a linear address record
		bool	linear_addr_rec = false;
		// List of data blocks
		container   blocks;

	public:
		hex_data() = default;
		hex_data(const std::string &s) {
			load(s);
		}

		typename container::iterator    begin() { return blocks.begin(); }
		typename container::iterator    end() { return blocks.end(); }

		// Merge adjacent blocks
		void	compact();
		//Delete everything
		void	clear();
		// Erase a single element
		void	erase(address_type);
		// Erase [first, last]
		void 	erase(address_type first, address_type last);
		ValueType  fill() const { return _fill; }
		void	    fill(const ValueType f) { _fill = f; }
		size_type   size();
		size_type   size_below_addr(address_type);
		//number of words in [lo, hi)
		size_type   size_in_range(address_type, address_type);
		address_type   max_addr_below(address_type);

		// Lowest address
		address_type   min_address() const;
		// Highest address
		address_type   max_address() const;

		bool	is_set(address_type);

		//Array access operator
		ValueType& operator[](address_type);
		// Return the value at address
		ValueType  get(address_type);
		// Set the value at address
		void	set(address_type, ValueType);

		// Load from a file
		bool	load(const std::string&);
		// Read data from an input stream
		bool	read(std::istream &);
		//Save hex data to a hex file
		bool	write(const char *);
		//Write all data to an output stream
		bool	write(std::ostream &);
		// Make things pretty
		void	tidy(size_type length);
	};

	template <typename value_type>
	bool compare(hex_data<value_type>&, hex_data<value_type>&, value_type, address_type, address_type);

	std::vector<uint8_t> hex2bin(const std::string&);
	bool hex2bin(char, unsigned&);
}
#endif
