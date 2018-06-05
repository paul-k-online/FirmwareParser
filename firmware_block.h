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
 * ************************************************************************* */

#ifndef __FIRMWARE_BLOCK_H__
#define __FIRMWARE_BLOCK_H__

/* =========================================================================
 *                    GLOBAL PART OF THE HEADER FILE
 * ========================================================================= */
#include <cstdint>
#include <vector>

/* =========================================================================
 *                            Global data types
 * ========================================================================= */
class Firmware_block
{
public:
    Firmware_block( const std::vector<uint8_t>& data );
    Firmware_block( uint32_t size, uint32_t address );
    Firmware_block( uint32_t address, const std::vector<uint8_t>& data);

    bool is_empty( void ) const;
    uint32_t size( void ) const;
    uint32_t raw_size( void ) const;
    uint32_t address(void) const;
    uint32_t back_address( void ) const;

    std::vector<uint8_t>& data( void );
    const std::vector<uint8_t>& const_data( void ) const;

    friend bool operator==( const Firmware_block& lhs, const Firmware_block& rhs );

private:
    static uint32_t make_address(const uint8_t* array );

    uint32_t m_address;                 ///< Address of block in FLASH memory
    std::vector<uint8_t> m_data_map;        ///< Block data
};

#endif   /* #ifndef __FIRMWARE_BLOCK_H__ */

/** @} */
