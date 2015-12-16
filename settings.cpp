#include "settings.h"

const QString iniName = "settings.ini";

CSettings::CSettings() {
    signalShape = SGL_SHAPE_SIN;
    signalAmplitudeUa =
            signalAmplitudeUb =
            signalAmplitudeUc =
            signalAmplitudeUn =
            signalAmplitudeIa =
            signalAmplitudeIb =
            signalAmplitudeIc =
            signalAmplitudeIn = 100.0;

    signalFrequency = 50.0;
    signalPhaseUa = 
        signalPhaseUb = 
        signalPhaseUc = 
        signalPhaseUn = 
        signalPhaseIa = 
        signalPhaseIb = 
        signalPhaseIc = 
        signalPhaseIn = 0.0;

    destinationMac = "FF:FF:FF:FF:FF:FF";
    genStreamId = "sampleStream";
    signalDiscrete = DISCRETE_80;

    captureMac = "FF:FF:FF:FF:FF:FF";
    captureStreamId = "sampleStream";

    langTranslation = LANG_TRANSLATE_RU;
}

void CSettings::load() {
    QSettings s(iniName, QSettings::IniFormat);
    s.beginGroup(getIniElementName(IF_GROUP_GENERATOR));
    signalShape = static_cast<SGL_SHAPE>(s.value(getIniElementName(IF_SIGNAL_SHAPE)).toUInt());
    signalAmplitudeUa = s.value(getIniElementName(IF_SIGNAL_AMPLITUDE_UA)).toDouble();
    signalAmplitudeUb = s.value(getIniElementName(IF_SIGNAL_AMPLITUDE_UB)).toDouble();
    signalAmplitudeUc = s.value(getIniElementName(IF_SIGNAL_AMPLITUDE_UC)).toDouble();
    signalAmplitudeUn = s.value(getIniElementName(IF_SIGNAL_AMPLITUDE_UN)).toDouble();
    signalAmplitudeIa = s.value(getIniElementName(IF_SIGNAL_AMPLITUDE_IA)).toDouble();
    signalAmplitudeIb = s.value(getIniElementName(IF_SIGNAL_AMPLITUDE_IB)).toDouble();
    signalAmplitudeIc = s.value(getIniElementName(IF_SIGNAL_AMPLITUDE_IC)).toDouble();
    signalAmplitudeIn = s.value(getIniElementName(IF_SIGNAL_AMPLITUDE_IN)).toDouble();
    signalFrequency = s.value(getIniElementName(IF_SIGNAL_FREQUENCY)).toDouble();
    signalPhaseUa = s.value(getIniElementName(IF_SIGNAL_PHASE_UA)).toDouble();
    signalPhaseUb = s.value(getIniElementName(IF_SIGNAL_PHASE_UB)).toDouble();
    signalPhaseUc = s.value(getIniElementName(IF_SIGNAL_PHASE_UC)).toDouble();
    signalPhaseUn = s.value(getIniElementName(IF_SIGNAL_PHASE_UN)).toDouble();
    signalPhaseIa = s.value(getIniElementName(IF_SIGNAL_PHASE_IA)).toDouble();
    signalPhaseIb = s.value(getIniElementName(IF_SIGNAL_PHASE_IB)).toDouble();
    signalPhaseIc = s.value(getIniElementName(IF_SIGNAL_PHASE_IC)).toDouble();
    signalPhaseIn = s.value(getIniElementName(IF_SIGNAL_PHASE_IN)).toDouble();
    destinationMac = s.value(getIniElementName(IF_DESTINATION_MAC)).toString();
    genStreamId = s.value(getIniElementName(IF_GEN_STREAM_ID)).toString();
    signalDiscrete = static_cast<DISCRETE>(s.value(getIniElementName(IF_SIGNAL_DISCRETE)).toUInt());
    genMac = s.value(getIniElementName(IF_GEN_MAC)).toString();
    captureStreamId = s.value(getIniElementName(IF_CAPTURE_STREAM_ID)).toString();
    captureMac = s.value(getIniElementName(IF_CAPTURE_MAC)).toString();
    //signal1stHarmonicA = s.value(getIniElementName(IF_SIGNAL_1ST_HARMONIC_A)).toDouble();
    //signal1stHarmonicPhi = s.value(getIniElementName(IF_SIGNAL_1ST_HARMONIC_PHI)).toDouble();

    macFront = s.value(getIniElementName(IF_MAC_FRONT)).toString();
    frontName = s.value(getIniElementName(IF_MAC_FRONT_NAME)).toString();
    macRear = s.value(getIniElementName(IF_MAC_REAR)).toString();
    rearName = s.value(getIniElementName(IF_MAC_REAR_NAME)).toString();

    langTranslation =  static_cast<LANG_TRANSLATE>(s.value(getIniElementName(IF_LANG_TRANSLATION)).toUInt());

    s.endGroup();
}
void CSettings::save() {
    QSettings s(iniName, QSettings::IniFormat);
    s.beginGroup(getIniElementName(IF_GROUP_GENERATOR));
    s.setValue(getIniElementName(IF_SIGNAL_SHAPE), signalShape);
    s.setValue(getIniElementName(IF_SIGNAL_AMPLITUDE_UA), signalAmplitudeUa);
    s.setValue(getIniElementName(IF_SIGNAL_AMPLITUDE_UB), signalAmplitudeUb);
    s.setValue(getIniElementName(IF_SIGNAL_AMPLITUDE_UC), signalAmplitudeUc);
    s.setValue(getIniElementName(IF_SIGNAL_AMPLITUDE_UN), signalAmplitudeUn);
    s.setValue(getIniElementName(IF_SIGNAL_AMPLITUDE_IA), signalAmplitudeIa);
    s.setValue(getIniElementName(IF_SIGNAL_AMPLITUDE_IB), signalAmplitudeIb);
    s.setValue(getIniElementName(IF_SIGNAL_AMPLITUDE_IC), signalAmplitudeIc);
    s.setValue(getIniElementName(IF_SIGNAL_AMPLITUDE_IN), signalAmplitudeIn);
    s.setValue(getIniElementName(IF_SIGNAL_FREQUENCY), signalFrequency);
    s.setValue(getIniElementName(IF_SIGNAL_PHASE_UA), signalPhaseUa);
    s.setValue(getIniElementName(IF_SIGNAL_PHASE_UB), signalPhaseUb);
    s.setValue(getIniElementName(IF_SIGNAL_PHASE_UC), signalPhaseUc);
    s.setValue(getIniElementName(IF_SIGNAL_PHASE_UN), signalPhaseUn);
    s.setValue(getIniElementName(IF_SIGNAL_PHASE_IA), signalPhaseIa);
    s.setValue(getIniElementName(IF_SIGNAL_PHASE_IB), signalPhaseIb);
    s.setValue(getIniElementName(IF_SIGNAL_PHASE_IC), signalPhaseIc);
    s.setValue(getIniElementName(IF_SIGNAL_PHASE_IN), signalPhaseIn);
    s.setValue(getIniElementName(IF_DESTINATION_MAC), destinationMac);
    s.setValue(getIniElementName(IF_GEN_STREAM_ID), genStreamId);
    s.setValue(getIniElementName(IF_SIGNAL_DISCRETE), signalDiscrete);
    s.setValue(getIniElementName(IF_GEN_MAC), genMac);
    s.setValue(getIniElementName(IF_CAPTURE_STREAM_ID), captureStreamId);
    s.setValue(getIniElementName(IF_CAPTURE_MAC), captureMac);
    //s.setValue(getIniElementName(IF_SIGNAL_1ST_HARMONIC_A), signal1stHarmonicA);
    //s.setValue(getIniElementName(IF_SIGNAL_1ST_HARMONIC_PHI), signal1stHarmonicPhi);

    // Эти два не нужно сохранять из UI, они забиваются на заводе
    //s.setValue(getIniElementName(IF_MAC_FRONT), macFront);
    //s.setValue(getIniElementName(IF_MAC_REAR), macRear);
    s.endGroup();
}
const QString CSettings::getIniElementName(const INI_ELEMENT element) {
    QString res;
    switch(element) {
    case IF_GROUP_GENERATOR:
        res = "generator";
        break;
    case IF_SIGNAL_SHAPE:
        res = "signalShape";
        break;
    case IF_SIGNAL_AMPLITUDE_UA:
        res = "signalAmplitudeUa";
        break;
    case IF_SIGNAL_AMPLITUDE_UB:
        res = "signalAmplitudeUb";
        break;
    case IF_SIGNAL_AMPLITUDE_UC:
        res = "signalAmplitudeUc";
        break;
    case IF_SIGNAL_AMPLITUDE_UN:
        res = "signalAmplitudeUn";
        break;
    case IF_SIGNAL_AMPLITUDE_IA:
        res = "signalAmplitudeIa";
        break;
    case IF_SIGNAL_AMPLITUDE_IB:
        res = "signalAmplitudeIb";
        break;
    case IF_SIGNAL_AMPLITUDE_IC:
        res = "signalAmplitudeIc";
        break;
    case IF_SIGNAL_AMPLITUDE_IN:
        res = "signalAmplitudeIn";
        break;
    case IF_SIGNAL_FREQUENCY:
        res = "signalFrequency";
        break;
    case IF_SIGNAL_PHASE_UA:
        res = "signalPhaseUa";
        break;
    case IF_SIGNAL_PHASE_UB:
        res = "signalPhaseUb";
        break;
    case IF_SIGNAL_PHASE_UC:
        res = "signalPhaseUc";
        break;
    case IF_SIGNAL_PHASE_UN:
        res = "signalPhaseUn";
        break;
    case IF_SIGNAL_PHASE_IA:
        res = "signalPhaseIa";
        break;
    case IF_SIGNAL_PHASE_IB:
        res = "signalPhaseIb";
        break;
    case IF_SIGNAL_PHASE_IC:
        res = "signalPhaseIc";
        break;
    case IF_SIGNAL_PHASE_IN:
        res = "signalPhaseIn";
        break;
    case IF_DESTINATION_MAC:
        res = "destinationMac";
        break;
    case IF_GEN_STREAM_ID:
        res = "genStreamId";
        break;
    case IF_SIGNAL_DISCRETE:
        res = "signalDiscrete";
        break;
    case IF_GEN_MAC:
        res = "genMac";
        break;
    case IF_CAPTURE_STREAM_ID:
        res = "captureStreamId";
        break;
    case IF_CAPTURE_MAC:
        res = "captureMac";
        break;
    /*
    case IF_SIGNAL_1ST_HARMONIC_A:
        res = "signal1stHarmonicA";
        break;
    case IF_SIGNAL_1ST_HARMONIC_PHI:
        res = "signal1stHarmonicPhi";
        break;
    */
    case IF_MAC_FRONT:
        res = "macFront";
        break;
    case IF_MAC_REAR:
        res = "macRear";
        break;
    case IF_MAC_FRONT_NAME:
        res = "frontName";
        break;
    case IF_MAC_REAR_NAME:
        res = "rearName";
        break;
    case IF_LANG_TRANSLATION:
        res = "langTranslation";
        break;
    default:
        throw("");
        break;
    }

    return res;
}
