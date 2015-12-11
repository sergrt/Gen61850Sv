#include "genParams.h"
#include <QFile>
#include <QStringList>
#include <cmath>
const double tinyDouble = 0.00001;
const QString shapeFileName = "_shape.sgl";
const int fileFieldW = 12;

SGenParams::SGenParams() {
	mutex = new QMutex();

    Ua_A = Ub_A = Uc_A = Un_A = 0.0;
    Ua_G = Ub_G = Uc_G = Un_G = 0.0;
    Ia_A = Ib_A = Ic_A = In_A = 0.0;
    Ia_G = Ib_G = Ic_G = In_G = 0.0;

	discrete = DISCRETE_80;
	
    dstMac[0] = 0x00;
    dstMac[1] = 0x25;
    dstMac[2] = 0x22;
    dstMac[3] = 0xEB;
    dstMac[4] = 0xC5;
    dstMac[5] = 0x51;
	
    srcMac[0] = 0x00;
    srcMac[1] = 0x25;
    srcMac[2] = 0x22;
    srcMac[3] = 0xEB;
    srcMac[4] = 0xC5;
    srcMac[5] = 0x51;
	
    svId.push_back('F');
    svId.push_back('C');
    svId.push_back('V');
    svId.push_back('S');
    svId.push_back('M');
    svId.push_back('U');
    svId.push_back('0');
    svId.push_back('3');
    svId.push_back('0');
    svId.push_back('1');

    shape = SGL_SHAPE_SIN;

    loadShape();
}
SGenParams::~SGenParams() {
	delete mutex;
}

SGenParams::SGenParams(const SGenParams& src) {
	init(src);
}

SGenParams& SGenParams::operator=(const SGenParams& src) {
	if (this != &src) {
		init(src);
	}

	return *this;
}

void SGenParams::init(const SGenParams& src) {
	QMutexLocker(src.mutex);

	mutex = new QMutex();

	Ua_A = src.Ua_A;
	Ub_A = src.Ub_A;
	Uc_A = src.Uc_A;
	Un_A = src.Un_A;
	Ua_G = src.Ua_G;
	Ub_G = src.Ub_G;
	Uc_G = src.Uc_G;
	Un_G = src.Un_G;
	Ia_A = src.Ia_A;
	Ib_A = src.Ib_A;
	Ic_A = src.Ic_A;
	In_A = src.In_A;
	Ia_G = src.Ia_G;
	Ib_G = src.Ib_G;
	Ic_G = src.Ic_G;
	In_G = src.In_G;

	freq = src.freq;
	Ak = src.Ak;
	phik = src.phik;

	discrete = src.discrete;
	shape = src.shape;
	for (int i = 0; i < 6; ++i) {
		dstMac[i] = src.dstMac[i];
		srcMac[i] = src.srcMac[i];
	}
	svId = src.svId;
}

SGenParams::SGenParams(const CSettings &settings) {
	mutex = new QMutex();
    Ua_A = settings.signalAmplitudeUa;
    Ua_G = settings.signalPhaseUa;
    Ub_A = settings.signalAmplitudeUb;
    Ub_G = settings.signalPhaseUb;
    Uc_A = settings.signalAmplitudeUc;
    Uc_G = settings.signalPhaseUc;
    Un_A = settings.signalAmplitudeUn;
    Un_G = settings.signalPhaseUn;

    Ia_A = settings.signalAmplitudeIa;
    Ia_G = settings.signalPhaseIa;
    Ib_A = settings.signalAmplitudeIb;
    Ib_G = settings.signalPhaseIb;
    Ic_A = settings.signalAmplitudeIc;
    Ic_G = settings.signalPhaseIc;
    In_A = settings.signalAmplitudeIn;
    In_G = settings.signalPhaseIn;

    freq = settings.signalFrequency;

    setDstMac(settings.destinationMac);
    setSrcMac(settings.genMac);
    setSvId(settings.genStreamId);

	discrete = static_cast<DISCRETE>(settings.signalDiscrete);

    shape = settings.signalShape;

    loadShape();
	/*
	if (fabs(settings.signal1stHarmonicA) > tinyDouble && Ak.size() > 1)
        Ak[1] = settings.signal1stHarmonicA;

	if (fabs(settings.signal1stHarmonicPhi) > tinyDouble && phik.size() > 1)
        phik[1] = settings.signal1stHarmonicPhi;
	*/
}
void SGenParams::loadShape() {
	QFile f(shapeFileName);
	if (f.open(QIODevice::ReadOnly)) {
		f.readLine(); // read header
		QByteArray array = f.readLine();
		while(array.count() > 0) {
			QString str(array);
			QStringList sl = str.split('|');
			if (sl.count() < 3)
				throw("Error parsing shape file");
			Ak.push_back(sl.at(1).toDouble());
			phik.push_back(sl.at(2).toDouble());

			array = f.readLine();
		}

		f.close();
	}
}
void SGenParams::saveShape(const vector<double>& Ak, const vector<double>& phik) {
	QFile f(shapeFileName);
	if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		const QString header = QString("%1|%2|%3\r\n").arg("k", fileFieldW, QChar(' ')).
			arg("Ak", fileFieldW, QChar(' ')).
			arg("phik", fileFieldW, QChar(' '));

		f.write(header.toLocal8Bit()); // write header

		for (int i = 0; i < Ak.size(); ++i) {
			QString line = QString("%1|%2|%3\r\n").arg(i, fileFieldW, 10, QChar(' ')).
				arg(Ak[i], fileFieldW, 'f', -1, QChar(' ')).
				arg(phik[i], fileFieldW, 'f', -1, QChar(' '));

			f.write(line.toLocal8Bit());
		}

		f.close();
	}
}
void SGenParams::setDstMac(const QString& mac) {
    for (int i = 0; i < 6; i++)
        dstMac[i] = mac.mid(i*3,2).toInt(0,16);
}

void SGenParams::setSrcMac(const QString& mac) {
    for (int i = 0; i < 6; i++)
        srcMac[i] = mac.mid(i*3,2).toInt(0,16);
}

void SGenParams::setSvId(const QString& id) {
    svId.clear();
    for (int i = 0; i < id.length(); i++)
        svId.push_back(id.at(i).toLatin1());
}
unsigned int SGenParams::getFramesPerPeriod() const {
	unsigned int res;
	switch(discrete) {
	case DISCRETE_80:
		res = 80;
		break;
	case DISCRETE_256:
		res = 256;
		break;
	case DISCRETE_640:
		res = 640;
		break;
	case DISCRETE_1280:
		res = 1280;
		break;
	default:
		res = 80;
		break;
	}
	return res;
}

unsigned int SGenParams::getValsPerPacket() const {
	unsigned int res;
	switch(discrete) {
	case DISCRETE_80:
		res = 1;
		break;
	case DISCRETE_256:
		res = 8;
		break;
	case DISCRETE_640:
		res = 16;
		break;
	case DISCRETE_1280:
		res = 16;//32; чтобы не превышать размер пакета
		break;
	default:
		res = 1;
		break;
	}
	return res;
}