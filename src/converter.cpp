#include "converter.h"
#include <algorithm>

converter converter::little_order = converter(endianness::little);
converter converter::big_order = converter(endianness::big);
converter converter::middle_order = converter(endianness::middle);


converter::endianness converter::get_local_endian()
{
    static uint32_t x = 0x11223344;
    const auto x1 = *reinterpret_cast<uint8_t*>(&x); // who 1st byte in array x
    switch (x1)
    {
        case 0x44: return endianness::little;
        case 0x11: return endianness::big;
        case 0x22: return endianness::middle;
        default: return endianness::unknown;
    }
}


bool converter::need_swap_bytes(endianness from, endianness to)
{
    if (from == endianness::unknown || to == endianness::unknown || from == to)
        return false;

    if ((from == endianness::little && to == endianness::middle) ||
        (from == endianness::middle && to == endianness::little))
        return false;

    return true;
}


bool converter::need_swap_words(endianness from, endianness to)
{
    if (from == endianness::unknown || to == endianness::unknown || from == to)
        return false;

    if ((from == endianness::big && to == endianness::middle) ||
        (from == endianness::middle && to == endianness::big))
        return false;

    return true;
}


converter::converter(const endianness order) 
    : m_endian(order)
{
    m_local_endian = get_local_endian();
    m_swap_bytes = need_swap_bytes(m_endian, m_local_endian);
    m_swap_words = need_swap_words(m_endian, m_local_endian);
}



uint64_t converter::hex_to_64(const std::string& s)
{
    std::stringstream o;
    o << std::hex << s;
    uint64_t v;
    o >> v;
    return v;
}

uint8_t converter::lo_byte(const uint16_t word)
{
    return word & 0xFF;
}

uint8_t converter::hi_byte(const uint16_t word)
{
    return word & 0xFF00 >> 8;
}

uint16_t converter::lo_word(const uint32_t word)
{
    return word & 0xFFFF;
}

uint16_t converter::hi_word(const uint32_t word)
{
    return word & 0xFFFF0000 >> 16;
}

void converter::split_word(uint16_t word, uint8_t& low, uint8_t& high)
{
    low = lo_byte(word);
    high = hi_byte(word);
}

void converter::split_dword(uint32_t dword, uint16_t & low, uint16_t& high)
{
    low = lo_word(dword);
    high = hi_word(dword);
}

uint16_t converter::make_word(uint8_t low, uint8_t high)
{
    return low | high << 8;
}

uint32_t converter::make_dword(uint16_t low_byte, uint16_t high_byte)
{
    return low_byte | high_byte << 16;
}

uint16_t converter::swap_bytes(uint16_t x)
{
    //x = ((x & 0xFF) << 8) | ((x & 0xFF00) >> 8);
    const auto ptr = reinterpret_cast<uint8_t*>(&x);
    std::swap(ptr[0], ptr[1]);
    return x;
}

uint32_t converter::swap_words(uint32_t x)
{
    //x = ((x & 0xFFFF) << 16) | ((x & 0xFFFF0000) >> 16);
    const auto ptr = reinterpret_cast<uint16_t*>(&x);
    std::swap(ptr[0], ptr[1]);
    return x;
}



uint16_t converter::swap(uint16_t & in) const
{
    if (m_swap_bytes)
        in = swap_bytes(in);
    return in;
}


uint32_t converter::swap(uint32_t & in) const
{
    if (m_swap_words)
        in = swap_words(in);
    const auto arr = reinterpret_cast<uint16_t*>(&in);
    swap(arr[0]);
    swap(arr[1]);
    return in;
}


//bool converter::hex_to_bin(const char ch, unsigned& val)
//{
//    if (ch >= '0' && ch <= '9')
//        val = ch - '0';
//    else if (ch >= 'A' && ch <= 'F')
//        val = ch - 'A' + 10;
//    else if (ch >= 'a' && ch <= 'f')
//        val = ch - 'a' + 10;
//    else
//        return false;
//    return true;
//}


// Convert a string from hex to binary and append it to a block
//std::vector<uint8_t> converter::hex_to_8_array(const std::string& from)
//{
//    std::vector<uint8_t> to;
//    to.reserve(from.size() / 2);
//    to.clear();
//
//    auto first = true;
//    for (auto& i : from)
//    {
//        unsigned bin = 0;
//        if (!hex_to_bin(i, bin))
//            break;
//        uint8_t value;
//        if (first)
//        {
//            value = bin << 4;
//        } else {
//            value |= bin;
//            to.emplace_back(value);
//        }
//        first = !first;
//    }
//    return to;
//}


bool converter::read_word(gsl::span<const uint8_t> in, uint16_t& out) const
{
    if (in.size() < 2)
        return false;
    std::memcpy(&out, in.data(), 2);
    out = swap(out);
    return true;
}


bool converter::read_dword(gsl::span<const uint8_t> in, uint32_t& out) const
{
    if (in.size() < 4)
        return false;
    std::memcpy(&out, in.data(), 4);
    out = swap(out);
    return true;
}

template<typename T>
bool converter::read(gsl::span<const uint8_t> in, T & out)
{
    const auto data_size = sizeof T;
    if (in.size() < data_size)
        return false;
    std::memcpy(&out, in.data(), data_size);
    out = swap(out);
    return true;
}


bool converter::join_vector(gsl::span<const uint8_t> lo_data, 
    gsl::span<const uint8_t> hi_data,
    std::vector<uint16_t>& word_data)
{
    auto result = true;
    if (result) {
        result = lo_data.size() == hi_data.size();
    }
    
    if (result) {
        for (size_t i = 0; i < lo_data.size(); i++) {
            word_data[i] = make_word(lo_data[i], hi_data[i]);
        }
    }
    return true;
}

bool converter::convert_vector(gsl::span<const uint8_t> in, std::vector<uint8_t>& out) const
{
    out.assign(in.cbegin(), in.cend());
    return true;
}


bool converter::convert_vector(gsl::span<const uint8_t> in, std::vector<uint16_t>& out) const
{
    const auto data_size = sizeof uint16_t;
    auto result = true;
    result = in.size() % data_size == 0;
    if (result) {
        out.clear();
        out.reserve(in.size() / data_size);
        for (auto iter = in.cbegin(); iter != in.cend(); iter += data_size) {
            uint16_t o;
            result = read_word(gsl::make_span(iter, data_size), o);
            if (result) {
                out.push_back(o);
            }
            
        }
    }
    return result;
}

bool converter::convert_vector(gsl::span<const uint8_t> in, std::vector<uint32_t>& out) const
{
    const auto data_size = sizeof uint32_t;
    auto result = true;
    result = in.size() % data_size == 0;
    if (result) {
        out.clear();
        out.reserve(in.size() / data_size);
        for (auto iter = in.cbegin(); iter != in.cend(); iter += data_size) {
            uint32_t o;
            result = read_dword(gsl::make_span(iter, data_size), o);
            if (result) {
                out.push_back(o);
            }

        }
    }
    return result;
}
