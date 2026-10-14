/*!
  \file   encoder.cpp
  \author Aleksey Lekhtin
  \date   09.11.2005
  \brief  Коммуникация с декодером ББИ
*/

#include <QComboBox>
#include <QDialog>

#include "comm.h"
#include "dbg.h"
#include "encoder.h"
#include "main.h"
#include "mainwindow.h"
#include "wmsg.h"


//-------------------------------------------------
ENCODER::ENCODER(MAIN *m) :
    m_main(m),
    m_pixmap_gray(":q_circle_gray.png"),
    m_pixmap_red(":q_circle_red.png"),
    m_pixmap_green(":q_circle_green.png")
{
    DBG(3);
    int hh = 24;
    m_pixmap_gray = m_pixmap_gray.scaledToHeight(hh, Qt::SmoothTransformation);
    m_pixmap_green = m_pixmap_green.scaledToHeight(hh, Qt::SmoothTransformation);
    m_pixmap_red = m_pixmap_red.scaledToHeight(hh, Qt::SmoothTransformation);
    read_settings();
}

//-------------------------------------------------
ENCODER::~ENCODER()
{
    DBG(4);
    save_settings();
    delete m_terminal;
}

//-------------------------------------------------
MW *ENCODER::mw() const
{
    return m_main->mw();
}

//-------------------------------------------------
COMM *ENCODER::comm() const
{
    return m_main->comm();
}

//-------------------------------------------------
void ENCODER::run()
{
    DBG(3);

    QList<port_name_t> portlist = SERIALCONF::get_port_list();
    m_comport_list->clear();
    foreach(port_name_t port, portlist) {
        m_comport_list->addItem(port.name, port.values);
    }
    m_comport_list->setCurrentIndex(-1);

    port_t port = comm()->comport()->port_settings();
    int pos = m_comport_list->findText(port.name);
    if(pos != -1) {
        m_comport_list->setCurrentIndex(pos);
    } else {
        port.name = "";
    }

    //---------------------------------
    connect(m_comport_refresh_button, &QPushButton::clicked,
            this, &ENCODER::slot_comport_refresh, Qt::QueuedConnection);

    connect(m_comport_setup_button, &QPushButton::clicked,
            this, &ENCODER::slot_comport_setup, Qt::QueuedConnection);

    connect(m_comport_list, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ENCODER::slot_comport_list_changed, Qt::QueuedConnection);

    connect(m_comport_connect_button, &QPushButton::clicked,
            this, &ENCODER::slot_comport_openclose, Qt::QueuedConnection);

    connect(comm()->comport(), &COMPORT::signal_comm_connected,
            this, &ENCODER::slot_comport_connected, Qt::QueuedConnection);

    connect(comm()->comport(), &COMPORT::signal_comm_disconnected,
            this, &ENCODER::slot_comport_disconnected, Qt::QueuedConnection);

    connect(comm()->comport(), &COMPORT::signal_comm_got_error,
            this, &ENCODER::slot_comport_got_error, Qt::QueuedConnection);

    connect(comm(), &COMM::signal_comm__responded,
            this, [mw = mw(), term = m_terminal](QString msg) {
        if (msg == "OK") return;
        term->put_data(msg.toUtf8());
        if (msg == "DONE" || msg == "STOP") {
            mw->showstatus(msg);
        }
    }, Qt::QueuedConnection);

    //---------------------------------
    connect(m_start_button, &QPushButton::pressed, this, [this]() {
        QString msg;
        std::string errmsg;
        char cmd[16];

        QApplication::setOverrideCursor(Qt::WaitCursor);
        DBG(1, "start pressed");
        emit mw()->showstatus(tr("sending..."), 1);
        m_terminal->put_data_ln();

        // Частота в Hz: "f200\n"
        int freq = m_freq_field->value();
        snprintf(cmd, sizeof(cmd), "f%d\n", freq);
        m_terminal->put_data("> ");
        m_terminal->put_data(QByteArray::fromRawData(cmd, strlen(cmd)));
        bool ok = comm()->send_bytes(cmd, errmsg);
        COMM::delay(100);
        if (!ok) {
            msg = QString::fromStdString(errmsg);
            DBG(1, msg);
            m_terminal->put_data_ln(msg.toUtf8());
            QApplication::restoreOverrideCursor();
            return;
        }

        // количество шагов: "c100\n" - вперёд 100 шагов, "c-200\n" - назад 200 шагов
        snprintf(cmd, sizeof(cmd), "c%d\n", m_steps_field->value());
        m_terminal->put_data("> ");
        m_terminal->put_data(QByteArray::fromRawData(cmd, strlen(cmd)));
        ok = comm()->send_bytes(cmd, errmsg);
        COMM::delay(100);
        if (!ok) {
            msg = QString::fromStdString(errmsg);
            DBG(1, msg);
            m_terminal->put_data_ln(msg.toUtf8());
            QApplication::restoreOverrideCursor();
            return;
        }

        // статус: "i\n"
        snprintf(cmd, sizeof(cmd), "i\n");
        m_terminal->put_data("> ");
        m_terminal->put_data(QByteArray::fromRawData(cmd, strlen(cmd)));
        ok = comm()->send_bytes(cmd, errmsg);
        COMM::delay(100);
        if (!ok) {
            msg = QString::fromStdString(errmsg);
            DBG(1, msg);
            m_terminal->put_data_ln(msg.toUtf8());
            QApplication::restoreOverrideCursor();
            return;
        }

        // запуск: "r\n"
        snprintf(cmd, sizeof(cmd), "r\n");
        m_terminal->put_data("> ");
        m_terminal->put_data(QByteArray::fromRawData(cmd, strlen(cmd)));
        ok = comm()->send_bytes(cmd, errmsg);
        COMM::delay(100);
        if (!ok) {
            msg = QString::fromStdString(errmsg);
            DBG(1, msg);
            m_terminal->put_data_ln(msg.toUtf8());
            QApplication::restoreOverrideCursor();
            return;
        }
        QApplication::restoreOverrideCursor();
        DBGNL(2);
    });

    //---------------------------------
    connect(m_stop_button, &QPushButton::pressed, this, [this]() {
        QString msg;
        std::string errmsg;
        char cmd[16];

        QApplication::setOverrideCursor(Qt::WaitCursor);
        DBG(1, "end pressed");
        emit mw()->showstatus(tr("stop..."), 1);
        m_terminal->put_data_ln();

        // остановка генерации: "e\n"
        snprintf(cmd, sizeof(cmd), "e\n");
        m_terminal->put_data("> ");
        m_terminal->put_data(QByteArray::fromRawData(cmd, strlen(cmd)));
        bool ok = comm()->send_bytes(cmd, errmsg);
        COMM::delay(100);
        if (!ok) {
            msg = QString::fromStdString(errmsg);
            DBG(1, msg);
            m_terminal->put_data_ln(msg.toUtf8());
            QApplication::restoreOverrideCursor();
            return;
        }
        QApplication::restoreOverrideCursor();
        DBGNL(2);
    });
}

//---------------------------------------------------------
void ENCODER::read_settings()
{
    DBG(3);
    QSettings *st = m_main->qSettings();
    m_freq_ini = st->value("Enc/freq", "1000").toInt(); // Hz
    m_steps_ini = st->value("Enc/steps", "100").toInt();
    m_terminal_ini = st->value("Enc/terminal", true).toBool();
}

//---------------------------------------------------------
void ENCODER::save_settings()
{
    DBG(3);
    QSettings *st = m_main->qSettings();
    st->setValue("Enc/freq",  m_freq_field->value());
    st->setValue("Enc/steps", m_steps_field->value());
    st->setValue("Enc/terminal", m_terminal_ini);
}
