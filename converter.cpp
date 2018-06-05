#include "converter.h"

converter converter::little_order = converter(endianness::little);
converter converter::big_order = converter(endianness::big);
converter converter::middle_order = converter(endianness::middle);


bool converter::need_swap_bytes(endianness e1, endianness e2)
{
    if (e1 == endianness::unknown || e2 == endianness::unknown || e1 == e2)
        return false;

    if ((e1 == endianness::little && e2 == endianness::middle) ||
        (e1 == endianness::middle && e2 == endianness::little))
        return false;

    return true;
}

bool converter::need_swap_words(endianness e1, endianness e2)
{
    if (e1 == endianness::unknown || e2 == endianness::unknown || e1 == e2)
        return false;

    if ((e1 == endianness::big && e2 == endianness::middle) ||
        (e1 == endianness::middle && e2 == endianness::big))
        return false;

    return true;
}

converter::converter(const endianness order)
{
    m_endian = order;
    m_local_endian = get_local_endian();
    m_swap_bytes = need_swap_bytes(m_endian, m_local_endian);
    m_swap_words = need_swap_words(m_endian, m_local_endian);

}

uint16_t converter::swap(uint16_t x)
{
    x = ((x & 0xFF) << 8) | ((x & 0xFF00) >> 8);
    return x;
}

uint32_t converter::swap(uint32_t x)
{
    x = ((x & 0xFFFF) << 16) | ((x & 0xFFFF0000) >> 16);
    return x;
}

uint8_t converter::hi_byte(uint16_t word)
{
    return word & 0xFF00 >> 8;
}

uint8_t converter::lo_byte(uint16_t word)
{
    return word & 0xFF;
}

uint16_t converter::hi_word(uint32_t word)
{
    return word & 0xFFFF0000 >> 16;
}

uint16_t converter::lo_word(uint32_t word)
{
    return word & 0xFFFF;
}


void converter::from_local(uint16_t& in)
{
    if (need_swap_bytes(m_local_endian, m_endian))
        in = swap(in);
}

void converter::to_local(uint16_t &in)
{
    auto endian = get_local_endian();
    if (m_endian == get_local_endian() || m_endian == endianness::unknown)
        return;
    
    in = swap(in);
}



uint64_t converter::hex_to_64(const std::string& s)
{
    std::stringstream o;
    o << std::hex << s;
    uint64_t v;
    o >> v;
    return v;
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




uint16_t converter::make_word(uint8_t lsb, uint8_t msb)
{
    return lsb | msb << 8;
}


uint32_t converter::make_dword(uint16_t lsw, uint16_t msw)
{
    return  lsw | msw << 16;
}

void converter::split_word(uint16_t word, uint8_t& lsb, uint8_t& msb)
{
    lsb = uint8_t(word >> 0);
    msb = uint8_t(word >> 8);
}

void converter::split_dword(uint32_t dword, uint16_t& lsw, uint16_t& msw)
{
    lsw = uint16_t(dword >> 0);
    msw = uint16_t(dword >> 16);
}




bool converter::make_word(const std::vector<uint8_t>& lsb_vector, const std::vector<uint8_t>& msb_vector, std::vector<uint16_t>& word_vector)
{
    if (lsb_vector.size() != msb_vector.size()) 
    {
        word_vector.clear();
        return false;
    }
    
    word_vector.resize(lsb_vector.size());
    for (size_t i = 0; i<lsb_vector.size(); i++)
    {
        word_vector[i] = make_word(lsb_vector[i], msb_vector[i]);
    }
    return true;
}


template <typename DataType>
bool converter::convert_vector(const std::vector<uint8_t>& in, std::vector<DataType>& out)
{
    return false;
}

template <>
bool converter::convert_vector<uint16_t>(const std::vector<uint8_t>& in, std::vector<uint16_t>& out)
{
    const auto data_size = sizeof uint16_t;
    auto result = true;
    result = in.size() % data_size == 0;
    if (result)
    {
        out.reserve(in.size() / data_size);
        auto iter = in.begin();
        while (iter != in.end()) {
            const auto o = converter::make_word(*iter++, *iter++);
            out.push_back(o);
        }
    }
    return result;
}


template <>
bool converter::convert_vector<uint32_t>(const std::vector<uint8_t>& in, std::vector<uint32_t>& out)
{
    const auto data_size = sizeof uint32_t;
    auto result = true;
    result = in.size() % data_size == 0;
    if (result)
    {
        out.reserve(in.size() / data_size);
        auto iter = in.begin();
        while (iter != in.end()) {
            const auto msw = converter::make_word(*iter++, *iter++);
            const auto lsw = converter::make_word(*iter++, *iter++);
            const auto o = converter::make_dword(lsw, msw);
            out.push_back(o);
        }
    }
    return result;
}

template <typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& vec)
{
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(os));
    return os;
}

