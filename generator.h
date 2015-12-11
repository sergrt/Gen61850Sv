#pragma once
#include <QThread>
#include "genParams.h"
#include "SVPacket.h"
//#include "SyncObject.h"
#include <QTime>


class CGenerator : public QThread {
	Q_OBJECT

public:
    CGenerator(QObject *parent, bool *stop, SGenParams *params);
	~CGenerator();
	void run();

private:
	bool *m_Stop;
	SVPacket generatePacket(int& counter);
	SGenParams *genParams;

	EthernetHeader header;

	QTime frameTimer;
	int delayTime;
	void updateDelayTime();
    //void uDelay(unsigned int val);
	unsigned int frameCounter; // for output freq control
	unsigned int counterForGeneration;
signals:
    void setGeneratorIsRunning(bool);
};

