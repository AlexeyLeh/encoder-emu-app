/**
  \file   dbg.h
  \author Aleksey Lekhtin
  \date   Nov  7, 2025
  \brief  debug features
*/

#ifndef DBG__H_
#define DBG__H_

#include <stdio.h>
#include <string.h>

#include <QFile>
#include <QString>
#include <QTextStream>

namespace DBG
{

extern int debug_level;
void dbg_prints(int num, const QString &msg);

extern QString     QDebugOutputFile;
extern QFile       QDebug_QFile;
extern QTextStream debugTextStream;
void QDebugMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#define USE_DEBUG

#ifdef USE_DEBUG

const char *dbg_funcname(const char *funcname, char *outbuf, int outbuflen);
const char *dbg_gettime__y_m_d__h_m_s();
void dbg_prints_raw(const char *format, ...);
void DBG_LOCK();
void DBG_UNLOCK();

/*-------------------------------------------------------*/
#define DBGNNL(LEVEL, ...)  \
do { \
    if (DBG::debug_level >= (LEVEL)) { \
        char _func_name_[64];     \
        const char *_t_i_m_ = DBG::dbg_gettime__y_m_d__h_m_s(); \
        DBG::dbg_funcname(__PRETTY_FUNCTION__, _func_name_, sizeof(_func_name_)); \
        char *_p_ = _func_name_ + strlen(_func_name_); \
        while (*_p_ == ' ' || *_p_ == '\t') { \
            *_p_ = '\0'; \
            --_p_; \
        } \
        DBG::DBG_LOCK(); \
        QString _msg_ptr_ = QString(__VA_ARGS__); \
        if (!_msg_ptr_.isEmpty()) {    \
            int _o_f_f_ = 0; \
            if (_msg_ptr_[0] == '\n') { \
                puts(""); \
                _o_f_f_ = 1; \
            }            \
            printf("%s [%d] %s %s", _t_i_m_, LEVEL, _func_name_, qPrintable(_msg_ptr_) + _o_f_f_); \
        } else { \
            printf("%s [%d] %s", _t_i_m_, LEVEL, _func_name_); \
        } \
        fflush(stdout); \
        DBG::DBG_UNLOCK(); \
} \
} while(0)

/*-------------------------------------------------------*/
#define DBG(LEVEL, ...) \
do { \
    if (DBG::debug_level >= (LEVEL)) { \
        QString _msg_ = QString(__VA_ARGS__); \
        if (_msg_.isEmpty()) {    \
            DBGNNL(LEVEL, QString(" \n")); \
        } else { \
            DBGNNL(LEVEL, _msg_ + QStringLiteral("\n")); \
        } \
    } \
} while(0)

/*-------------------------------------------------------*/
#define DBGFNNL(LEVEL, FORMAT, ...)  \
do { \
    if (DBG::debug_level >= (LEVEL)) { \
        const char *_t_i_m_ = DBG::dbg_gettime__y_m_d__h_m_s(); \
        char _buf_[1024*6]; \
        char _func_name_[64]; \
        DBG::dbg_funcname(__PRETTY_FUNCTION__, _func_name_, sizeof(_func_name_)); \
        char *p_t_r = _func_name_ + strlen(_func_name_); \
        while (*p_t_r == ' ' || *p_t_r == '\t') { \
            *p_t_r = '\0'; \
            p_t_r--; \
        } \
        snprintf(_buf_, sizeof(_buf_), FORMAT, __VA_ARGS__);      \
        int _o_f_f_ = 0; \
        DBG::DBG_LOCK(); \
        if (_buf_[0] == '\n') { \
            puts(""); \
            _o_f_f_ = 1; \
        } \
        printf("%s [%d] %s %s", _t_i_m_, LEVEL, _func_name_, _buf_ + _o_f_f_); \
        fflush(stdout); \
        DBG::DBG_UNLOCK(); \
    } \
} while(0)

/*-------------------------------------------------------*/
#define DBGF(LEVEL, FORMAT, ...) DBGFNNL(LEVEL, FORMAT "\n", __VA_ARGS__)

#define DBGRAW(LEVEL, FORMAT, ...) \
do { \
    if (debug_level >= (LEVEL)) { \
        DBG::dbg_prints_raw(FORMAT, __VA_ARGS__); \
    } \
} while(0)

/*-------------------------------------------------------*/
#define DBGNL(LEVEL) \
do { \
    if (DBG::debug_level >= (LEVEL)) { \
        DBG::DBG_LOCK(); \
        puts(""); \
        fflush(stdout); \
        DBG::DBG_UNLOCK(); \
    } \
} while(0)

#else // USE_DEBUG

/*-------------------------------------------------------*/
#define DBGNNL(LEVEL, ...)
#define DBG(LEVEL, ...)
#define DBGFNNL(LEVEL, FORMAT, ...)
#define DBGF(LEVEL, FORMAT, ...)
#define DBGNL(LEVEL)

#endif // USE_DEBUG

} //namesapce DBG

#endif //DBG__H_
