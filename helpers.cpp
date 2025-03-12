#include "helpers.h"

#define WPCAP
#define HAVE_REMOTE
#include "pcap.h"
#undef min

#define VERSION "1.5.0a";

QString getAppVersion() {
    return VERSION;
}

QString doubleToStr(const double val, const char digits) {
    QString tmp;
    tmp = QString("%1").arg(val, 0, 'f', digits, '0');
    if ( digits == -1 )
        tmp = tmp.trimmed();

    return tmp;
}

void rmemcpy(unsigned char * dst, const unsigned char * const src, const unsigned int count) {
    for (unsigned int i = 0; i < count; ++i)
        dst[count - 1 - i] = src[i];
}

void uDelay(unsigned int val) {
    if (val > 1000 * 1000 * 1000)
        return;
    //// This does not work under MSVC2013 - sleeping too long
    //std::this_thread::sleep_for(std::chrono::microseconds(val));

    #ifdef WIN32
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);
        double PCFreq = double(li.QuadPart)/1000000.0;
        QueryPerformanceCounter(&li);
        __int64 CounterStart = li.QuadPart;
        while (true) {
            QueryPerformanceCounter(&li);
            double sleep = double(li.QuadPart-CounterStart)/PCFreq;
            if (sleep > val)
                break;
        }
    #else
        timespec a;
        clock_gettime(CLOCK_REALTIME, &a);
        ldiv_t t = ldiv(a.tv_nsec + val*1000, 1000000000l);
        timespec ts;
        ts.tv_sec = a.tv_sec + t.quot;
        ts.tv_nsec = t.rem;
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, NULL);
    #endif

}

QString AddrToString(sockaddr* sa)
{
    char s[INET6_ADDRSTRLEN];
    switch (sa->sa_family) {
    case AF_INET:
        inet_ntop(AF_INET, &(((struct sockaddr_in*)sa)->sin_addr), s, INET6_ADDRSTRLEN);
        break;

    case AF_INET6:
        inet_ntop(AF_INET6, &(((struct sockaddr_in6*)sa)->sin6_addr), s, INET6_ADDRSTRLEN);
        break;

    default:
        strncpy(s, "Unknown AF", INET6_ADDRSTRLEN);
        return NULL;
    }

    return QString::fromLocal8Bit(s);
}
