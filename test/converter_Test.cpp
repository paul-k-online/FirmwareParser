#include <CppUnitTest.h>
#include "../src/converter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace FirmwareParser_Test
{
    TEST_CLASS(converter_Test)
    {
    public:
        TEST_METHOD(Test_read_dword)
        {
            std::vector<uint8_t> v{ 0x0, 0x0, 0x1, 0x2, 0x3, 0x4, 0x0, 0x0 , 0xEE};
            const auto iter = v.begin() + 2;

            uint32_t out;
            auto result = converter::big_order.read_dword(gsl::span<uint8_t>(iter._Ptr, 4), out);
            
            Assert::AreEqual(unsigned(0x01020304), unsigned(out));

        }

        TEST_METHOD(Test_local_endianness)
        {
            Assert::AreEqual(unsigned(converter::get_local_endian()), unsigned(converter::endianness::little));
        }

        TEST_METHOD(Test_byte_swap)
        {           
            uint16_t t1 = 0x1234;
            uint16_t t2 = converter::swap_bytes(t1);
            uint16_t t1_0 = converter::swap_bytes(t2);
            Assert::AreEqual(unsigned(t2), unsigned(0x3412U));
            Assert::AreEqual(unsigned(t1), unsigned(t1_0));

            uint32_t a1 = 0x003F7FF7;
            uint32_t a2 = 0x7FF7003F;
            Assert::AreEqual(unsigned(a1), unsigned(converter::swap_words(a2)));
            Assert::AreEqual(unsigned(a2), unsigned(converter::swap_words(a1)));
            Assert::AreEqual(unsigned(a1), unsigned(converter::swap_words(converter::swap_words(a1))));
        }

        TEST_METHOD(Test_endian_swap)
        {
            const uint32_t integer = 0x0A0B0C0D;

            uint8_t big_arr[] = { 0xA, 0xB, 0xC, 0xD };
            uint8_t lit_arr[] = { 0xD, 0xC, 0xB, 0xA };
            uint8_t mid_arr[] = { 0xB, 0xA, 0xD, 0xC };

            Assert::AreEqual(unsigned(integer),
                unsigned(converter::big_order.swap(
                    *reinterpret_cast<uint32_t*>(big_arr)
                )));

            Assert::AreEqual(unsigned(integer),
                unsigned(converter::little_order.swap(
                    *reinterpret_cast<uint32_t*>(lit_arr)
                )));

            Assert::AreEqual(unsigned(integer),
                unsigned(converter::middle_order.swap(
                    *reinterpret_cast<uint32_t*>(mid_arr)
                )));

            uint16_t int1 = 0x0A0B;

            uint8_t big_arr1[] = { 0xA, 0xB };
            uint8_t lit_arr1[] = { 0xB, 0xA };
            uint8_t mid_arr1[] = { 0xB, 0xA };

            uint16_t big1, lit1, mid1;

            memcpy(&big1, big_arr1, 2);
            memcpy(&lit1, lit_arr1, 2);
            memcpy(&mid1, mid_arr1, 2);
        }

        TEST_METHOD(Test_convert_vector)
        {
            std::vector<uint8_t> v{ 0xA, 0xB, 0xC, 0xD,  0x1, 0x2, 0x3, 0x4,  };
            std::vector<uint16_t> vo;

            const auto res = converter::big_order.convert_vector(v, vo);

            Assert::IsTrue(res);
            Assert::AreEqual(unsigned(0x0A0BU), unsigned(vo[0]));
            Assert::AreEqual(unsigned(0x0102U), unsigned(vo[2]));
        }

    };
}