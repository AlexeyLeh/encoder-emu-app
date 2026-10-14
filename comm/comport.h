/*!
  \file   comport.h
  \author Алексей Лехтин
  \brief  Adam, com-порт реализация
  \date   Nov  7, 2025
*/

#ifndef __CONNECTION_COM_H__
#define __CONNECTION_COM_H__

#include <string>

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "serial_conf.h"

using byte = uint8_t;

class MAIN;
class COMPORT : public QObject
{
    Q_OBJECT;

public:
    COMPORT(MAIN *m);
    virtual ~COMPORT();

public:
    bool            connect_to();
    void            disconnect_from(int err);
    void            set_port_settings(const port_t &port);
    void            clear();

    bool            connected();
    const QString   name() const;
    int             state() const;
    //SERIALPORT      *sp();

    void            read_settings();
    void            save_settings();

    int             error() const;
    QString         error_string() const;
    static QString  error_reason(int error);


    bool            _connect_to();
    bool            _disconnect_from();


    bool            request(const std::vector<byte> &bytes, std::string &errmsg);
    static void     delay(int ms);

public:
    QSerialPort     *qt_serialport() const;
    port_t          port_settings() const;
    static QString  port_info(const port_t &port);
    static QString  port_info_short(const port_t &port);

private:
    void            read_data(QIODevice *device);

private slots:
    void            slot_serialport_error(QSerialPort::SerialPortError err);
    void            slot_serialport_ready_read();

signals:
    void            signal_comm_connected();
    void            signal_comm_disconnected(int err);
    void            signal_comm_got_error(int err, QString err_string);
    void            signal_comm_got_bytes(QByteArray data);

private:
    MAIN            *m_main;
    QSerialPort     *m_qt_serial_port;  // последовательный порт Qt
    QByteArray      m_serial_buf;       // буфер приема данных с порта

};

#endif // __CONNECTION_COM_H__
