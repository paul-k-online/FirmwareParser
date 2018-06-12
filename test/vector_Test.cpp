#include <CppUnitTest.h>
#include <vector>

#include "../src/hex_firmware_intel.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FirmwareParser_Test
{
    TEST_CLASS(vector_Test)
    {
    public:

        TEST_METHOD(Test_join_pos_positive_1)
        {
            std::vector<int> dest = { 10,20,30,40,50,60,70,80,90,100 };
            std::vector<int> src = { 0,1,2,3,4,5,6,7,8,9 };
            hex_firmware<int>::join_vectors(dest, 5, src);
            Assert::AreEqual(dest.size(), 15U);
            Assert::AreEqual(dest[5], src[0]);
        }

        TEST_METHOD(Test_join_pos_positive_2)
        {
            std::vector<int> dest = { 10,20,30,40,50,60,70,80,90,100 };
            std::vector<int> src = { 0,1,2,3 };
            hex_firmware<int>::join_vectors(dest, 5, src, 0);
            Assert::AreEqual(dest.size(), 10U);
            Assert::AreEqual(dest[5], src[0]);
        }



        TEST_METHOD(Test_join_pos_negative_1)
        {
            std::vector<int> dest = { 10,20,30,40,50,60,70,80,90,100 };
            const std::vector<int> src = { 0,1,2,3 };
            hex_firmware<int>::join_vectors(dest, -1, src, 0);
            Assert::AreEqual(dest.size(), 11U);
            Assert::AreEqual(dest[0], src[0]);
        }

        TEST_METHOD(Test_join_pos_negative_2)
        {
            std::vector<int> dest = { 10,20,30 };
            std::vector<int> src = { 0,1,2,3,4,5,6,7,8,9 };
            hex_firmware<int>::join_vectors(dest, -1, src, 0);
            Assert::AreEqual(dest.size(), 10U);
            Assert::AreEqual(dest[0], src[0]);
        }


        TEST_METHOD(Test_join_pos_negative_3)
        {
            std::vector<int> dest = { 10,20,30,40,50,60,70,80,90,100 };
            std::vector<int> src = { 0,1,2,3 };
            hex_firmware<int>::join_vectors(dest, -5, src, 0);
            Assert::AreEqual(dest.size(), 15U);
            Assert::AreEqual(dest[0], src[0]);
            Assert::AreEqual(dest[0], src[0]);
        }
    };
}