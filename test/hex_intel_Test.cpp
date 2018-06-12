#include <CppUnitTest.h>

#define NOMINMAX
#include <algorithm>
#include <list>
#include <map>

#include "../src/hex_entry_intel.h"
#include "../src/hex_firmware.h"
#include "../src/hex_firmware_intel.h"
#include "../src/hex_firmware_c2000.h"
#include "../src/firmware.h"


//#define WINDOWS
//#include <stdio.h>  /* defines FILENAME_MAX */
//#ifdef WINDOWS
//#include <direct.h>
//#define GetCurrentDir _getcwd
//#endif
//char cCurrentPath[FILENAME_MAX];
//bool res = GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define INTEL_R8_LSB    R"(..\data\Blinker --intel --romwidth=8.hex)"
#define INTEL_R8_MSB    R"(..\data\Blinker --intel --romwidth=8.i01)"
#define ASCII_SCI8      R"(..\data\Blinker --ascii --boot --sci8.hex)"

namespace FirmwareParser_Test
{
	TEST_CLASS(hex_intel_Test)
	{
	public:
	    
	    TEST_METHOD(Test_intel_hex_entry)
		{
            const std::list<std::string> s {
                ":10010000214601360121470136007EFE09D2190140", //21 46 01 36 01 21 47 01 36 00 7E FE 09 D2 19 01
                ":100110002146017E17C20001FF5F16002148011928",
                ":10012000194E79234623965778239EDA3F01B2CAA7",
                ":100130003F0156702B5E712B722B732146013421C7",
                ":00000001FF",
            };

		    for (auto& s1 : s) {
                auto entry = hex_entry_intel(s1);
                Assert::IsTrue(entry.valid());
                auto s2 = entry.to_string();
                Assert::AreEqual(s1.c_str(), s2.c_str());
            }
		}


        TEST_METHOD(Test_intel_hex_file_load_and_shrink_1block)
        {
            hex_firmware_c2000 i_c2000;
            std::string filename(INTEL_R8_MSB);
            i_c2000.load(filename);

            //::parse_intelhex_file(INTEL_R8_MSB, entries);
            //Assert::AreEqual(size_t(91), entries.size());
            
	        //std::map<uint32_t, std::vector<uint8_t>> blocks;
            //auto res = intel_hex_firmware_c2000::to_data_map(entries, blocks);

            Assert::AreEqual(3U, i_c2000.data_map().size());
            Assert::AreEqual(3U, i_c2000.total_size());

            //Assert::AreEqual(size_t(2547),  blocks[0x3E8000].size());
            //Assert::AreEqual(size_t(162),   blocks[0x3F4000].size());
            //Assert::AreEqual(size_t(2),     blocks[0x3F7FF6].size());

            Firmware ascii_fw(ASCII_SCI8);
        }

	};
}