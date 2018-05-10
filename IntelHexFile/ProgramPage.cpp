#include <ProgramPage.h>

ProgramPage::ProgramPage(size_t address, size_t pageSize, const vector<uint8_t>& pageData)
{
	this->address_ = address;
	this->page_size_ = pageSize;
	data_.resize(pageData.size());

	const auto startAddress = address % pageSize;

	for(auto i = startAddress; i < (startAddress + pageData.size()); i++)
	{
		data_[i] = pageData[i - startAddress];
	}
}


size_t ProgramPage::getAddress() const
{
	return address_;
}


size_t ProgramPage::getPageSize() const
{
	return page_size_;
}


size_t ProgramPage::getEndAddress() const
{
	return address_ + data_.size();
}


vector<uint8_t> ProgramPage::getData() const
{
	return data_;
}


ostream& operator<<(ostream& os, const ProgramPage& rhs)
{
	os << "[Start Address: 0x" << hex << rhs.address_;
	os << ", End Address: 0x" << hex << (rhs.address_+rhs.data_.size());
	os << ", Size: " << rhs.data_.size();
	os << ", Page Size: " << dec << rhs.page_size_;
	os << ", Data: ";
	for(auto it = rhs.data_.begin(); it != rhs.data_.end(); ++it)
	{
		const auto v = *it;
		os << hex << static_cast<size_t>(v);
	}
	os << dec << "]";
}

