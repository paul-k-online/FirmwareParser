#include "ascii_hex_block.h"


uint32_t ascii_hex_block::size() const
{
    return m_data.size();
}

uint32_t ascii_hex_block::address() const
{
    return m_address;
}

uint32_t ascii_hex_block::end_address() const
{
    return m_address + m_data.size();
}