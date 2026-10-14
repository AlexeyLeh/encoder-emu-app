/*!
  \file   mainwindow.h
  \author Aleksey Lekhtin
  \date   created 17.11.2014
  \brief  Главный класс данных
*/

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <mutex>

#include <QtWidgets>
#include <QPrinter>

#include "comm.h"

class MAIN;
class API;
class BTO;
class DIAGN;
class FIRMWARE;
class PLATO;

//-----------------------------------------------------------
class MW : public QMainWindow
{
    Q_OBJECT;

public:
    explicit MW(MAIN *m);
    ~MW();

public:
    void                read_settings();
    void                save_settings();
    void                run();
    COMM                *comm() const;

    void                get_fonts_size();
    int                 content_font_pointSize() const;
    void                clearstatus();
    void                showstatus(const QString &msg, int sec = 60);
    //void              setPermanetStatus(const QString &msg);

    static void         delay(int ms);
    static QString      make_tip(QAction *act);
    static QString      make_tip(const QString &tip, QKeySequence keyseq);

protected:
    void                closeEvent(QCloseEvent *e);
    //void                resizeEvent(QResizeEvent *event);

private:
    void                create_menu();

private slots:
    void                slot_about();
    void                showContextMenu(const QPoint &pos);
    void                slot_font_settings();
    void                slot_showstatus(const QString &msg, int sec = 10);

    void                slot_set_menu_font_size();
    void                slot_set_content_font_size();
    void                slot_set_statusbar_font_size();
    void                slot_set_wmsg_font_size();

signals:
    void               signal_save_settings();
    void               signal_message(QMessageBox::Icon icon, const QString &msg);

private:
    MAIN                *m_main;
    //QLabel            *m_status_bar_permanent_Label;

    //-----------------------------------------------------------
private:
    int                 m_sys_menu_font_size;       // системный размер фонта меню
    int                 m_sys_statusbar_font_size;  // системный размер фонта строки статуса
    int                 m_sys_content_font_size;    // сист. размер фонта
    int                 m_sys_wmsg_font_size;       // сист. размер фонта сообщений
    QFont               m_sys_content_font;         // системный фонт

    // settings from .ini
private:
    int                 m_fullscreen;
    QSize               m_app_size;
    QPoint              m_app_position;
    int                 m_menu_font_size;       // размер фонта меню
    int                 m_content_font_size;    // размер фонта терминала
    int                 m_statusbar_font_size;  // размер фонта строки статуса
    int                 m_wmsg_font_size;       // размер фонта океа сообщений
};

#endif // _MAINWINDOW_H_
