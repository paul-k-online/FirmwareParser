#include "Program.h"


Program::Program(const map<uint16_t, IntelHexFileEntry>& addressToFileEntries)
{
	const size_t hAddress = ((addressToFileEntries.rbegin())->second).getEndAddress();
	data.resize(hAddress);

	for(auto it = addressToFileEntries.begin(); it != addressToFileEntries.end(); ++it)
	{
		const auto address = static_cast<size_t>(it->first);
		auto entry = it->second;
		auto entry_data = entry.getData();

		for(size_t i = 0; i < entry_data.size(); i++)
		{
			auto v = entry_data[i];
			data[address + i] = v;
		}
	}
}


vector<ProgramPage> Program::getPages(size_t pageSize)
{
	vector<ProgramPage> pages;
	for(size_t i = 0; i < data.size(); i+= pageSize)
	{
		const auto pageEndAddress = min(i + pageSize, data.size());
		const vector<uint8_t> pageData(data.begin() +i, data.begin() + pageEndAddress);
		const ProgramPage p(i, pageSize, pageData);
		pages.push_back(p);
	}
	return pages;
}

ostream& operator<<(ostream& os, const Program& rhs)
{
	os << "[Num Bytes: " << rhs.data.size() << ", Data: ";
	for(auto it = rhs.data.begin(); it != rhs.data.end(); ++it)
	{
		const auto v = (*it);
		os << hex << static_cast<size_t>(v);
	}
	os << dec << "]";
}
