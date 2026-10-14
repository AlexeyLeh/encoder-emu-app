/**
  \file   dbg.cpp
  \author Aleksey Lekhtin
  \date   Nov  7, 2025
  \brief  debug features
*/


#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <QDateTime>

#include "dbg.h"

#ifdef USE_DEBUG
#include "utils.h"
#include "thr.h"

#ifdef _WIN32
#define vsnprintf(buf, bufsize, fmt, ap) vsnprintf_s(buf, bufsize, bufsize, fmt, ap)
#endif

namespace DBG
{

static thread_local char str[32];
static thread_local char bf[64];

static pthread_mutex_t dbg_mutex = PTHREAD_MUTEX_INITIALIZER;

//---------------------------------------------------------
void DBG_LOCK()
{
    pthread_mutex_lock(&dbg_mutex);
}

//---------------------------------------------------------
void DBG_UNLOCK()
{
    pthread_mutex_unlock(&dbg_mutex);
}

//---------------------------------------------------------
static struct tm *local_time(const time_t *secs, struct tm *tm)
{
#ifdef __linux__
    tzset();
    if (!localtime_r(secs, tm)) {
        return NULL;
    }
#endif
#ifdef _WIN32
    if (localtime_s(tm, secs) != 0) {
        return NULL;
    }
#endif
    return tm;
}

//---------------------------------------------------------
static struct tm clock_get_local_time(struct timespec *ts)
{
    struct tm tm;
    clock_gettime(CLOCK_REALTIME, ts);
    local_time(&ts->tv_sec, &tm);
#ifdef __linux
    ts->tv_sec += tm.tm_gmtoff;
#endif
    return tm;
}

//---------------------------------------------------------
const char *gettime()
{
    static struct timespec ts;
    struct tm tm = clock_get_local_time(&ts);
    strftime(str, sizeof(str), "%d.%m.%Y %H:%M:%S", &tm);
    snprintf(bf, sizeof(bf), "%s.%06ld", str, ts.tv_nsec / 1000);
    return bf;
}

//---------------------------------------------------------
const char *dbg_gettime__y_m_d__h_m_s()
{
    static struct timespec ts;
    struct tm tm = clock_get_local_time(&ts);
    strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", &tm);
    //snprintf(bf, sizeof(bf), "%s.%06ld", str, ts.tv_nsec / 1000);
    snprintf(bf, sizeof(bf), "%s.%03ld", str, ts.tv_nsec / 1000000);
    return bf;

}

//---------------------------------------------------------
const char *dbg_funcname(const char *funcname, char *outbuf, int outbuflen)
{
    static int swidth = 40;
    if (outbuflen-6 < swidth) {
        return funcname;
    }
    strncpy(outbuf, funcname, outbuflen-6);
    char *p = strchr(outbuf, '(');
    if (p && *(p+1) != ')') {
        *(p+1) = '.';
        *(p+2) = '.';
        *(p+3) = '.';
        *(p+4) = ')';
        *(p+5) = '\0';
    }
    int sz = (int)strlen(outbuf);
    int ln = swidth - sz;
    if (ln < 0) ln = 0;
    int i;
    for(i = 0; i < ln; i++) {
        outbuf[sz + i] = ' ';
    }
    outbuf[sz + i] = '\0';
    return outbuf;
}

//---------------------------------------------------------
void dbg_prints_raw(const char *format, ...)
{
    static char buf[4096] = {0};
    DBG_LOCK();
    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);
    printf("%s", buf);
    fflush(stdout);
    DBG_UNLOCK();
}

//----------------------------------------------------------

int debug_level = 0;

QString     QDebugOutputFile;
QFile       QDebug_QFile;
QTextStream debugTextStream;

//----------------------------------------------------------
void QDebugMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        //fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        if (debug_level == 3)
            fprintf(stderr, "%s \t\t(%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        else
            fprintf(stderr, "%s\n", localMsg.constData());
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        abort();
        break;
    }

    if (debugTextStream.device()) {
        debugTextStream << localMsg.constData() << endl;
    }
}

//---------------------------------------------------------
void dbg_prints(int num, const QString &msg)
{
    auto dt = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    printf("%s [%d] %s", qPrintable(dt), num, qPrintable(msg));
    fflush(stdout);

    if (debugTextStream.device()) {
        char buf[1024*4];
        snprintf(buf, sizeof(buf), "%s [%d] %s", qPrintable(dt), num, qPrintable(msg));
        debugTextStream << buf;
        debugTextStream.flush();
    }
}

} // namespace DBG

#endif //USE_DEBUG
