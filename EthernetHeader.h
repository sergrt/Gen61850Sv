#pragma once
#include <vector>
using namespace std;
const unsigned char MAC_LENGTH = 6;

class EthernetHeader {
public:
    EthernetHeader(const unsigned char dstMAC[MAC_LENGTH], const unsigned char srcMAC[MAC_LENGTH]);
    ~EthernetHeader();

    unsigned char srcMAC[MAC_LENGTH];
    unsigned char dstMAC[MAC_LENGTH];

    unsigned char TPID[2];
    unsigned char TCI[2];

    unsigned char EtherType[2];
    void setTCI(const unsigned char user_priority, const bool CFI, const unsigned short VID);
    void getPacket(vector<unsigned char>& res);
};
