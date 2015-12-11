#ifndef GENPARAMS_H
#define GENPARAMS_H
#include <vector>
#include "settings.h"
using namespace std;

#include <QMutex>

class SGenParams {
public:
    SGenParams();
	~SGenParams();
	SGenParams(const SGenParams& src);
	SGenParams& operator=(const SGenParams& src);
	SGenParams(const CSettings& settings);
    void setDstMac(const QString& mac);
    void setSrcMac(const QString& mac);
    void setSvId(const QString& id);

	double Ua_A;
	double Ub_A;
	double Uc_A;
	double Un_A;
	double Ua_G;
	double Ub_G;
	double Uc_G;
	double Un_G;

	double Ia_A;
	double Ib_A;
	double Ic_A;
	double In_A;
	double Ia_G;
	double Ib_G;
	double Ic_G;
	double In_G;

	double freq;

	vector<double> Ak;
	vector<double> phik;

	DISCRETE discrete; // 80 || 256
	SGL_SHAPE shape;

	unsigned char dstMac[6];
	unsigned char srcMac[6];
	vector<unsigned char> svId;

private:
	QMutex *mutex;

	void init(const SGenParams& src);

public:
	unsigned int getFramesPerPeriod() const;
	unsigned int getValsPerPacket() const;

private:
    void loadShape();
public:
	static void saveShape(const vector<double>& Ak, const vector<double>& phik);
};

#endif
