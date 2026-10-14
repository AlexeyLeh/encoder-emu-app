/*!
  \file   conn_com.cpp
  \author Алексей Лехтин
  \brief  Adam, com-порт реализация
  \date   10.05.2023
*/

#include <string>
#include <iostream>

#include <QApplication>
#include <QtDebug>
#include <QDateTime>
#include <QByteArray>
#include <QSerialPort>

#include "comm.h"
#include "comport.h"
#include "dbg.h"
#include "main.h"

//---------------------------------------------------------
COMPORT::COMPORT(MAIN *m) :
    m_main(m),
    m_qt_serial_port(new QSerialPort())
{
    read_settings();
    connect(m_qt_serial_port, SIGNAL(readyRead()), this, SLOT(slot_serialport_ready_read()));

    connect(m_qt_serial_port, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
            this, SLOT(slot_serialport_error(QSerialPort::SerialPortError)));
}

//-------------------------------------------------
COMPORT::~COMPORT()
{
    DBG(4);
    _disconnect_from();
    save_settings();
    delete m_qt_serial_port;
}

//-------------------------------------------------
QSerialPort *COMPORT::qt_serialport() const
{
    return m_qt_serial_port;
}

//-------------------------------------------------
const QString COMPORT::name() const
{
    return m_qt_serial_port->portName();
}

//-------------------------------------------------
int COMPORT::state() const
{
    return (int)m_qt_serial_port->isOpen();
}

//-------------------------------------------------
bool COMPORT::connected()
{
    return m_qt_serial_port->isOpen();
}

//-------------------------------------------------
/**
 * @brief Задержка в ms
 * @param ms - задержка
 */
void COMPORT::delay(int ms)
{
    QDateTime tim_cur = QDateTime::currentDateTime();
    QDateTime tim_end = tim_cur.addMSecs(ms);
    do {
        QCoreApplication::processEvents();
        QCoreApplication::sendPostedEvents();
        struct timespec ts = {.tv_sec = 0, .tv_nsec = 10000};
        nanosleep(&ts, NULL);
        tim_cur = QDateTime::currentDateTime();
    } while(tim_cur < tim_end);
}

//-----------------------------------------------------------
void COMPORT::clear()
{
    m_qt_serial_port->close();
}

//-------------------------------------------------
void COMPORT::read_settings()
{
    DBG(3);
    QSettings *st = m_main->qSettings();
    st->beginGroup("Comport");

    QString portName = st->value( "Port", port_t::def_name() ).toString();
    m_qt_serial_port->setPortName(portName);
    int baudRate = (QSerialPort::BaudRate)st->value( QStringLiteral("Speed"), 9600 ).toInt();
    m_qt_serial_port->setBaudRate(baudRate);
    QSerialPort::DataBits dataBits = (QSerialPort::DataBits)st->value( QStringLiteral("Bits"),     8 ).toInt();
    m_qt_serial_port->setDataBits(dataBits);
    QSerialPort::StopBits stopBits = (QSerialPort::StopBits)st->value( QStringLiteral("StopBits"), 1 ).toInt();
    m_qt_serial_port->setStopBits(stopBits);

    QSerialPort::Parity parity = QSerialPort::UnknownParity;
    QString str = st->value("Parity", "Auto").toString();
    if      (str.toLower() == QStringLiteral("even" )) parity = QSerialPort::EvenParity;
    else if (str.toLower() == QStringLiteral("odd"  )) parity = QSerialPort::OddParity;
    else if (str.toLower() == QStringLiteral("space")) parity = QSerialPort::SpaceParity;
    else if (str.toLower() == QStringLiteral("mark" )) parity = QSerialPort::MarkParity;
    else if (str.toLower() == QStringLiteral("no"   )) parity = QSerialPort::NoParity;
    m_qt_serial_port->setParity(parity);

    QSerialPort::FlowControl flowctl = QSerialPort::UnknownFlowControl;
    str = st->value( "FlowControl", "None" ).toString();
    if      (str.toUpper() == QStringLiteral("RTS/CTS" )) flowctl = QSerialPort::HardwareControl;
    else if (str.toUpper() == QStringLiteral("XON/XOFF")) flowctl = QSerialPort::SoftwareControl;
    else if (str.toUpper() == QStringLiteral("None"    )) flowctl = QSerialPort::NoFlowControl;
    m_qt_serial_port->setFlowControl(flowctl);
    st->endGroup();
}

//-------------------------------------------------
void COMPORT::save_settings()
{
    DBG(3);
    QSettings *st = m_main->qSettings();
    QString sParity = QStringLiteral("Auto");
    if (m_qt_serial_port->parity() == QSerialPort::EvenParity ) sParity = QStringLiteral("Even");
    if (m_qt_serial_port->parity() == QSerialPort::OddParity  ) sParity = QStringLiteral("Odd");
    if (m_qt_serial_port->parity() == QSerialPort::SpaceParity) sParity = QStringLiteral("Space");
    if (m_qt_serial_port->parity() == QSerialPort::MarkParity ) sParity = QStringLiteral("Mark");
    if (m_qt_serial_port->parity() == QSerialPort::NoParity   ) sParity = QStringLiteral("No");

    QString sFlowctl = QStringLiteral("None");
    if (m_qt_serial_port->flowControl() == QSerialPort::HardwareControl) sFlowctl = QStringLiteral("RTS/CTS");
    if (m_qt_serial_port->flowControl() == QSerialPort::SoftwareControl) sFlowctl = QStringLiteral("XON/XOFF");
    if (m_qt_serial_port->flowControl() == QSerialPort::NoFlowControl  ) sFlowctl = QStringLiteral("None");

    if (m_qt_serial_port) {
        st->beginGroup("Comport");
        st->setValue("Port",        m_qt_serial_port->portName());
        st->setValue("Speed",       (int)m_qt_serial_port->baudRate());
        st->setValue("Bits",        (int)m_qt_serial_port->dataBits());
        st->setValue("Parity",      sParity);
        st->setValue("StopBits",    (int)m_qt_serial_port->stopBits());
        st->setValue("FlowControl", sFlowctl);
        st->endGroup();
    }
}

//-----------------------------------------------------------
port_t COMPORT::port_settings() const
{
    port_t port(m_qt_serial_port->portName(),
                m_qt_serial_port->baudRate(),
                m_qt_serial_port->dataBits(),
                m_qt_serial_port->stopBits(),
                m_qt_serial_port->parity(),
                m_qt_serial_port->flowControl());
    return port;
}

//-------------------------------------------------
void COMPORT::set_port_settings(const port_t &port)
{
    m_qt_serial_port->setPortName(port.name);
    m_qt_serial_port->setBaudRate(port.baudRate);
    m_qt_serial_port->setDataBits(port.dataBits);
    m_qt_serial_port->setStopBits(port.stopBits);
    m_qt_serial_port->setParity(port.parity);
    m_qt_serial_port->setFlowControl(port.flowCtrl);
    save_settings();
}

//-------------------------------------------------
bool COMPORT::connect_to()
{
    DBGF(2, "'%s'", qPrintable(name()));
    bool ok = _connect_to();
    if (m_qt_serial_port->isOpen()) {
        DBG(2, "emit signal_conn_connected();");
        emit signal_comm_connected();
    }
    return ok;
}

//-------------------------------------------------
bool COMPORT::_connect_to()
{
    if (m_qt_serial_port->isOpen()) {
        DBG(3, QString("%1 already connected - going to be close").arg(name()));
        m_qt_serial_port->setRequestToSend(false);
        m_qt_serial_port->close();
        QApplication::processEvents();
        COMPORT::delay(100);
    }

    if (m_qt_serial_port->open(QIODevice::ReadWrite)) {
        DBG(1, QString("connected to %1 %2 - status: %3")
            .arg(name())
            .arg(m_qt_serial_port->baudRate())
            .arg(m_qt_serial_port->isOpen() ? "Open" : "Close"));
        m_qt_serial_port->setRequestToSend(false);
    } else {
        DBG(1, QString("%1 - failed to connect - %2")
            .arg(port_info_short(port_settings()))
            .arg(error_string()));
        return false;
    }

    if (!m_qt_serial_port->setParity(m_qt_serial_port->parity())) {
        DBG(3, QString("err: %1, '%2'").arg(error()).arg(error_string()));
    }
    return true;
}

//-------------------------------------------------
void COMPORT::disconnect_from(int err)
{
    if (m_qt_serial_port->isOpen()) {
        DBG(3);
        if (_disconnect_from()) {
            DBG(3, "emit signal_conn_disconnected()");
            emit signal_comm_disconnected(err);
        }
    }
}

//-------------------------------------------------
bool COMPORT::_disconnect_from()
{
    if (m_qt_serial_port->isOpen()) {
        DBG(3, QString("'%1' status:%2").arg(name()).arg((state() ? "Open" : "Close")));
        COMPORT::delay(100);
        m_qt_serial_port->close();
        QApplication::processEvents();
        DBG(3, QString("now status:%2").arg((state() ? "Open" : "Close")));
        return true;
    }
    return false;
}

//---------------------------------------------------------
void COMPORT::slot_serialport_error(QSerialPort::SerialPortError err)
{
    if (err == QSerialPort::NoError) {
        return;
    }
    DBG(1, QString("'%1', state: %2, err: %3, '%4'")
        .arg(m_qt_serial_port->portName())
        .arg(state()).arg(error()).arg(error_string()));
    auto msg = QString("%1 ;; %2 ;; %3")
            .arg(port_info_short(port_settings()))
            .arg(error_string())
            .arg(error_reason(error()));
    DBG(3, "emit signal_conn_got_error()");
    emit signal_comm_got_error(error(), msg);
}

//-------------------------------------------------
int COMPORT::error() const
{
    return m_qt_serial_port->error();
}

//-------------------------------------------------
QString COMPORT::error_string() const
{
    return m_qt_serial_port->errorString();
}

//-------------------------------------------------
QString COMPORT::error_reason(int error)
{
    QString reason;
    switch(error)
    {
    case QSerialPort::NoError:
        reason = tr("No error occurred");
        break;
    case QSerialPort::DeviceNotFoundError:
        reason = tr("An error occurred while attempting to open an non-existing device");
        break;
    case QSerialPort::PermissionError:
        reason = tr("An error occurred while attempting to open an already opened device by another process or a user not having enough permission and credentials to open");
        break;
    case QSerialPort::OpenError:
        reason = tr("An error occurred while attempting to open an already opened device in this object");
        break;
    case QSerialPort::NotOpenError:
        reason = tr("The device is not open");
        break;
    case QSerialPort::WriteError:
        reason = tr("An I/O error occurred while writing the data");
        break;
    case QSerialPort::ReadError:
        reason = tr("An I/O error occurred while reading the data");
        break;
    case QSerialPort::ResourceError:
        reason = tr("An I/O error occurred when a resource becomes unavailable,\ne.g. when the device is unexpectedly removed from the system");
        break;
    case QSerialPort::UnsupportedOperationError:
        reason = tr("The requested device operation is not supported or\nprohibited by the running operating system");
        break;
    case QSerialPort::TimeoutError:
        reason = tr("A timeout error occurred");
        break;
    case QSerialPort::UnknownError:
        reason = tr("An unidentified error occurred");
        break;
    default:
        reason = tr("An unidentified error occurred");
        break;
    }
    return reason;
}

//-------------------------------------------------
bool COMPORT::request(const std::vector<byte> &bytes, std::string &errmsg)
{
    QString msg;
    if (!m_qt_serial_port->isOpen()) {
        int err = QSerialPort::NotOpenError;
        msg = error_reason(err);
        errmsg = msg.toStdString();
        DBG(3, "QSerialPort::NotOpenError");
        DBG(3, QString("err: %1, '%2'").arg(err).arg(msg));
        DBG(3, "emit signal_conn_got_error()");
        emit signal_comm_got_error(err, msg);
        return false;
    }

    const char *p = reinterpret_cast<const char *>(bytes.data());
    int sz = bytes.size();

    if (!m_qt_serial_port->isOpen()) {
        errmsg = tr("Serial port is not open").toStdString();
        return false;
    }

    if (!m_qt_serial_port->setRequestToSend(true)) {
        msg = QString("err: %1, '%2'").arg(error()).arg(error_string());
        DBG(3, msg);
        errmsg = msg.toStdString();
        return false;
    }

    for(int i = 0; i < sz; ++i)
    {
        if (!m_qt_serial_port->isOpen()) {
            errmsg = tr("Serial port is not open").toStdString();
            return false;
        }

        qint64 sz = m_qt_serial_port->write(p, 1);
        if (sz == -1)
        {
            msg = QString("err: %1, '%2'").arg(error()).arg(error_string());
            DBG(3, msg);
            errmsg = msg.toStdString();
            return false;
        }

        QCoreApplication::sendPostedEvents();
        COMPORT::delay(6);
        ++p;
    }

    if (!m_qt_serial_port->isOpen()) {
        errmsg = tr("Serial port is not open").toStdString();
        return false;
    }

    if (!m_qt_serial_port->setRequestToSend(false))
    {
        msg = QString("err: %1, '%2'").arg(error()).arg(error_string());
        DBG(3, msg);
        errmsg = msg.toStdString();
        return false;
    }

    if (!m_qt_serial_port->isOpen()) {
        errmsg = tr("Serial port is not open").toStdString();
        return false;
    }

    return true;
}

//---------------------------------------------------------
void COMPORT::slot_serialport_ready_read()
{
    DBG(4);
    read_data(m_qt_serial_port);
}

//---------------------------------------------------------
QString COMPORT::port_info(const port_t &port)
{
    auto name = port.name;
    if (name.isEmpty())
    {
#ifdef Q_OS_WIN
        name = "COM?";
#else
        name = "/dev/ttyS?";
#endif
    }

    QString str = QString("[%1, %2, %3, %4, %5, %6]")
            .arg(name)
            .arg(port.baudRate)
            .arg(port.dataBits)
            .arg(port.stopBits)
            .arg(port.parity)
            .arg(port.flowCtrl);
    return str;
}

//---------------------------------------------------------
QString COMPORT::port_info_short(const port_t &port)
{
    auto name = port.name;
    if (name.isEmpty())
    {
#ifdef Q_OS_WIN
        name = "COM?";
#else
        name = "/dev/ttyS?";
#endif
    }

    QString str = QString("[%1, %2, %3, %4]")
            .arg(name)
            .arg(port.baudRate)
            .arg(port.dataBits)
            .arg(port.stopBits);
    return str;
}

//---------------------------------------------------------
inline void COMPORT::read_data(QIODevice *device)
{
    if (!device) return;
    m_serial_buf = device->readAll();
    DBG(3, QString("got %1 bytes - '%2'").arg(m_serial_buf.size()).arg(m_serial_buf.toHex().constData()));
    DBG(4, "emit signal_comm_got_bytes();");
    emit signal_comm_got_bytes(m_serial_buf);
}
