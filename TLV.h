#pragma once
#include <vector>
using namespace std;

class TLV {
public:
    TLV();
    TLV(const unsigned char tag, const vector<unsigned char>& data);
    TLV(const vector<unsigned char>& raw_data);

    void getData(vector<unsigned char>& res) const;
    unsigned short getFullLength() const;
    void getPacket(vector<unsigned char>& res) const;
private:
    unsigned char tag;
    unsigned short length;
    vector<unsigned char> data;
};
