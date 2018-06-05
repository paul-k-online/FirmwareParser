#include <CppUnitTest.h>
#include "../FirmwareParser/converter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace FirmwareParser_Test
{
    TEST_CLASS(converter_Test)
    {
    public:
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

            uint32_t a3 = a1;
            //converter::swap(&a3);
        }


        TEST_METHOD(Test_endian_swap)
        {
            //uint8_t bytes[] = { 0x00, 0x3F, 0xF7, 0xF7 };
            //uint32_t bytes_address;
            //memcpy(&bytes_address, bytes, sizeof bytes /sizeof bytes[0]);

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


            //, lit, mid;

            //memcpy(&lit, lit_arr, 4);
            //memcpy(&mid, mid_arr, 4);





            uint16_t int1 = 0x0A0B;

            uint8_t big_arr1[] = { 0xA, 0xB };
            uint8_t lit_arr1[] = { 0xB, 0xA };
            uint8_t mid_arr1[] = { 0xB, 0xA };

            uint16_t big1, lit1, mid1;

            memcpy(&big1, big_arr1, 2);
            memcpy(&lit1, lit_arr1, 2);
            memcpy(&mid1, mid_arr1, 2);




            auto middle_converter = converter::middle_order;
            //middle_converter.to_local(local_address);



        }



    };
}