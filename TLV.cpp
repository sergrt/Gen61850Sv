#include "TLV.h"
#ifndef WIN32
    #include <cstring> //for memcpy
#endif


TLV::TLV() {
	tag = 0x00;
	length = 0x00;
	data.clear();
}

TLV::TLV(const unsigned char tag, const vector<unsigned char>& data) {
	this->tag = tag;
	this->length = data.size();
	this->data = data;
}

TLV::TLV(const vector<unsigned char>& raw_data) {
	// need check for zero size, etc
    this->tag = raw_data[0];
    unsigned char length_len = raw_data[1];
	unsigned short data_begin = 2;

    if (length_len & 0x80) {
		length_len &= 0x7F;
		int len = 0;
		for (int i = 1 + length_len; i >= 2; i--)
			len |= raw_data[i] << ((length_len - i + 1) * 8);

		this->length = len;
		data_begin = 2 + length_len;
    } else {
        this->length = raw_data[1];
	}
	
	this->data.resize(this->length);
	memcpy(this->data.data(), raw_data.data() + data_begin, this->length);
}

void TLV::getData(vector<unsigned char>& res) const {
	res = data;
}

unsigned short TLV::getFullLength() const {
	unsigned short res = 1;
    if (length > 0x7F) {
		res += 1;
		// as far as we use only short type, 
		res += 2;
    } else {
		res += 1;
	}
	res += data.size();
	return res;
}

void TLV::getPacket(vector<unsigned char>& res) const{
	res.clear();
    res.push_back(tag);
    if (length > 0x7F) {
		// Можно для int переделать - 82 83, 84
		// пока только для short
        res.push_back(0x82);
        res.push_back(static_cast<unsigned char>(length >> 8));
        res.push_back(static_cast<unsigned char>(length));
    } else {
        res.push_back(static_cast< unsigned char >(length));
	}
	
	int rs = res.size();
	int new_size = data.size() + rs;

    res.resize(new_size);
	unsigned char * cc = res.data() + rs;
	memcpy(cc, data.data(), data.size());
}
