/*!
  \file   encoder.h
  \author Aleksey Lekhtin
  \date   09.11.2005
  \brief  Коммуникация с декодером ББИ
*/

#ifndef __ENCODER_H_
#define __ENCODER_H_

#include <mutex>

#include <QComboBox>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QToolButton>

#include "terminal.h"

class COMM;
class MAIN;
class MW;

class ENCODER : public QObject
{
    Q_OBJECT;

public:
    ENCODER(MAIN *m);
    ~ENCODER();

public:
    void            run();
    MW              *mw() const;
    COMM            *comm() const;
    QWidget         *create_gui();
    void            set_comport_status_pixmap(const QPixmap &pixmap);

public:
    void            read_settings();
    void            save_settings();
    void            comport_connected();
    void            comport_disconnected();

private:
    QWidget         *create_enc_fields();
    QWidget         *create_comport_control();

private slots:
    void            slot_comport_refresh();
    void            slot_comport_setup();
    void            slot_comport_list_changed(int index);
    void            slot_comport_openclose(bool checked);
    void            slot_comport_connected();
    void            slot_comport_disconnected(int err);
    void            slot_comport_got_error(int err, QString err_string);


signals:

private:
    // хранитель глобальных ресурсов программы
    MAIN            *m_main;

    // элементы отображения информации и управления
public:
    QPixmap         m_pixmap_gray;
    QPixmap         m_pixmap_red;
    QPixmap         m_pixmap_green;
    QLabel          *m_comport_status_pict;
    QPushButton     *m_comport_connect_button;
    QComboBox       *m_comport_list;
    QSpinBox        *m_freq_field;
    QLabel          *m_freq_clocks;
    QSpinBox        *m_steps_field;
    QPushButton     *m_reverse_button;
    QPushButton     *m_start_button;
    QPushButton     *m_stop_button;
    QPushButton     *m_comport_refresh_button;
    QPushButton     *m_comport_setup_button;
    TERMINAL        *m_terminal;

    // settings from .ini
public:
    int             m_freq_ini;
    int             m_steps_ini;
    bool            m_terminal_ini;
};

#endif // __ENCODER_H_
