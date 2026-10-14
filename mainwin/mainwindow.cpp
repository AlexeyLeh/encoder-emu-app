/*!
  \file   mainwindow.cpp
  \author Aleksey Lekhtin
  \date   created 17.11.2014
  \brief  Главное окно приложения
*/

#include <QWidget>
#include <QWindow>
#include <QMessageBox>

#include "dbg.h"
#include "utils.h"
#include "ver.h"

#include "comm.h"
#include "encoder.h"
#include "main.h"
#include "mainwindow.h"
#include "translator.h"
#include "wmsg.h"

//---------------------------------------------------------
// Main window of application
MW::MW(MAIN *m) :
    QMainWindow(),
    m_main(m)
    //m_status_bar_permanent_Label(new QLabel())
{
    setAttribute(Qt::WA_QuitOnClose, true);
    setWindowTitle(m_main->app_title());
    setMinimumSize(300, 300);
    read_settings();
    create_menu();
    auto wgt = m_main->enc()->create_gui();
    setCentralWidget(wgt);
}

//---------------------------------------------------------
MW::~MW()
{
    DBG(4);
    save_settings();
}

//---------------------------------------------------------
void MW::closeEvent(QCloseEvent *ev)
{
    DBG(3);
    statusBar()->showMessage(tr("Shutdown..."));
    ev->accept();
}

//---------------------------------------------------------
void MW::read_settings()
{
    DBG(3);
    QSettings *st = m_main->qSettings();

    m_app_size = {0, 0};
    m_app_position = {0, 0};

    m_app_size = st->value("size", QSize(0,0)).toSize();
    m_app_position = st->value("pos" , QPoint(0,0)).toPoint();
    m_fullscreen = st->value("fullscreen" , false).toBool();

    m_menu_font_size      = st->value("Font/menu_size", "0").toString().left(2).toInt();
    m_content_font_size   = st->value("Font/content_size", "0").toString().left(2).toInt();
    m_statusbar_font_size = st->value("Font/statusbar_size", "0").toString().left(2).toInt();
    m_wmsg_font_size       = st->value("Font/winmsg_size", "0").toString().left(2).toInt();
}

//---------------------------------------------------------
void MW::save_settings()
{
    DBG(3);
    QSettings *st = m_main->qSettings();

    if (windowState() == Qt::WindowMaximized) {
        st->setValue("fullscreen", true);
    } else {
        st->setValue("fullscreen", false);
        st->setValue("pos",  pos());
        st->setValue("size", size());
    }

    st->setValue("Font/menu_size",      m_menu_font_size);
    st->setValue("Font/content_size",   m_content_font_size);
    st->setValue("Font/statusbar_size", m_statusbar_font_size);
    st->setValue("Font/winmsg_size",    m_wmsg_font_size);
}

//---------------------------------------------------------
void MW::run()
{
    DBG(3);
    if (m_fullscreen) {
        showMaximized();
    }

    if (!m_app_size.isEmpty()) {
        resize(m_app_size);
    }

    if (!m_app_position.isNull()) {
        move(m_app_position);
    }

    slot_set_menu_font_size();
    slot_set_content_font_size();
    slot_set_statusbar_font_size();
    slot_set_wmsg_font_size();

    //m_status_bar_permanent_Label->setText("Status: OK");
    //statusBar()->addPermanentWidget(m_status_bar_permanent_Label);
    showstatus(tr("Right-Click - Settings"), 10);
}

//---------------------------------------------------------
void MW::slot_showstatus(const QString &msg, int sec)
{
    showstatus(msg, sec * 1000);
}

//---------------------------------------------------------
void MW::showstatus(const QString &msg, int sec)
{
    statusBar()->showMessage(msg, sec * 1000);
}

// //---------------------------------------------------------
// void MW::setPermanetStatus(const QString &msg)
// {
//     m_status_bar_permanent_Label->setText(msg);
// }

//---------------------------------------------------------
void MW::clearstatus()
{
    statusBar()->clearMessage();
}

//---------------------------------------------------------
// Пауза (без "замораживания" GUI)
void MW::delay(int ms)
{
    QDateTime tim_cur = QDateTime::currentDateTime();
    QDateTime tim_end = tim_cur.addMSecs(ms);
    do {
        tim_cur = QDateTime::currentDateTime();
        QApplication::processEvents();
    } while (tim_cur < tim_end);
}

//---------------------------------------------------------
COMM *MW::comm() const
{
    return m_main->comm();
}

//---------------------------------------------------------
void MW::slot_about()
{
    QString trmsg = tr("Translation not found.\nUsing en_US");
    if (!m_main->tr()->name().isEmpty()) {
        trmsg = QString("%1:\n%2")
                .arg(tr("Translations"), QDir::toNativeSeparators(m_main->tr()->name()));
    }
    QString msg = QString("%1\n%2\n\n%3")
            .arg(m_main->app_title(), m_main->version(),trmsg);
    WMSG::WMsg(this, tr("About Application"), msg, QMessageBox::Information);
}
