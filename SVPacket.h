#pragma once
#include "EthernetHeader.h"
#include "TLV.h"

class SVPacket {
public:
    SVPacket(const EthernetHeader& header, const unsigned char appid[2], const TLV& savPDU);
    ~SVPacket(void);

private:
    EthernetHeader header;
    unsigned char APPID[2]; // APPID 0x4000 (default) to 0x7FFF
    TLV savPDU;
public:
    void getPacket(vector<unsigned char>& res);
};
