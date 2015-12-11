#include "SVPacket.h"
#ifndef WIN32
    #include <cstring> //for memcpy
#endif

SVPacket::SVPacket(const EthernetHeader& header, const unsigned char appid[2], const TLV& savPDU)
    : header(header) {
    this->APPID[0] = appid[0];
    this->APPID[1] = appid[1];

	this->savPDU = savPDU;
}

SVPacket::~SVPacket(void) {
}

void SVPacket::getPacket(vector< unsigned char >& res) {
	res.clear();	
	header.getPacket(res);

    /* Below is fully working piece of code commented for optimization
	res.push_back( APPID[ 0 ] );
	res.push_back( APPID[ 1 ] );

	//push length temporary two bytes
	res.push_back( 0x00 );
	res.push_back( 0x00 );
	int lengthPos = res.count() - 2;

	//push reserved
	res.push_back( 0x00 );
	res.push_back( 0x00 );

	res.push_back( 0x00 );
	res.push_back( 0x00 );

	QVector< unsigned char > savPDU_vector;
	savPDU.getPacket( savPDU_vector );
	
	for ( int x = 0; x < savPDU_vector.count(); x++ )
	{
		res.push_back( savPDU_vector.at( x ) );
	}	

	// set length
	res[ lengthPos     ] = static_cast< unsigned char >( ( savPDU_vector.count() + 4 ) >> 8 );
	res[ lengthPos + 1 ] = static_cast< unsigned char >( ( savPDU_vector.count() + 4 ) );
	*/

	int rs = res.size();
	vector<unsigned char> savPDU_vector;
    savPDU.getPacket(savPDU_vector);

	res.resize(rs + 2 + 2 + 2 + 2 + savPDU_vector.size());
    memcpy(res.data() + rs, &APPID, 2);

    char zeroes[2] = {0x00, 0x00};

    memcpy(res.data() + rs + 2, &zeroes, 2);
	int lengthPos = rs + 2;

    memcpy(res.data() + rs + 2 + 2, &zeroes, 2);
    memcpy(res.data() + rs + 2 + 2 + 2, &zeroes, 2);

	memcpy(res.data() + rs + 2 + 2 + 2 + 2, savPDU_vector.data(), savPDU_vector.size());
	
	// set length
	// added +4 after ENIP
	res[lengthPos    ] = static_cast<unsigned char>((savPDU_vector.size() + 4 + 4) >> 8);
	res[lengthPos + 1] = static_cast<unsigned char>((savPDU_vector.size() + 4 + 4));
}
