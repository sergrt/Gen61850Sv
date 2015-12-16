#include "capture.h"
#include "SVPacket.h"
#include <QTest>
#include <QNetworkInterface>
#include "helpers.h"

const int MAX_DATA = 256;

CCapture::CCapture(QObject* parent, bool* stop) {
    stopThread = stop;
    QObject::connect(this, SIGNAL(setCaptureIsRunning(bool)), parent, SLOT(setCaptureIsRunning(bool)));
    QObject::connect(this, SIGNAL(displayData(vector<SVDecoded>*, bool*)), parent, SLOT(displayData(vector<SVDecoded>*, bool*)));
}

CCapture::~CCapture(void) {
}

// Some code commented out as it is not part of LE specification, 
// but it could be useful in future
void CCapture::decode_light(vector<unsigned char>& data, SVDecoded& res) {
    // Exclude ethernet header
    int trimFront = 22; //without TPID
    if (data[12] == 0x81 && data[13] == 0x00) // with TPID
        trimFront = 26;

    data.erase(data.begin(), data.begin() + trimFront);
    TLV savPDU(data);

    vector<unsigned char> savPDU_data;
    savPDU.getData(savPDU_data);

    TLV noASDU(savPDU_data);

    vector<unsigned char>& sequenceASDU_data = savPDU_data;
    unsigned int tmp_len = noASDU.getFullLength();
    sequenceASDU_data.erase(sequenceASDU_data.begin(), sequenceASDU_data.begin() + tmp_len);

    // check if security TLV present
    /*
    if ( savPDU_data[tmp_len] == 0x81 ) { //exists
        // need to remove this tag
    }
    */
    TLV sequenceASDU(sequenceASDU_data);

    vector<unsigned char> tmp;
    noASDU.getData(tmp);
    unsigned char noASDU_count = tmp[0];

    sequenceASDU.getData(sequenceASDU_data);

    for (unsigned char i = 0; i < noASDU_count; i++) {
        TLV ASDU(sequenceASDU_data);
        //ASDUs.push_back(ASDU);

        TLV& ASDU_cur = ASDU;
        vector<unsigned char> asdu_data;
        ASDU_cur.getData(asdu_data);
        TLV svID(asdu_data);
        asdu_data.erase(asdu_data.begin(), asdu_data.begin() + svID.getFullLength());

        // check for datset
        /*
        if (asdu_data[ 0 ] == 0x81) {
            //remove
        }
        */

        TLV smpCnt(asdu_data);
        asdu_data.erase(asdu_data.begin(), asdu_data.begin() + smpCnt.getFullLength());
        /*
        CTLV confRev(asdu_data);
        asdu_data.erase(asdu_data.begin(), asdu_data.begin() + confRev.getFullLength());
        */
        asdu_data.erase(asdu_data.begin(), asdu_data.begin() + 2 + asdu_data[1]);

        //check for refrTm
        /*
        if (asdu_data[ 0 ] == 0x84) {
            // remove
        }
        */
        /*
        CTLV smpSynch(asdu_data);
        asdu_data.erase(asdu_data.begin(), asdu_data.begin() + smpSynch.getFullLength());
        */
        asdu_data.erase(asdu_data.begin(), asdu_data.begin() + 2 + asdu_data[1]);
        // check for smpRate
        /*
        if (asdu_data[ 0 ] == 0x86) {
            //remove
        }
        */

        TLV sequenceData(asdu_data);
        //const unsigned char IUPhaseValues = 8; // Ua, Ub, Uc, Un, Ia, Ib, Ic, In
        sequenceData.getData(asdu_data);

        int Ua = 0;
        int Ub = 0;
        int Uc = 0;
        int Un = 0;
        int Ia = 0;
        int Ib = 0;
        int Ic = 0;
        int In = 0;

        int QUa = 0;
        int QUb = 0;
        int QUc = 0;
        int QUn = 0;
        int QIa = 0;
        int QIb = 0;
        int QIc = 0;
        int QIn = 0;

        rmemcpy((unsigned char*)&Ia, asdu_data.data(), 4);
        rmemcpy((unsigned char*)&QIa, asdu_data.data() + 4, 4);

        rmemcpy((unsigned char*)&Ib, asdu_data.data() + 8, 4);
        rmemcpy((unsigned char*)&QIb, asdu_data.data() + 12, 4);

        rmemcpy((unsigned char*)&Ic, asdu_data.data() + 16, 4);
        rmemcpy((unsigned char*)&QIc, asdu_data.data() + 20, 4);

        rmemcpy((unsigned char*)&In, asdu_data.data() + 24, 4);
        rmemcpy((unsigned char*)&QIn, asdu_data.data() + 28, 4);

        rmemcpy((unsigned char*)&Ua, asdu_data.data() + 32, 4);
        rmemcpy((unsigned char*)&QUa, asdu_data.data() + 36, 4);

        rmemcpy((unsigned char*)&Ub, asdu_data.data() + 40, 4);
        rmemcpy((unsigned char*)&QUb, asdu_data.data() + 44, 4);

        rmemcpy((unsigned char*)&Uc, asdu_data.data() + 48, 4);
        rmemcpy((unsigned char*)&QUc, asdu_data.data() + 52, 4);

        rmemcpy((unsigned char*)&Un, asdu_data.data() + 56, 4);
        rmemcpy((unsigned char*)&QUn, asdu_data.data() + 60, 4);

        smpCnt.getData(tmp);
        int counter = 0;
        rmemcpy((unsigned char*)&counter, tmp.data(), tmp.size());

        res.counter.push_back(counter);

        svID.getData(tmp);
        string svID_str;
        for(int x = 0; x < tmp.size(); x++)
            svID_str.push_back(tmp[x]);

        res.svID.push_back(svID_str);

        res.Ua.push_back(Ua/100.0);
        res.Ub.push_back(Ub/100.0);
        res.Uc.push_back(Uc/100.0);
        res.Un.push_back(Un/100.0);

        res.QUa.push_back(QUa);
        res.QUb.push_back(QUb);
        res.QUc.push_back(QUc);
        res.QUn.push_back(QUn);

        res.Ia.push_back(Ia/1000.0);
        res.Ib.push_back(Ib/1000.0);
        res.Ic.push_back(Ic/1000.0);
        res.In.push_back(In/1000.0);

        res.QIa.push_back(QIa);
        res.QIb.push_back(QIb);
        res.QIc.push_back(QIc);
        res.QIn.push_back(QIn);

        unsigned short len = ASDU.getFullLength();
        sequenceASDU_data.erase(sequenceASDU_data.begin(), sequenceASDU_data.begin() + len);
    }
}

// This is less efficient legacy code
void CCapture::decode(const vector<unsigned char>& raw_data, SVDecoded& res) {
    vector<unsigned char> data = raw_data;
    // Exclude ethernet header
    int trimFront = 22; //without TPID
    if (data[ 12 ] == 0x81 && data[ 13 ] == 0x00) // with TPID
        trimFront = 26;

    data.erase(data.begin(), data.begin() + trimFront);

    TLV savPDU(data);
    vector<unsigned char> savPDU_data;
    savPDU.getData(savPDU_data);

    TLV noASDU(savPDU_data);

    vector<unsigned char> tmp;
    noASDU.getPacket(tmp);

    vector<unsigned char> sequenceASDU_data = savPDU_data;
    sequenceASDU_data.erase(sequenceASDU_data.begin(), sequenceASDU_data.begin() + tmp.size());

    // check if security TLV present
    /*
    if ( savPDU_data[ tmp.count() ] == 0x81 )  {//exists
        // need to remove this tag
    }
    */

    TLV sequenceASDU(sequenceASDU_data);

    tmp.clear();
    noASDU.getData(tmp);
    unsigned char noASDU_count = tmp[0];

    sequenceASDU.getData(sequenceASDU_data);

    vector<TLV> ASDUs;
    for (unsigned char i = 0; i < noASDU_count; i++) {
        TLV ASDU(sequenceASDU_data);
        ASDUs.push_back(ASDU);

        unsigned short len = ASDU.getFullLength();
        sequenceASDU_data.erase(sequenceASDU_data.begin(), sequenceASDU_data.begin() + len);
    }

    for (int i = 0; i < noASDU_count; i++) {
        TLV ASDU_cur = ASDUs[i];

        vector<unsigned char> asdu_data;
        ASDU_cur.getData(asdu_data);

        TLV svID(asdu_data);
        asdu_data.erase(asdu_data.begin(), asdu_data.begin() + svID.getFullLength());

        // check for datset
        /*
        if (asdu_data[ 0 ] == 0x81 ) {
            //remove
        }
        */

        TLV smpCnt(asdu_data);
        asdu_data.erase(asdu_data.begin(), asdu_data.begin() + smpCnt.getFullLength());

        TLV confRev( asdu_data );
        asdu_data.erase(asdu_data.begin(), asdu_data.begin() + confRev.getFullLength());

        //check for refrTm
        /*
        if (asdu_data[ 0 ] == 0x84) {
            // remove
        }
        */

        TLV smpSynch(asdu_data);
        asdu_data.erase(asdu_data.begin(), asdu_data.begin() + smpSynch.getFullLength());

        // check for smpRate
        /*
        if ( asdu_data[ 0 ] == 0x86 ) {
            //remove
        }
        */

        TLV sequenceData(asdu_data);
        sequenceData.getData( asdu_data );
        int Ua = 0;
        int Ub = 0;
        int Uc = 0;
        int Un = 0;
        int Ia = 0;
        int Ib = 0;
        int Ic = 0;
        int In = 0;

        int QUa = 0;
        int QUb = 0;
        int QUc = 0;
        int QUn = 0;
        int QIa = 0;
        int QIb = 0;
        int QIc = 0;
        int QIn = 0;

        rmemcpy((unsigned char*)&Ia, asdu_data.data(), 4);
        rmemcpy((unsigned char*)&QIa, asdu_data.data() + 4, 4);

        rmemcpy((unsigned char*)&Ib, asdu_data.data() + 8, 4);
        rmemcpy((unsigned char*)&QIb, asdu_data.data() + 12, 4);

        rmemcpy((unsigned char*)&Ic, asdu_data.data() + 16, 4);
        rmemcpy((unsigned char*)&QIc, asdu_data.data() + 20, 4);

        rmemcpy((unsigned char*)&In, asdu_data.data() + 24, 4);
        rmemcpy((unsigned char*)&QIn, asdu_data.data() + 28, 4);

        rmemcpy((unsigned char*)&Ua, asdu_data.data() + 32, 4);
        rmemcpy((unsigned char*)&QUa, asdu_data.data() + 36, 4);

        rmemcpy((unsigned char*)&Ub, asdu_data.data() + 40, 4);
        rmemcpy((unsigned char*)&QUb, asdu_data.data() + 44, 4);

        rmemcpy((unsigned char*)&Uc, asdu_data.data() + 48, 4);
        rmemcpy((unsigned char*)&QUc, asdu_data.data() + 52, 4);

        rmemcpy((unsigned char*)&Un, asdu_data.data() + 56, 4);
        rmemcpy((unsigned char*)&QUn, asdu_data.data() + 60, 4);

        /* upper variant seems to be slight more efficient
        for ( int x = 0; x < 4; x++ )
        {
            Ia  |= asdu_data[  4 - x - 1 ] << ( x * 8 );
            QIa |= asdu_data[  8 - x - 1 ] << ( x * 8 );

            Ib  |= asdu_data[ 12 - x - 1 ] << ( x * 8 );
            QIb |= asdu_data[ 16 - x - 1 ] << ( x * 8 );

            Ic  |= asdu_data[ 20 - x - 1 ] << ( x * 8 );
            QIc |= asdu_data[ 24 - x - 1 ] << ( x * 8 );

            In  |= asdu_data[ 28 - x - 1 ] << ( x * 8 );
            QIn |= asdu_data[ 32 - x - 1 ] << ( x * 8 );


            Ua  |= asdu_data[ 36 - x - 1 ] << ( x * 8 );
            QUa |= asdu_data[ 40 - x - 1 ] << ( x * 8 );

            Ub  |= asdu_data[ 44 - x - 1 ] << ( x * 8 );
            QUb |= asdu_data[ 48 - x - 1 ] << ( x * 8 );

            Uc  |= asdu_data[ 52 - x - 1 ] << ( x * 8 );
            QUc |= asdu_data[ 56 - x - 1 ] << ( x * 8 );

            Un  |= asdu_data[ 60 - x - 1 ] << ( x * 8 );
            QUn |= asdu_data[ 64 - x - 1 ] << ( x * 8 );
        }
        */

        smpCnt.getData( tmp );
        int counter = 0;
        /*
        for ( int x = 0; x < tmp.count(); x++ )
            counter |= tmp[ tmp.count() - x - 1 ] << ( x * 8 );
        */
        rmemcpy((unsigned char*)&counter, tmp.data(), tmp.size());

        res.counter.push_back(counter);

        svID.getData(tmp);
        string svID_str;
        for(int x = 0; x < tmp.size(); x++)
            svID_str.push_back(tmp[x]);

        res.svID.push_back(svID_str);

        res.Ua.push_back(Ua);
        res.Ub.push_back(Ub);
        res.Uc.push_back(Uc);
        res.Un.push_back(Un);

        res.QUa.push_back(QUa);
        res.QUb.push_back(QUb);
        res.QUc.push_back(QUc);
        res.QUn.push_back(QUn);

        res.Ia.push_back(Ia);
        res.Ib.push_back(Ib);
        res.Ic.push_back(Ic);
        res.In.push_back(In);

        res.QIa.push_back(QIa);
        res.QIb.push_back(QIb);
        res.QIc.push_back(QIc);
        res.QIn.push_back(QIn);
    }
}

void CCapture::init(const QString& streamName, const QString& mac) {
#ifdef WIN32
    this->streamName = streamName.toLocal8Bit();
#else
    this->streamName = streamName.toLocal8Bit().data();
#endif
    streamFilter = "(ether proto 0x8100 or ether proto 0x88ba) and ether dst " + mac;
    captureMac = mac.toUpper();
}

void CCapture::run() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t* alldevs;
    pcap_findalldevs(&alldevs, errbuf);
    pcap_if_t *d = alldevs;

    QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    int ifsID = 0;
    while (ifsID < ifs.count()) {
        if (ifs.at(ifsID).hardwareAddress() == captureMac)
            break;
        ifsID++;
    }
    
    QString ifName = ifs.at(ifsID).name();
    QString curID = QString(d->name);
    curID = curID.right(curID.length() - curID.indexOf('{'));
    while (ifName != curID) {
        d = d->next;
        curID = QString(d->name);
        curID = curID.right(curID.length() - curID.indexOf('{'));
    }

    pcapHandle = pcap_open_live(d->name,            // name of the device
        4096/*65535*/,                // portion of the packet to capture
        1,  // promiscuous mode
        1000,               // read timeout
        errbuf              // error buffer
        );

    // compile and set filters
    struct bpf_program filter;
    char* filter_app = new char[streamFilter.length() + 1];
    memcpy(filter_app, streamFilter.toLocal8Bit().data(), streamFilter.length());
    filter_app[streamFilter.length()] = 0x00;

    bpf_u_int32 mask;
    bpf_u_int32 net;

    //Get net address and interface mask
    pcap_lookupnet(d->name, &net, &mask, errbuf);

    if (pcap_compile(pcapHandle, &filter, filter_app, 0, mask) == -1) {
        //exit(-1);
    }
    delete [] filter_app;

    if (pcap_setfilter(pcapHandle, &filter) == -1) {
        //exit(-1);
    }

    dataMeasured.clear();
    emit setCaptureIsRunning(true);

    bool doAdd = false;

    // Data logging
    //QFile f("_data.log");
    //f.open(QIODevice::WriteOnly);

    while(!*stopThread) {
        pcap_pkthdr* pkt_header;
        const u_char* pkt_data;
        int res = pcap_next_ex(pcapHandle, &pkt_header, &pkt_data);

        if (res == 1) { //valid data
            vector<unsigned char> packet_data;
            packet_data.resize(pkt_header->caplen);
            memcpy(packet_data.data(),pkt_data,pkt_header->caplen);

            SVDecoded d;
            decode_light(packet_data, d);

            if (d.svID[0] == streamName) {
                //for (size_t zx = 0; zx < d.counter.size();zx++)
                    //f.write(QString("%1 | %2\n").arg(d.counter.at(zx)).arg(d.Ua.at(zx)).toLocal8Bit());

                if (!doAdd) {
                    if (d.counter[0] == 0)
                        doAdd = true;
                }
                if (doAdd) {
                    if (dataMeasured.size() < MAX_DATA) {
                        dataMeasured.push_back(d);
                    } else {
                        bool ready = false;
                        emit displayData(&dataMeasured,&ready);
                        while(!ready)
                            uDelay(3);
                        dataMeasured.clear();
                        doAdd = false;
                    }
                }
            }
        } else if (res == 0) { //timeout - do nothing
            doAdd = false;
            dataMeasured.clear();
            bool ready = false;
            emit displayData(&dataMeasured,&ready);
            while(!ready)
                uDelay(3);
        } else if (res < 0) {
                *stopThread = true;
        }
    }

    //f.close();
   
    pcap_close(pcapHandle);
    emit setCaptureIsRunning(false);
}
