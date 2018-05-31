#pragma once
#include <cstdint>
#include <vector>

class ascii_hex_block
{
public:

    explicit ascii_hex_block(uint32_t address, std::vector<uint16_t> data)
        : m_address(address), m_data(data)
    {}

    explicit ascii_hex_block(uint32_t address, uint32_t size, uint16_t fill = uint16_t(-1))
        : m_address(address)
    {
        m_data.resize(size, fill);
    }


    uint32_t size() const;
    uint32_t address() const;
    uint32_t end_address() const;

    static bool parse(std::vector<uint8_t> raw_data);

private:
    uint32_t m_address;                 ///< Address of block in FLASH memory
    std::vector<uint16_t> m_data;        ///< Block data
};

