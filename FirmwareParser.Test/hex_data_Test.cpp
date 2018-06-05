#include <CppUnitTest.h>
//#include "hex_data.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FirmwareParser_Test
{
	TEST_CLASS(hex_data_Test)
	{
	public:

		TEST_METHOD(Test_Method1)
		{
			//unsigned val = 0;
			//auto b = IntelHex::hex2bin('F', val);
			//Assert::AreEqual((int)val, (int)0xf);
		}

		TEST_METHOD(Test_Method2)
		{
			//std::string s("FF12ba");
			//const std::vector<uint8_t> v1{ 0xff,0x12, 0xba };
			//const auto v = IntelHex::hex2bin(s);
			//Assert::IsTrue(v == v1);
		}


		TEST_METHOD(Test_Method_Load)
		{
			//const std::string path("..//..//CAN_bootloader.hex.Intel");
			//auto parser = IntelHex::hex_data<uint8_t>(path);
		}
	};
}