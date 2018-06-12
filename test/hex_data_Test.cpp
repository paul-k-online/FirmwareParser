#include <CppUnitTest.h>
#include "../src/hex_data.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define INTEL_R8_LSB    R"(..\data\Blinker --intel --romwidth=8.hex)"
#define INTEL_R8_MSB    R"(..\data\Blinker --intel --romwidth=8.i01)"
#define INTEL_R16       R"(..\data\Blinker --intel --romwidth=16.hex)"
#define ASCII_SCI8      R"(..\data\Blinker --ascii --boot --sci8.hex)"


namespace FirmwareParser_Test
{
	TEST_CLASS(hex_data_Test)
	{
	public:

		TEST_METHOD(Test_Method_Load)
		{
			auto parser = IntelHex::hex_data<uint16_t>(INTEL_R16);
		}
	};
}