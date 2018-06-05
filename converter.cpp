#include "converter.h"

converter::endianness converter::c2000 = converter(endianness::little, endianness::big);

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

uint16_t converter::make_word(uint8_t arr[])
{
    
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

