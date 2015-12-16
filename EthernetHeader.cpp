#include "EthernetHeader.h"
#ifndef WIN32
    #include <cstring> //for memcpy
#endif

EthernetHeader::EthernetHeader(const unsigned char dstMAC[MAC_LENGTH], const unsigned char srcMAC[MAC_LENGTH]) {
    for (unsigned char i = 0; i < MAC_LENGTH; i++) {
        this->dstMAC[i] = dstMAC[i];
        this->srcMAC[i] = srcMAC[i];
    }
    
    // TPID always the same
    TPID[0] = 0x81;
    TPID[1] = 0x00;

    // EtherType always the same
    EtherType[0] = 0x88;
    EtherType[1] = 0xBA;

    // Fill TCI with default values
    unsigned char user_priority = 4; // default priority = 4
    bool CFI = 0; // should be 0
    unsigned short VID = 0;	// default is 0
    setTCI(user_priority, CFI, VID);
}

EthernetHeader::~EthernetHeader(void) {
}

void EthernetHeader::setTCI(const unsigned char user_priority, const bool CFI, const unsigned short VID) {
    TCI[0] = 0;
    TCI[0] = user_priority << 5;
    TCI[0] |= static_cast< unsigned char >(CFI ? 1 : 0)  << 4;
    TCI[0] |= static_cast< unsigned char >(VID >> 8) & 0x0F;
    TCI[1] = static_cast< unsigned char>(VID);
}

// VLAN support have been removed
void EthernetHeader::getPacket(vector<unsigned char>& res) {
    res.clear();

    /*
    res.resize(MAC_LENGTH * 2 + 2 + 2 + 2);
    memcpy(res.data(), &dstMAC, MAC_LENGTH);
    memcpy(res.data() + MAC_LENGTH, &srcMAC, MAC_LENGTH);

    memcpy(res.data() + MAC_LENGTH * 2 , &TPID, 2);
    memcpy(res.data() + MAC_LENGTH * 2 + 2, &TCI, 2);
    memcpy(res.data() + MAC_LENGTH * 2 + 2 + 2, &EtherType, 2);
    */

    res.resize(MAC_LENGTH * 2 + 2);
    memcpy(res.data(), &dstMAC, MAC_LENGTH);
    memcpy(res.data() + MAC_LENGTH, &srcMAC, MAC_LENGTH);

    memcpy(res.data() + MAC_LENGTH * 2, &EtherType, 2);
}
