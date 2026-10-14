/*!
  \file   comm.cpp
  \author Aleksey Lekhtin
  \brief  Коммуникация с устройствами
  \date   10.05.2023
*/


#include <QtGlobal>
#include <QString>

#include <sstream>
#include <string>

#include "comm.h"
#include "comport.h"
#include "encoder.h"
#include "main.h"
#include "mainwindow.h"
#include "utils.h"

COMM::COMM(MAIN *m) :
    m_main(m),
    m_com(new COMPORT(m))
{
    DBG(3);
    read_settings();

    connect(m_com, SIGNAL(signal_comm_connected()),
            this, SLOT(slot_comm_connected()), Qt::QueuedConnection);

    connect(m_com, SIGNAL(signal_comm_disconnected(int)),
            this, SLOT(slot_comm_disconnected(int)), Qt::QueuedConnection);

    connect(m_com, SIGNAL(signal_comm_got_error(int, QString)),
            this, SLOT(slot_comm_got_error(int, QString)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_comm__not_responded(QString)),
            this, SLOT(slot_comm__not_responded(QString)), Qt::QueuedConnection);

    connect(m_com, SIGNAL(signal_comm_got_bytes(QByteArray)),
            this, SLOT(slot_got_bytes(QByteArray)), Qt::DirectConnection);
}

//-------------------------------------------------
COMM::~COMM()
{
    DBG(4);
    save_settings();
    delete m_com;
}

//-------------------------------------------------
MW *COMM::mw() const
{
    return m_main->mw();
}

//-------------------------------------------------
COMPORT *COMM::comport() const
{
    return m_com;
}

//-------------------------------------------------
const QString COMM::name() const
{
    return m_com->name();
}

//-------------------------------------------------
int COMM::state() const
{
    return m_com->state();
}

//-------------------------------------------------
QString COMM::state_string() const
{
    return (m_com->state() ? "Open" : "Close");
}

//-------------------------------------------------
bool COMM::isAlive()
{
    return m_com->connected();
}

//-------------------------------------------------
bool COMM::connect_to()
{
    DBG(3);
     if (m_com->connected()) {
        return true;
    }
    return m_com->connect_to();
}

//-------------------------------------------------
void COMM::disconnect_from(int err)
{
    m_com->disconnect_from(err);
}

//-------------------------------------------------
int COMM::error() const
{
    return m_com->error();
}

//-------------------------------------------------
QString COMM::errorString() const
{
    return m_com->error_string();
}

//-------------------------------------------------
QString COMM::errorReason() const
{
    return m_com->error_reason(m_com->error());
}

//-------------------------------------------------
QString COMM::errorReason(int err) const
{
    return m_com->error_reason(err);
}

//-------------------------------------------------
void COMM::read_settings()
{
    //DBG(3);
}

//-------------------------------------------------
void COMM::save_settings()
{
    //DBG(3);
}

//-------------------------------------------------
bool COMM::send_bytes(const char *msg, std::string &errmsg)
{
    size_t length = strlen(msg);
    const std::vector<byte> bytes(reinterpret_cast<const byte*>(msg),
                                  reinterpret_cast<const byte*>(msg + length));
    return send_bytes(bytes, errmsg);
}

//-------------------------------------------------
bool COMM::send_bytes(const std::vector<byte> &bytes, std::string &errmsg)
{
    if (DBG::debug_level >= 2) {
        DBGNNL(2, "send: ");
        foreach(const auto &b, bytes) {
            DBG::dbg_prints_raw("%c", (isprint(b) ? b : '.'));
        }
        DBG::dbg_prints_raw(" ( ");
        foreach(const auto &b, bytes) {
            DBG::dbg_prints_raw("%02X ", b);
        }
        DBG::dbg_prints_raw(")");
        DBGNL(2);
    }

    bool ok = m_com->request(bytes, errmsg);
    return ok;
}

//-------------------------------------------------
QString &COMM::response()
{
    return m_response;
}

//-------------------------------------------------
void COMM::slot_got_bytes(const QByteArray ba)
{
    const std::vector<byte> bytes(ba.begin(), ba.end());
    if (DBG::debug_level >= 2) {
        DBGNNL(2, "got:  ");
        foreach(const auto &b, bytes) {
            DBG::dbg_prints_raw("%c", (isprint(b) ? b : '.'));
        }
        DBG::dbg_prints_raw(" ( ");
        foreach(const auto &b, bytes) {
            DBG::dbg_prints_raw("%02X ", b);
        }
        DBG::dbg_prints_raw(")");
        DBGNL(2);
    }

    m_response = ba;
    m_response.replace('\r', '\n');

    DBG(3, QString("%1").arg(m_response));
    DBG(4, "emit signal_comm__responded();");
    emit signal_comm__responded(m_response);
}

//-------------------------------------------------
/**
 * @brief Задержка в ms
 * @param ms - задержка
 */
void COMM::delay(int ms)
{
    QDateTime tim_cur = QDateTime::currentDateTime();
    QDateTime tim_end = tim_cur.addMSecs(ms);
    do {
        QCoreApplication::processEvents();
        //QCoreApplication::sendPostedEvents();
        tim_cur = QDateTime::currentDateTime();
    } while(tim_cur < tim_end);
}
