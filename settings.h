#ifndef SETTINGS_H
#define SETTINGS_H
#include <QSettings>

// Signal parameters
enum SGL_SHAPE {
    SGL_SHAPE_SIN       = 0,
    SGL_SHAPE_TRIANGLE  = 1,
    SGL_SHAPE_MEANDER   = 2,
    SGL_SHAPE_SAW       = 3,
    SGL_SHAPE_DEFINED   = 4
};

enum DISCRETE {
	DISCRETE_80 = 0,
	DISCRETE_256,
	DISCRETE_640,
	DISCRETE_1280
};

enum LANG_TRANSLATE {
	LANG_TRANSLATE_RU = 0,
	LANG_TRANSLATE_EN
};

class CSettings : public QSettings
{
public:
    CSettings();
    void load();
    void save();

    enum INI_ELEMENT {
        IF_GROUP_GENERATOR = 0,
        IF_SIGNAL_SHAPE,
        IF_SIGNAL_AMPLITUDE_UA,
        IF_SIGNAL_AMPLITUDE_UB,
        IF_SIGNAL_AMPLITUDE_UC,
        IF_SIGNAL_AMPLITUDE_UN,
        IF_SIGNAL_AMPLITUDE_IA,
        IF_SIGNAL_AMPLITUDE_IB,
        IF_SIGNAL_AMPLITUDE_IC,
        IF_SIGNAL_AMPLITUDE_IN,
        IF_SIGNAL_FREQUENCY,
        IF_SIGNAL_PHASE_UA,
		IF_SIGNAL_PHASE_UB,
		IF_SIGNAL_PHASE_UC,
		IF_SIGNAL_PHASE_UN,
        IF_SIGNAL_PHASE_IA,
		IF_SIGNAL_PHASE_IB,
		IF_SIGNAL_PHASE_IC,
		IF_SIGNAL_PHASE_IN,
        IF_DESTINATION_MAC,
        IF_GEN_STREAM_ID,
        IF_SIGNAL_DISCRETE,
        IF_GEN_MAC,
        IF_CAPTURE_STREAM_ID,
        IF_CAPTURE_MAC,
		/*
        IF_SIGNAL_1ST_HARMONIC_A,
        IF_SIGNAL_1ST_HARMONIC_PHI
		*/
		IF_MAC_FRONT,
		IF_MAC_FRONT_NAME,
		IF_MAC_REAR,
		IF_MAC_REAR_NAME,
		IF_LANG_TRANSLATION
	};
	
    SGL_SHAPE signalShape;
    double signalAmplitudeUa;
    double signalAmplitudeUb;
    double signalAmplitudeUc;
    double signalAmplitudeUn;
    double signalAmplitudeIa;
    double signalAmplitudeIb;
    double signalAmplitudeIc;
    double signalAmplitudeIn;
    double signalFrequency;
    double signalPhaseUa;
	double signalPhaseUb;
	double signalPhaseUc;
	double signalPhaseUn;
    double signalPhaseIa;
	double signalPhaseIb;
	double signalPhaseIc;
	double signalPhaseIn;
	DISCRETE signalDiscrete;
    //double signal1stHarmonicA;
    //double signal1stHarmonicPhi;

    // 61850 parameters - for generator
    QString destinationMac;
    QString genStreamId;
    QString genMac;

    QString captureStreamId;
    QString captureMac;

	QString macFront;
	QString frontName;
	QString macRear;
	QString rearName;

	LANG_TRANSLATE langTranslation;

private:
    const QString getIniElementName(const INI_ELEMENT element);
};

#endif // SETTINGS_H
