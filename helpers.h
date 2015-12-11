#ifndef HELPERS_H
#define HELPERS_H
#include <QString>

QString getAppVersion();
QString doubleToStr(const double val, const char digits = -1);
void rmemcpy(unsigned char * dst, const unsigned char * const src, const unsigned int count);
void uDelay(unsigned int val);
#ifdef WIN32
//int round(double r);
#endif
#endif
