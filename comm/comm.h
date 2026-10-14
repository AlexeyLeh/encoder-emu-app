/*!
  \file   comm.h
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Коммуникация с устройствами
*/

#ifndef __COMM_H_
#define __COMM_H_

#include <mutex>

#include <QAbstractSocket>
#include <QObject>
#include <QTimer>
#include <QTime>
#include <QSettings>

#include "comport.h"
#include "serial_conf.h"

class IConn;
class MAIN;
class MW;

class COMM : public QObject
{
    Q_OBJECT;

public:
    struct parameters
    {
        QString     host;
        int         port;
        int         timeout;
        int         m_rtime;        // ReadTime - время ожидания при чтении с Adam-устройства
    };

    using Parameters = struct parameters;

public:
    COMM(MAIN *m);
    ~COMM();

public:
    MW              *mw() const;
    bool            isAlive();
    void            read_settings();
    void            save_settings();

    COMPORT         *comport() const;
    const QString   name() const;
    int             state() const;
    QString         state_string() const;
    bool            connect_to();
    void            disconnect_from(int err = QSerialPort::NoError);
    bool            send_bytes(const std::vector<byte> &bytes, std::string &errmsg);
    bool            send_bytes(const char *bytes, std::string &errmsg);
    QString         &response();

    int             error() const;
    QString         errorString() const;
    QString         errorReason() const;
    QString         errorReason(int err) const;

    static void     delay(int ms);

private slots:
    void            slot_got_bytes(const QByteArray ba);
    void            slot_comm_connected();
    void            slot_comm_disconnected(int err);
    void            slot_comm_got_error(int err, QString err_string);
    void            slot_comm__not_responded(QString err_string);

signals:
    void            signal_comm__comport_connected();
    void            signal_comm__comport_disconnected();
    void            signal_comm__not_responded(QString err_string);
    void            signal_comm__responded(QString response);

    void            signal_showstatus(const QString &msg, int sec);
    void            signal_message(const QString &msg);

private:
    MAIN            *m_main;    // манагер глобальных ресурсов программы
    COMPORT         *m_com;
    QString         m_response;
    std::mutex      m_lock_queue;

    // settings from .ini
public:
};

#endif // __COMM_H_
