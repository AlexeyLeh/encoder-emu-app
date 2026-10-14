/*!
  \file   serial_conf.h
  \brief  Serial port form
  \author Aleksey Lekhtin
  \date   created 17.11.2014
*/

#ifndef __SERIALCONF_H_
#define __SERIALCONF_H_

#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSerialPort>
#include <QSerialPortInfo>

typedef struct port_name_t {
    QString name;
    QStringList values;
} port_name_t;

//-----------------------------------------------------------
// Структура описания последовательного порта
//
class port_t
{
public:
    port_t(QString portname = "",
           int baud = QSerialPort::Baud9600,
           QSerialPort::DataBits bits = QSerialPort::Data8,
           QSerialPort::StopBits stop = QSerialPort::OneStop,
           QSerialPort::Parity parit = QSerialPort::NoParity,
           QSerialPort::FlowControl flowctl = QSerialPort::NoFlowControl)
    {
        name      = portname;
        baudRate  = baud;
        dataBits  = bits;
        stopBits  = stop;
        parity    = parit;
        flowCtrl  = flowctl;
    };

    //-------------------------------------------
    ~port_t()
    {
        ;
    }

    //-------------------------------------------
    static QString def_name()
    {
        QString def_name = QStringLiteral("/dev/ttyS0");
#ifdef Q_OS_WIN
        def_name = QStringLiteral("COM1");
#endif
        return def_name;
    }

    //-------------------------------------------
    port_t operator=(const port_t & port)
    {
        name     = port.name;
        baudRate  = port.baudRate;
        dataBits  = port.dataBits;
        parity    = port.parity;
        stopBits  = port.stopBits;
        flowCtrl  = port.flowCtrl;
        return *this;
    }

    //-------------------------------------------
    QString                  name;
    int                      baudRate;
    QSerialPort::DataBits    dataBits;
    QSerialPort::Parity      parity;
    QSerialPort::StopBits    stopBits;
    QSerialPort::FlowControl flowCtrl;
};

class SERIALCONF : public QWidget
{
   Q_OBJECT;

public:

   explicit SERIALCONF(QWidget *parent = 0);
   ~SERIALCONF();

   const port_t &port_settings() const;
   void     set_port_settings(const port_t & port);
   QWidget  *create_fields_layout();
   void     update_form_fields();
   void     flush_fields();

   static QList<port_name_t> get_port_list();

private slots:
   void slot_checkCustomBaudRatePolicy(int index);

private:
   void set_form_values();

private:
   QGroupBox     *m_parametersBox;
   QLabel        *m_baudRateLabel;
   QLabel        *m_dataBitsLabel;
   QLabel        *m_parityLabel;
   QLabel        *m_stopBitsLabel;
   QLabel        *m_flowCtrlLabel;

   QComboBox     *m_baudRateBox;
   QComboBox     *m_dataBitsBox;
   QComboBox     *m_parityBox;
   QComboBox     *m_stopBitsBox;
   QComboBox     *m_flowCtrlBox;

   QGroupBox     *m_miscBox;
   QPushButton   *m_refreshButton0;
   QCheckBox     *m_localEchoCheckBox;

   port_t         m_port;           // выбранные параметры
   QIntValidator *m_intValidator;   // проверка на тип - число
};

#endif // __SERIALCONF_H_
