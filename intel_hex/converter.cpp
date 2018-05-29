#include "converter.h"



uint64_t converter::hex_to_64(const std::string& s)
{
    std::stringstream o;
    o << std::hex << s;
    uint64_t v;
    o >> v;
    return v;
}


bool converter::hex_to_bin(const char ch, unsigned& val)
{
    if (ch >= '0' && ch <= '9')
        val = ch - '0';
    else if (ch >= 'A' && ch <= 'F')
        val = ch - 'A' + 10;
    else if (ch >= 'a' && ch <= 'f')
        val = ch - 'a' + 10;
    else
        return false;
    return true;
}


// Convert a string from hex to binary and append it to a block
std::vector<uint8_t> converter::hex_to_8_array(const std::string& from)
{
    std::vector<uint8_t> to;
    to.reserve(from.size() / 2);
    to.clear();

    auto first = true;

    for (auto& i : from)
    {
        unsigned bin = 0;
        if (!hex_to_bin(i, bin))
            break;
        uint8_t value;
        if (first)
            value = bin << 4;
        else
        {
            value |= bin;
            to.emplace_back(value);
        }
        first = !first;
    }
    return to;
}
