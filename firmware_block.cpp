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
 * @addtogroup firmware block
 * @{
 *
 * @brief Firmware block class
 *
 * ************************************************************************* */

#include "firmware_block.h"

/* =========================================================================
 *   Definition of global functions or procedures that is adjustable also
 *                            from other modules
 * ========================================================================= */

/** ************************************************************************
 * @brief Constructor of Firmware_block class
 *
 * Initializes the firmware block from the data stream
 *
 * @param   [in]  data  Reference to data stream
 *
 * @return  n/a
 * ************************************************************************* */
Firmware_block::Firmware_block( const std::vector<uint8_t>& data )
    : m_address(0),
      m_data_map(0)
{
    uint32_t size = 0;

    if (data.size() > 1) {
        size = 2 * (data.at(0) | (data.at(1) << 8));
    }

    if ((size != 0) && ((size + 6) <= data.size())) {
        m_address = make_address(&data[2]);
        m_data_map.resize(size);

        for (uint32_t i = 0; i < size; i++) {
            m_data_map[i] = data[6 + i];
        }
    }
}


/** ************************************************************************
 * @brief Constructor of Firmware_block class
 *
 * Initializes the firmware block of specified size
 *
 * @param   [in]  size     Size of firmware block
 * @param   [in]  address  Address of firmware block in FLASH memory
 *
 * @return  n/a
 * ************************************************************************* */
Firmware_block::Firmware_block( uint32_t size, uint32_t address )
    : m_address(address),
      m_data_map(0)
{
    m_data_map.assign(size, 0xFF);
}

Firmware_block::Firmware_block(uint32_t address, const std::vector<uint8_t>& data)
    : m_address(address), m_data_map(data)
{
}

/** ************************************************************************
 * @brief Check firmware block for data
 *
 * @param   [in,out]  n/a n/a
 *
 * @retval  true  If firmware block have a data
 * @retval  false If firmware block have no a data
 * ************************************************************************* */
bool Firmware_block::is_empty( void ) const
{
    return m_data_map.empty();
}

/** ************************************************************************
 * @brief Get size of firmware block
 *
 * @param   [in,out]  n/a n/a
 *
 * @return Size of firmware block
 * ************************************************************************* */
uint32_t Firmware_block::size( void ) const
{
    return m_data_map.size();
}

/** ************************************************************************
 * @brief Get raw size of firmware block
 *
 * @param   [in,out]  n/a n/a
 *
 * @return Raw size of firmware block (size of data + size if header)
 * ************************************************************************* */
uint32_t Firmware_block::raw_size( void ) const
{
    uint32_t size = m_data_map.size();
    return (size > 0) ? (size + 6) : (2);
}

/** ************************************************************************
 * @brief Get address of firmware block address in FLASH
 *
 * @param   [in,out]  n/a n/a
 *
 * @return Address of firmware block address in FLASH
 * ************************************************************************* */
uint32_t Firmware_block::address( void ) const
{
    return m_address;
}

uint32_t Firmware_block::back_address() const
{
    return m_address + m_data_map.size();
}

/** ************************************************************************
 * @brief Get reference to data of firmware block
 *
 * @param   [in,out]  n/a n/a
 *
 * @return Reference to data of firmware block
 * ************************************************************************* */
std::vector<uint8_t>& Firmware_block::data( void )
{
    return m_data_map;
}

/** ************************************************************************
 * @brief Get reference to data of firmware block
 *
 * @param   [in,out]  n/a n/a
 *
 * @return Constant reference to data of firmware block
 * ************************************************************************* */
const std::vector<uint8_t>& Firmware_block::const_data( void ) const
{
    return m_data_map;
}

/** ************************************************************************
 * @brief Compare two firmware block
 *
 * Overloaded operator for comparing two firmware blocks
 *
 * @param   [in]  lhs  Reference to left operand
 * @param   [in]  rhs  Reference to right operand
 *
 * @retval  true  If firmware blocks are equal
 * @retval  false If firmware blocks are not equal
 * ************************************************************************* */
bool operator==( const Firmware_block& lhs, const Firmware_block& rhs )
{
    return (lhs.m_address == rhs.m_address)
            && (lhs.m_data_map == rhs.m_data_map);
}


/* =========================================================================
 *    Definition of local functions and procedures that is callable only
 *                              from the module
 * ========================================================================= */

/** ************************************************************************
 * @brief Make address of block
 *
 * Function make address of firmware block from data stream (8-bit).
 * SPRUGO0B "TMS320x2803x Piccolo Boot ROM".
 *
 * @param   [in]  array  Ponter to data stream which contain block address
 *
 * @return  Address of block in FLASH
 * ************************************************************************* */
uint32_t Firmware_block::make_address( const uint8_t* array )
{
    uint32_t addr = 0;
    if (array != nullptr) {
        addr = (array[0] << 16) | (array[1] << 24) | \
               (array[2] <<  0) | (array[3] <<  8);
    }
    return addr;
}

/** @} */
