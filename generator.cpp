#include "generator.h"
#define WPCAP
#define HAVE_REMOTE
#include "pcap.h"
#undef min

#include <QTest>
#include "qmath.h"
#include <QNetworkInterface>
#include "helpers.h"
#include <cfloat>

CGenerator::CGenerator(QObject* parent, bool* stop, SGenParams* params/*, CSyncObject * sync*/)
    : header(params->dstMac, params->srcMac) {
    m_Stop = stop;
    genParams = params;
    //m_sync = sync;
    QObject::connect(this, SIGNAL(setGeneratorIsRunning(bool)), parent, SLOT(setGeneratorIsRunning(bool)));
    //QObject::connect( this, SIGNAL( setGeneratorStartTime() ), parent, SLOT( setGeneratorStartTime() ) );
}

CGenerator::~CGenerator(void) {
}

void CGenerator::run() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs;
    pcap_findalldevs(&alldevs, errbuf);
    pcap_if_t *d = alldevs;

    QString mac;
    SGenParams genParamsLocal(*genParams);
    
    for (int i = 0; i < sizeof(genParamsLocal.srcMac); i++)
        mac += QString("%1:").arg(genParamsLocal.srcMac[i], 2, 16, QChar('0')).toUpper();

    mac.remove(mac.length() - 1, 1);

    QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    int ifsID = 0;
    while (ifsID < ifs.count()) {
        qDebug(ifs.at(ifsID).hardwareAddress().toLocal8Bit());
        qDebug(mac.toLocal8Bit());

        if (ifs.at(ifsID).hardwareAddress() == mac)
            break;
        ifsID++;
    }

    auto targetAddresses = ifs.at(ifsID).addressEntries();
    bool foundInterface = false;
    while (d) {
        auto address = d->addresses;
        while (address) {
            auto addrStr = AddrToString(address->addr);
            for (auto targetAddress : targetAddresses) {
                if (targetAddress.ip().toString() == addrStr) {
                    foundInterface = true;
                    break;
                }
            }
            if (foundInterface)
                break;
            address = address->next;
        }
        if (foundInterface)
            break;
        d = d->next;
    }

    pcap_t* fp= pcap_open_live(d->name, // name of the device
        4096,                           // portion of the packet to capture
        1,                              // promiscuous mode
        1000,                           // read timeout
        errbuf                          // error buffer
        );

    emit setGeneratorIsRunning(true);
    
    int cnt = 0; // SV counter
    counterForGeneration = 0;
    int total_transmitted = 0;

    QTime controlTimer;
    controlTimer.start();
    int controlFrameCounter = 0;

    delayTime = 100;
    frameTimer.start();
    frameCounter = 0;
    const int FRAME_CNT_UPDATE_TIMER = 16; // How often udate sleep interval
    while(!*m_Stop) {
        //QTime t_check;
        //t_check.start();
        //for (int dd = 0; dd < 100000; ++dd) {
        SVPacket p = generatePacket(cnt);
        vector<unsigned char> tmp;
        p.getPacket(tmp);
        //}
        //qDebug() << "100000 packets generate test time = " << t_check.elapsed();
        //qDebug() << "Ethernet frame length = " << tmp.size();

        if (pcap_sendpacket(fp, tmp.data(), tmp.size()) == 0) {
            total_transmitted++;
            frameCounter++;
            controlFrameCounter++;
        } else {
            qDebug() << "Error transmitting! " << pcap_geterr(fp);
        }

        if (cnt == 0) {
            qDebug() << "Frame counter" << controlFrameCounter << "in " << controlTimer.elapsed() << "total_transmitted = " << total_transmitted;
            controlFrameCounter = 0;
            controlTimer.start();
        }

        if (frameCounter >= FRAME_CNT_UPDATE_TIMER) {
            updateDelayTime();
            frameCounter = 0;
            frameTimer.start();
        }

        uDelay(delayTime);
    }
    pcap_close(fp);
    emit setGeneratorIsRunning(false);

    qDebug() << "Total frames send" << total_transmitted;
}

void CGenerator::updateDelayTime() {
    const double elapsed = frameTimer.elapsed();

    const int desiredRate = genParams->getFramesPerPeriod() * 50 /*genParams.freq*/ / (genParams->getValsPerPacket());
    const double actualRate = 1000.0 * frameCounter / elapsed;
    const double delayAdd = 1000.0 / (double)desiredRate - 1000.0 / actualRate;

    if (delayAdd != DBL_MAX)
        delayTime += round(delayAdd * 1000.0);
    if (delayTime < 0)
        delayTime = 0;

    //qDebug() << "Delay time set to" << delayTime;
}

SVPacket CGenerator::generatePacket(int& counter) {
    SGenParams genParamsLocal(*genParams);
    //////////////////////////////////////////////////////////////////////////
    // Create savPDU TLV

    vector<unsigned char> tmp;

    //////////////////////////////////////////////////////////////////////////
    // noASDU
    // const unsigned char asdu_count = 0x08; // Number of groups in one packet - 1 for 80 points, 8 for 256

    const unsigned char asdu_count = genParamsLocal.getValsPerPacket();//(genParams.discrete == 80 ? 0x01 : 0x08); // Число групп в одном пакете - 1 для 80 точек, 8 для 256

    tmp.push_back(asdu_count);
    TLV noASDU(0x80, tmp);

    //////////////////////////////////////////////////////////////////////////
    // sequence of ASDU

    vector<unsigned char> sequenceASDU_data;
    for (int i = 0; i < asdu_count; i++) {
        //////////////////////////////////////////////////////////////////////////
        // svID		
        TLV svID(0x80, genParamsLocal.svId);

        //////////////////////////////////////////////////////////////////////////
        // smpCnt
        tmp.clear();
        tmp.push_back(static_cast<unsigned char>(counter >> 8));
        tmp.push_back(static_cast<unsigned char>(counter));
        TLV smpCnt(0x82, tmp);

        //////////////////////////////////////////////////////////////////////////
        //confRev - configuration version. Fixed = 1 for LE specification
        tmp.clear();
        char confRev_raw[4] = {0x00, 0x00, 0x00, 0x01};
        tmp.resize(4);
        memcpy(tmp.data(), &confRev_raw, 4);
        TLV confRev(0x83, tmp);

        //////////////////////////////////////////////////////////////////////////
        //smpSync - flag, identifying if there is external time sync
        tmp.clear();
        //if (counter == 1)
            tmp.push_back(0x01);
        //else
            //tmp.push_back( 0x00 );
        TLV smpSync(0x85, tmp);

        //////////////////////////////////////////////////////////////////////////
        // Sequence of data
        tmp.clear();
        const unsigned int k_max = 50;
        const double PI = 3.1415926535897932334;

        double pt_count = 50 * (genParamsLocal.getFramesPerPeriod()) / genParamsLocal.freq;
        double val_Ua;
        double val_Ub;
        double val_Uc;
        double val_Un;
        if (genParamsLocal.shape == SGL_SHAPE_SIN) {
            //val_Ua = genParams.Ua_A * 100.0 * qSin(genParams.Ua_G + (counter  - (int)(counter / pt_count) * pt_count) * (2 * 3.14) / pt_count);

            double a1 = 2*PI*genParamsLocal.freq/(genParamsLocal.getFramesPerPeriod()*50.0);
            double a2 = PI/180.0;
            double alpha = a1;
            double beta = genParamsLocal.Ua_G*a2;
            double Ucommon_a = 1.0*sin(alpha*counterForGeneration + beta);
            val_Ua = Ucommon_a * genParamsLocal.Ua_A * 100.0;

            double Ucommon_b = 1.0*sin(alpha*counterForGeneration + beta);
            val_Ub = Ucommon_b * genParamsLocal.Ub_A * 100.0;

            double Ucommon_c = 1.0*sin(alpha*counterForGeneration + beta);
            val_Uc = Ucommon_c * genParamsLocal.Uc_A * 100.0;

            double Ucommon_n = 1.0*sin(alpha*counterForGeneration + beta);
            val_Un = Ucommon_n * genParamsLocal.Un_A * 100.0;

            /*
            val_Ub = val_Ua;
            val_Uc = val_Ua;
            val_Un = val_Ua;
            */
            //val_Ub = Ucommon * genParams.Ub_A * 100;
            //val_Uc = Ucommon * genParams.Uc_A * 100;
            //val_Un = Ucommon * genParams.Un_A * 100;
        } else if (genParamsLocal.shape == SGL_SHAPE_SAW) {
            val_Ua = genParamsLocal.Ua_A * 100  * (counterForGeneration  - (int)(counterForGeneration / pt_count) * pt_count) / pt_count;

            val_Ub = val_Ua;
            val_Uc = val_Ua;
            val_Un = val_Ua;
        } else if (genParamsLocal.shape == SGL_SHAPE_TRIANGLE) {
            int cPos = counterForGeneration  - (int)(counterForGeneration / pt_count) * pt_count;
            if (cPos < pt_count * 0.25)
                val_Ua = genParamsLocal.Ua_A * 100  * cPos / (pt_count * 0.25);
            else if (cPos < pt_count * 0.5)
                val_Ua = genParamsLocal.Ua_A * 100  * 4 * (pt_count * 0.5 - cPos) / pt_count;
            else if (cPos < pt_count * 0.75)
                val_Ua = -1 * genParamsLocal.Ua_A * 100  * 4 * (cPos - pt_count * 0.5) / pt_count;
            else
                val_Ua = -1 * genParamsLocal.Ua_A * 100  * 4 * (pt_count - cPos) / pt_count;

            val_Ub = val_Ua;
            val_Uc = val_Ua;
            val_Un = val_Ua;
        } else if (genParamsLocal.shape == SGL_SHAPE_MEANDER) {
            val_Ua = (counterForGeneration  - (int)(counterForGeneration / pt_count) * pt_count) > pt_count / 2 ? genParamsLocal.Ua_A * 100 : 0.0;
            val_Ub = val_Ua;
            val_Uc = val_Ua;
            val_Un = val_Ua;
        } else if (genParamsLocal.shape == SGL_SHAPE_DEFINED) {
            val_Ua = val_Ub = val_Uc = val_Un = genParamsLocal.Ak.at(0);

            double Ucommon = 0.0;
            /* no optimization
            for (int k = 1; k < k_max + 1; k++) {
                double alpha = 2*PI*k*genParams.freq/(genParams.discrete*50.0);
                double beta = genParams.phik.at(k)*PI/180.0;
                Ucommon += genParams.Ak.at(k)*sin(alpha*n + beta);
            }
            */

            /*
             * with optimization
             */
            double UcommonA = 0.0;
            double UcommonB = 0.0;
            double UcommonC = 0.0;
            double UcommonN = 0.0;

            double a1 = 2*PI*genParamsLocal.freq/(genParamsLocal.getFramesPerPeriod()*50.0);
            double a2 = PI/180.0;

            {
                int k = 1;
                double alpha = k*a1;
                double beta = genParamsLocal.phik.at(k)*a2;

                UcommonA += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta + genParamsLocal.Ua_G*a2);
                UcommonB += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta + genParamsLocal.Ub_G*a2);
                UcommonC += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta + genParamsLocal.Uc_G*a2);
                UcommonN += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta + genParamsLocal.Un_G*a2);
            }

            for (int k = 2; k < k_max + 1; k++) {
                double alpha = k*a1;
                double beta = genParamsLocal.phik.at(k)*a2;

                UcommonA += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta);
                UcommonB += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta);
                UcommonC += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta);
                UcommonN += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta);
            }

            UcommonA += genParamsLocal.Ak[0]/sqrt(2.0);
            UcommonB += genParamsLocal.Ak[0]/sqrt(2.0);
            UcommonC += genParamsLocal.Ak[0]/sqrt(2.0);
            UcommonN += genParamsLocal.Ak[0]/sqrt(2.0);

            val_Ua = UcommonA * genParamsLocal.Ua_A * 100;
            val_Ub = UcommonB * genParamsLocal.Ub_A * 100;
            val_Uc = UcommonC * genParamsLocal.Uc_A * 100;
            val_Un = UcommonN * genParamsLocal.Un_A * 100;
        }

        double val_Ia;
        double val_Ib;
        double val_Ic;
        double val_In;
        //= genParams.Ia_A * 1000 * qSin(genParams.Ia_G + (counter  - (int)(counter / pt_count) * pt_count) * (2 * 3.14) / pt_count);
        if (genParamsLocal.shape == SGL_SHAPE_SIN) {
            /*val_Ia = genParams.Ia_A * 1000  * qSin(genParams.Ia_G + (counter  - (int)(counter / pt_count) * pt_count) * (2 * 3.14) / pt_count);
            val_Ib = val_Ia;
            val_Ic = val_Ia;
            val_In = val_Ia;
            */
            double a1 = 2*PI*genParamsLocal.freq/(genParamsLocal.getFramesPerPeriod()*50.0);
            double a2 = PI/180.0;
            double alpha = a1;
            double beta = genParamsLocal.Ia_G*a2;
            double Icommon_a = 1*sin(alpha*counterForGeneration + beta);
            val_Ia = Icommon_a * genParamsLocal.Ia_A * 1000;

            double Icommon_b = 1*sin(alpha*counterForGeneration + beta);
            val_Ib = Icommon_b * genParamsLocal.Ib_A * 1000;

            double Icommon_c = 1*sin(alpha*counterForGeneration + beta);
            val_Ic = Icommon_c * genParamsLocal.Ic_A * 1000;

            double Icommon_n = 1*sin(alpha*counterForGeneration + beta);
            val_In = Icommon_n * genParamsLocal.In_A * 1000;
        } else if (genParamsLocal.shape == SGL_SHAPE_SAW) {
            val_Ia = genParamsLocal.Ia_A * 1000  * (counterForGeneration  - (int)(counterForGeneration / pt_count) * pt_count) / pt_count;
            val_Ib = val_Ia;
            val_Ic = val_Ia;
            val_In = val_Ia;
        } else if (genParamsLocal.shape == SGL_SHAPE_TRIANGLE) {
            int cPos = counterForGeneration  - (int)(counterForGeneration / pt_count) * pt_count;
            if (cPos < pt_count * 0.25)
                val_Ia = genParamsLocal.Ia_A * 1000  * cPos / (pt_count * 0.25);
            else if (cPos < pt_count * 0.5)
                val_Ia = genParamsLocal.Ia_A * 1000  * 4 * (pt_count * 0.5 - cPos) / pt_count;
            else if (cPos < pt_count * 0.75)
                val_Ia = -1 * genParamsLocal.Ia_A * 1000  * 4 * (cPos - pt_count * 0.5) / pt_count;
            else
                val_Ia = -1 * genParamsLocal.Ia_A * 1000  * 4 * (pt_count - cPos) / pt_count;

            val_Ib = val_Ia;
            val_Ic = val_Ia;
            val_In = val_Ia;
        } else if (genParamsLocal.shape == SGL_SHAPE_MEANDER) {
            val_Ia = (counterForGeneration  - (int)(counterForGeneration / pt_count) * pt_count) > pt_count / 2 ? genParamsLocal.Ia_A * 1000 : 0.0;

            val_Ib = val_Ia;
            val_Ic = val_Ia;
            val_In = val_Ia;
        } else if (genParamsLocal.shape == SGL_SHAPE_DEFINED) {
            val_Ia = val_Ib = val_Ic = val_In = genParamsLocal.Ak.at(0);

            double Icommon = 0.0;
            /* no optimization
            for (int k = 1; k < k_max + 1; k++) {
                double alpha = 2*PI*k*genParams.freq/(genParams.discrete*50.0);
                double beta = genParams.phik.at(k)*PI/180.0;
                Icommon += genParams.Ak.at(k)*sin(alpha*n + beta);
            }
            */

            /*
             * with optimization
             */
            double IcommonA = 0.0;
            double IcommonB = 0.0;
            double IcommonC = 0.0;
            double IcommonN = 0.0;

            double a1 = 2*PI*genParamsLocal.freq/(genParamsLocal.getFramesPerPeriod()*50.0);
            double a2 = PI/180.0;

            {
                int k = 1;
                double alpha = k*a1;
                double beta = genParamsLocal.phik.at(k)*a2;

                IcommonA += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta + genParamsLocal.Ia_G*a2);
                IcommonB += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta + genParamsLocal.Ib_G*a2);
                IcommonC += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta + genParamsLocal.Ic_G*a2);
                IcommonN += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta + genParamsLocal.In_G*a2);
            }

            for (int k = 2; k < k_max + 1; k++) {
                double alpha = k*a1;
                double beta = genParamsLocal.phik.at(k)*a2;

                IcommonA += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta);
                IcommonB += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta);
                IcommonC += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta);
                IcommonN += genParamsLocal.Ak.at(k)*sin(alpha*counterForGeneration + beta);
            }

            IcommonA += genParamsLocal.Ak[0]/sqrt(2.0);
            IcommonB += genParamsLocal.Ak[0]/sqrt(2.0);
            IcommonC += genParamsLocal.Ak[0]/sqrt(2.0);
            IcommonN += genParamsLocal.Ak[0]/sqrt(2.0);

            val_Ia = IcommonA * genParamsLocal.Ia_A * 1000;
            val_Ib = IcommonB * genParamsLocal.Ib_A * 1000;
            val_Ic = IcommonC * genParamsLocal.Ic_A * 1000;
            val_In = IcommonN * genParamsLocal.In_A * 1000;
        }


        int iValUa = round(val_Ua);//static_cast<int>(val_Ua);
        int iValUb = round(val_Ub);//static_cast<int>(val_Ub);
        int iValUc = round(val_Uc);//static_cast<int>(val_Uc);
        int iValUn = round(val_Un);//static_cast<int>(val_Un);

        int iValIa = round(val_Ia);//static_cast<int>(val_Ia);
        int iValIb = round(val_Ib);//static_cast<int>(val_Ib);
        int iValIc = round(val_Ic);//static_cast<int>(val_Ic);
        int iValIn = round(val_In);//static_cast<int>(val_In);
        const int zero = 0;

        // Ia
        int rs = tmp.size();
        tmp.resize(rs + 8);
        rmemcpy(tmp.data() + rs, (const unsigned char * const)&iValIa, 4);
        memcpy(tmp.data() + rs + 4, &zero, 4);

        // Ib
        rs = tmp.size();
        tmp.resize(rs + 8);
        rmemcpy(tmp.data() + rs, (const unsigned char * const)&iValIb, 4);
        memcpy(tmp.data() + rs + 4, &zero, 4);

        // Ic
        rs = tmp.size();
        tmp.resize( rs + 8 );
        rmemcpy(tmp.data() + rs, (const unsigned char * const)&iValIc, 4);
        memcpy(tmp.data() + rs + 4, &zero, 4);

        // In
        rs = tmp.size();
        tmp.resize( rs + 8 );
        rmemcpy(tmp.data() + rs, (const unsigned char * const)&iValIn, 4);
        memcpy(tmp.data() + rs + 4, &zero, 4);

        // Ua
        rs = tmp.size();
        tmp.resize( rs + 8 );
        rmemcpy(tmp.data() + rs, (const unsigned char * const)&iValUa, 4);
        memcpy(tmp.data() + rs + 4, &zero, 4);

        // Ub
        rs = tmp.size();
        tmp.resize( rs + 8 );
        rmemcpy(tmp.data() + rs, (const unsigned char * const)&iValUb, 4);
        memcpy(tmp.data() + rs + 4, &zero, 4);

        // Uc
        rs = tmp.size();
        tmp.resize( rs + 8 );
        rmemcpy(tmp.data() + rs, (const unsigned char * const)&iValUc, 4);
        memcpy(tmp.data() + rs + 4, &zero, 4);

        // Un
        rs = tmp.size();
        tmp.resize( rs + 8 );
        rmemcpy(tmp.data() + rs, (const unsigned char * const)&iValUn, 4);
        memcpy(tmp.data() + rs + 4, &zero, 4);

        TLV dataSet(0x87, tmp);

        vector<unsigned char> asdu_data_total;
        svID.getPacket(tmp);
        rs = asdu_data_total.size();
        asdu_data_total.resize(rs + tmp.size());
        memcpy(asdu_data_total.data() + rs, tmp.data(), tmp.size());

        smpCnt.getPacket(tmp);
        rs = asdu_data_total.size();
        asdu_data_total.resize(rs + tmp.size());
        memcpy(asdu_data_total.data() + rs, tmp.data(), tmp.size());
        
        confRev.getPacket(tmp);
        rs = asdu_data_total.size();
        asdu_data_total.resize(rs + tmp.size());
        memcpy(asdu_data_total.data() + rs, tmp.data(), tmp.size());
        
        smpSync.getPacket(tmp);
        rs = asdu_data_total.size();
        asdu_data_total.resize(rs + tmp.size());
        memcpy(asdu_data_total.data() + rs, tmp.data(), tmp.size());
        
        dataSet.getPacket(tmp);
        rs = asdu_data_total.size();
        asdu_data_total.resize(rs + tmp.size());
        memcpy(asdu_data_total.data() + rs, tmp.data(), tmp.size());

        TLV ASDU(0x30, asdu_data_total);
        ASDU.getPacket(tmp);
        rs = sequenceASDU_data.size();
        sequenceASDU_data.resize(rs + tmp.size());
        memcpy(sequenceASDU_data.data() + rs, tmp.data(), tmp.size());

        ++counter;
        ++counterForGeneration;
        if (counter >= genParamsLocal.getFramesPerPeriod() * 50) // на промышленной частоте
            counter = 0;
    }

    TLV sequenceASDU(0xA2, sequenceASDU_data);
    
    vector<unsigned char> savPDU_tmp;
    sequenceASDU.getPacket(savPDU_tmp);
    noASDU.getPacket(tmp);
    int rs = tmp.size();
    tmp.resize(rs + savPDU_tmp.size());
    memcpy(tmp.data() + rs, savPDU_tmp.data(), savPDU_tmp.size());

    vector<unsigned char>& savPDU_data = tmp;
    
    TLV savPDU(0x60, savPDU_data);

    // APPID set as default
    unsigned char APPID[2];
    APPID[0] = 0x40;
    APPID[1] = 0x00;

    SVPacket p(header, APPID, savPDU);
    return p;
}
