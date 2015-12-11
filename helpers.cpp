#include "helpers.h"
#ifndef WIN32
#include <time.h>
#endif

//#ifdef _MSC_VER
#define VERSION "1.5.0a";
//#endif

QString getAppVersion() {
    return VERSION;
}

QString doubleToStr(const double val, const char digits) {
    QString tmp;
    if ( digits != -1 ) {
        tmp = QString( "%1" ).arg( val, 0, 'f', digits, '0' );
    } else {
        tmp = QString( "%1" ).arg( val, 0, 'f', -1, '0' );
        tmp = tmp.trimmed();
    }

    return tmp;
}

void rmemcpy(unsigned char * dst, const unsigned char * const src, const unsigned int count) {
    for (unsigned int i = 0; i < count; ++i)
        dst[count - 1 - i] = src[i];
}

#define WPCAP
#define HAVE_REMOTE
#include "pcap.h"
#undef min
/*
void uDelay(unsigned int val) {
    LARGE_INTEGER freq = {0};
    LARGE_INTEGER iStart, iStop;
    if (freq.QuadPart == 0)
        QueryPerformanceFrequency(&freq), freq.QuadPart /= 1000;

    QueryPerformanceCounter(&iStart);
    while (true) {
        QueryPerformanceCounter(&iStop);
        double sleep = ((double)iStop.QuadPart - (double)iStart.QuadPart);

        if ( sleep > val )
            break;
    }
}
*/
void uDelay(unsigned int val) {
    if (val > 1000 * 1000 * 1000)
        return;

    //qDebug() << "uDelay start time " << QDateTime::currentDateTimeUtc().toString("hh:mm:ss.zzz") << " wanted delay " << val;
    #ifdef WIN32

    double PCFreq = 0.0;
    __int64 CounterStart = 0;

    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);
    PCFreq = double(li.QuadPart)/1000000.0;
    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
    while (true) {
        QueryPerformanceCounter(&li);
        double sleep = double(li.QuadPart-CounterStart)/PCFreq;
        if (sleep > val)
            break;
    }
    /*
        LARGE_INTEGER freq = {0};
        LARGE_INTEGER iStart, iStop;
        if (freq.QuadPart == 0)
            QueryPerformanceFrequency(&freq), freq.QuadPart /= 1000;

        QueryPerformanceCounter(&iStart);
        while (true) {
            QueryPerformanceCounter(&iStop);
            double sleep = ((double)iStop.QuadPart - (double)iStart.QuadPart);

            if (sleep > val)
                break;
        }
        */
    #else
        //QTest::qWait(val/1000);
        /*
        sched_param sch;
        sch.sched_priority = sched_get_priority_max(SCHED_FIFO);
        sched_setscheduler(0, SCHED_FIFO, &sch);
        */
        timespec a;
        clock_gettime(CLOCK_REALTIME, &a);
        ldiv_t t = ldiv(a.tv_nsec + val*1000, 1000000000l);
        timespec ts;
        ts.tv_sec = a.tv_sec + t.quot;
        ts.tv_nsec = t.rem;
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, NULL);
    #endif
    //qDebug() << "uDelay end time " << QDateTime::currentDateTimeUtc().toString("hh:mm:ss.zzz");
}
#ifdef WIN32
/*
int round(double r) {
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}
*/
#endif
