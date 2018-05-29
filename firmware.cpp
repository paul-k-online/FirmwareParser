/** ************************************************************************
 * @file
 * @copyright UNIS a.s.
 * @copyright Aerospace and Advanced Control
 * @copyright Jundrovska 33
 * @copyright Brno, Czech Republic
 *
 * $HeadURL$
 * $Revision$
 * $Date$
 * $Author$
 *
 * @addtogroup firmware
 * @{
 *
 * @brief Firmware class
 *
 * ************************************************************************* */

#include "firmware.h"


/* =========================================================================
 *   Definition of global functions or procedures that is adjustable also
 *                            from other modules
 * ========================================================================= */

/** ************************************************************************
 * @brief Constructor of Firmware class
 *
 * This function reads the firmware file and initializes the list
 * of firmware blocks.
 *
 * @param   [in]  file_path  Reference to string containing path to file
 *
 * @return  n/a
 * ************************************************************************* */
Firmware::Firmware( const std::string& file_path )
    : m_is_valid(false)
{
    std::ifstream fw_file(file_path, std::ios::binary);

    if (fw_file.is_open()) {
        std::vector<uint8_t> data;

        // get length of file
        fw_file.seekg (0, std::ios::end);
        uint32_t size = uint32_t(fw_file.tellg());
        fw_file.seekg (0, std::ios::beg);

        char* const buf = new char[size];
        fw_file.read(buf, size);

        uint8_t tmp;
        bool msb = true;
        for (uint32_t i = 0; i < size; i++) {
            char c = buf[i];
            if (isxdigit(c) == 0) {
                msb = true;
            } else {
                if (msb) {
                    tmp = char_to_hex(c) << 4;
                    msb = false;
                } else {
                    tmp |= char_to_hex(c);
                    data.push_back(tmp);
                    msb = true;
                }
            }
        }
        fw_file.close();
        delete [] buf;
        m_is_valid = parse_fw(data);
    }
}

/** ************************************************************************
 * @brief Constructor of Firmware class
 *
 * This function initializes the list of firmware blocks.
 *
 * @param   [in]  fw_vec  Reference to vertor containing firmware data stream
 *
 * @return  n/a
 * ************************************************************************* */
Firmware::Firmware( const std::vector<uint8_t>& fw_vec )
    : m_is_valid(false)
{
    m_is_valid = parse_fw(fw_vec);
}

/** ************************************************************************
 * @brief Constructor of Firmware class
 *
 * This function initializes the empty list of firmware blocks.
 *
 * @param   [in,out]  n/a   n/a
 *
 * @return  n/a
 * ************************************************************************* */
Firmware::Firmware( void )
    : m_is_valid(false)
{
}

/** ************************************************************************
* @brief Constructor of Firmware class
*
* This function initializes the list of firmware blocks from parameter list
*
* @param   [in]  block_list   list of firmware blocks
*
* @return  n/a
* ************************************************************************* */
Firmware::Firmware(const std::list<Firmware_block>& block_list)
    : m_is_valid(!block_list.empty()), m_blocks(block_list)
{
}

/** ************************************************************************
 * @brief Check validity of firmware
 *
 * @param   [in,out]  n/a n/a
 *
 * @retval  true  If firmware block have a data
 * @retval  false If firmware block have no a data
 * ************************************************************************* */
bool Firmware::is_valid( void ) const
{
    return m_is_valid;
}

/** ************************************************************************
 * @brief Get firmware size
 *
 * @param   [in,out]  n/a n/a
 *
 * @return  Size of firmware in bytes
 * ************************************************************************* */
uint32_t Firmware::size (void) const
{
    uint32_t size = 0;
    for (auto& block : m_blocks) {
        size += block.size();
    }
    return size;
}

/** ************************************************************************
 * @brief Get reference to list of firmware blocks
 *
 * @param   [in,out]  n/a n/a
 *
 * @return Reference to list of firmware blocks
 * ************************************************************************* */
std::list<Firmware_block>& Firmware::blocks( void )
{
    return m_blocks;
}

/** ************************************************************************
 * @brief Get constant reference to list of firmware blocks
 *
 * @param   [in,out]  n/a n/a
 *
 * @return Constant reference to list of firmware blocks
 * ************************************************************************* */
const std::list<Firmware_block>& Firmware::const_blocks( void ) const
{
    return m_blocks;
}

/** ************************************************************************
 * @brief Add firmware block to the firmware
 *
 * Overloaded operator adding firmware block to the firmware
 *
 * @param   [in]  block  Reference to firmware block
 *
 * @return Reference to the current firmware object
 * ************************************************************************* */
Firmware& Firmware::operator+=( const Firmware_block& block )
{
    m_is_valid = true;
    m_blocks.push_back(block);
    return *this;
}

/** ************************************************************************
 * @brief Move the firmware block to the end
 *
 * This function finds the firmware block containing the address
 * and moves it to the end of the list
 *
 * @param   [in]  address  The address contained in the block that must be moved
 * @param   [in]  word_addressing If MCU is word addressing then 'true'
 *
 * @retval  true  If block found and moved
 * @retval  false If block not found and not moved
 * ************************************************************************* */
bool Firmware::move_block_to_end( uint32_t address, bool word_addressing )
{
    bool result = false;
    if (m_blocks.size() > 1) {
        for (auto& block : m_blocks) {
            uint32_t block_size_native = block.size();
            if (word_addressing) {
                block_size_native /= 2;
            }

            uint32_t end_block_address = block.address() + block_size_native;
            if ((address >= block.address()) && (address < end_block_address)) {
                std::swap(block, m_blocks.back());
                result = true;
                break;
            }
        }
    }
    return result;
}


/** ************************************************************************
 * @brief Save firmware to file
 *
 * @param   [in]  file_path  Reference to string containing path to file
 * @param   [in]  entry_point Address of entry point
 *
 * @retval  true  If firmware successfully saved
 * @retval  false If firmware is not saved
 * ************************************************************************* */
bool Firmware::save( const std::string& file_path, uint32_t entry_point )
{
    bool result = false;

    std::ofstream fw_file(file_path, std::ios::binary | std::ios::trunc);
    if (fw_file.is_open() && (m_blocks.empty() == false)) {
        std::string out;
        char str[13];

        // add start key, reserved
        out.append("\x02\r\nAA 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ");

        // add entry point
        long_to_str(str, entry_point);
        out.append(str);
        out.append("\r\n");

        for (auto& block : m_blocks) {
            uint32_t idx = 0;

            // add block length
            word_to_str(str, block.size()/2);
            out.append(str);
            idx += 2;

            // add block address
            long_to_str(str, block.address());
            out.append(str);
            idx += 4;

            for (auto& byte : block.const_data()) {
                if ((++idx % 24) == 0) {
                    out.append("\r\n");
                }
                byte_to_str(str, byte);
                out.append(str);
            }
            out.append("\r\n");
        }

        // end of data
        out.append("00 00 \r\n\x03");

        fw_file.write(out.c_str(), out.size());
        fw_file.close();
        result = true;
    }
    return result;
}

/** ************************************************************************
 * @brief Get the bootloader version
 *
 *
 * @param   [in]  sect_addr  Address of sector containing the bootloader version
 *
 * @retval  0     If sector is not found
 * @retval  other Bootloader version
 * ************************************************************************* */
uint16_t Firmware::bootloader_version( uint32_t sect_addr )
{
    uint16_t version = 0;
    for (auto& block : m_blocks) {
        if (sect_addr == block.address()) {
            version = block.const_data().at(0);
            version |= uint16_t(block.const_data().at(1)) << 8;
            break;
        }
    }
    return version;
}

/** ************************************************************************
 * @brief Get the bootloader MCU ID
 *
 *
 * @param   [in]  sect_addr  Address of sector containing the MCU ID
 *
 * @retval  0     If sector is not found
 * @retval  other MCU ID
 * ************************************************************************* */
uint16_t Firmware::bootloader_mcu_id( uint32_t sect_addr )
{
    uint16_t mcu_id = 0;
    for (auto& block : m_blocks) {
        if (sect_addr == block.address()) {
            mcu_id = block.const_data().at(2);
            mcu_id |= uint16_t(block.const_data().at(3)) << 8;
            break;
        }
    }
    return mcu_id;
}


/* =========================================================================
 *    Definition of local functions and procedures that is callable only
 *                              from the module
 * ========================================================================= */

/** ************************************************************************
 * @brief Parse firmware data stream
 *
 * Function parse firmware from data stream (8-bit).
 * SPRUGO0B "TMS320x2803x Piccolo Boot ROM".
 *
 * @param   [in]  fw_vec  Reference to data stream
 *
 * @retval  true  If firmware successfully parsed
 * @retval  false If firmware is not parsed
 * ************************************************************************* */
bool Firmware::parse_fw (const std::vector<uint8_t>& fw_vec)
{
    bool result = false;
    m_blocks.clear();

    if (fw_vec.size() > m_first_block_offset + m_empty_block_length) {
        uint32_t offset = m_first_block_offset;
        bool is_empty_block;

        do {
            std::vector<uint8_t> buf(fw_vec.begin()+ offset, fw_vec.end());
            Firmware_block block(buf);
            offset += block.raw_size();
            is_empty_block = block.is_empty();
            if (is_empty_block == false) {
                m_blocks.push_back(block);
            }
        } while ((offset < fw_vec.size()) && (is_empty_block == false));

        result = is_empty_block; // last block must be empty
    }
    return result;
}

/** ************************************************************************
 * @brief Convert HEX value to CHAR
 *
 * @param   [in]  hex  HEX value from 0x0 to 0xF
 *
 * @return CHAR
 * ************************************************************************* */
char Firmware::hex_to_char (uint8_t hex)
{
    const char tbl[16] = {'0', '1', '2', '3',
                          '4', '5', '6', '7',
                          '8', '9', 'A', 'B',
                          'C', 'D', 'E', 'F'};
    return tbl[hex & 0x0F];
}

/** ************************************************************************
 * @brief Convert CHAR to HEX value
 *
 * @param   [in]  c CHAR
 *
 * @return HEX value from 0x0 to 0xF
 * ************************************************************************* */
uint8_t Firmware::char_to_hex (char c)
{
    uint8_t tmp = 0;
    if (c >= '0' && c <= '9') {
        tmp = c - '0';
    } else if (c >= 'A' && c <= 'F') {
        tmp = c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        tmp = c - 'a' + 10;
    }
    return tmp;
}

/** ************************************************************************
 * @brief Convert byte (8-bit) to string
 *
 * @param   [out] str  Output string
 * @param   [in]  byte Byte value
 *
 * @return n/a
 * ************************************************************************* */
void Firmware::byte_to_str (char str[4], uint8_t byte)
{
    str[0] = hex_to_char(0x0F &(byte >> 4));
    str[1] = hex_to_char(0x0F &(byte >> 0));
    str[2] = ' ';
    str[3] = 0;
}

/** ************************************************************************
 * @brief Convert word (16-bit) to string
 *
 * @param   [out] str  Output string
 * @param   [in]  word Word value
 *
 * @return n/a
 * ************************************************************************* */
void Firmware::word_to_str (char str[7], uint16_t word)
{
    str[0] = hex_to_char(0x000F &(word >> 4));
    str[1] = hex_to_char(0x000F &(word >> 0));
    str[2] = ' ';
    str[3] = hex_to_char(0x000F &(word >> 12));
    str[4] = hex_to_char(0x000F &(word >> 8));
    str[5] = ' ';
    str[6] = 0;
}

/** ************************************************************************
 * @brief Convert double word (32-bit) to string
 *
 * @param   [out] str   Output string
 * @param   [in]  dword Double word value
 *
 * @return n/a
 * ************************************************************************* */
void Firmware::long_to_str (char str[13], uint32_t dword)
{
    word_to_str(str, 0xFFFF & (dword >> 16));
    word_to_str(str + 6, 0xFFFF & (dword >>  0));
}

/** @} */
