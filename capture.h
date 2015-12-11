// Capture IEC61850-9.2LE stream thread

#pragma once
#include <QThread>
#include "SVDecoded.h"

#define WPCAP
#define HAVE_REMOTE
#include "pcap.h"
#undef min

class CCapture : public QThread {
    Q_OBJECT

public:
    CCapture(QObject* parent, bool* stop);
    ~CCapture(void);
    void init(const QString& streamName, const QString& mac);
    void run();
private:
    bool* stopThread;
    QString streamFilter;
    QString captureMac;
    string streamName;
    pcap_t* pcapHandle;
    void decode(const vector<unsigned char>& raw_data, SVDecoded& res);
    void decode_light(vector<unsigned char>& data, SVDecoded& res);  // Faster, but not so self-explaining
    vector<SVDecoded> dataMeasured;
signals:
    void setCaptureIsRunning(bool);
    void displayData(vector<SVDecoded>*,bool* ready);
};
