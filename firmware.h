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
 * ************************************************************************* */

#ifndef __FIRMWARE_H__
#define __FIRMWARE_H__

/* =========================================================================
 *                    GLOBAL PART OF THE HEADER FILE
 * ========================================================================= */
#include <list>
#include <string>

#include <iostream>
#include <fstream>

#include "firmware_block.h"


/* =========================================================================
 *                            Global data types
 * ========================================================================= */
class Firmware
{
public:
    Firmware( const std::string& file_path );
    Firmware( const std::vector<uint8_t>& fw_vec );
    Firmware( void );

    Firmware( const std::list<Firmware_block>& block_list );


    bool is_valid( void ) const;
    uint32_t size( void ) const;

    std::list<Firmware_block>& blocks( void );
    const std::list<Firmware_block>& const_blocks( void ) const;

    Firmware& operator+=( const Firmware_block& block );
    bool move_block_to_end(uint32_t address, bool word_addressing );
    bool save( const std::string& file_path, uint32_t entry_point );

    uint16_t bootloader_version( uint32_t sect_addr );
    uint16_t bootloader_mcu_id( uint32_t sect_addr );

private:
    bool parse_fw( const std::vector<uint8_t>& fw_vec );

    uint8_t char_to_hex( char c );
    char hex_to_char( uint8_t hex );
    void byte_to_str( char str[4], uint8_t byte );
    void word_to_str( char str[7],  uint16_t word );
    void long_to_str( char str[13], uint32_t dword );

    bool m_is_valid;
    std::list<Firmware_block> m_blocks;
    static const uint32_t m_first_block_offset = 22;
    static const uint32_t m_empty_block_length = 2;
};


#endif   /* #ifndef __TEMPLATE_H__ */

/** @} */
