#include <CppUnitTest.h>
#include "../FirmwareParser/hex_firmware_ascii.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define ASCII_SCI8      R"(..\..\FirmwareParser.Data\Blinker --ascii --boot --sci8.hex)"

namespace FirmwareParser_Test
{
    TEST_CLASS(hex_ascii_Test)
    {
    public:

        TEST_METHOD(Test_ascii_hex_parse)
        {
            hex_firmware_ascii<uint16_t> fw;
            fw.load(ASCII_SCI8);
            const auto data = fw.const_data_map();
            const auto size = data.size();
            Assert::AreEqual(size_t(5484), size_t(size));
        }
    };
}