/*!
  \file   main.h
  \author Aleksey Lekhtin
  \date   21.05.2025
  \brief  Главный объект
*/

#ifndef _MAIN_H_
#define _MAIN_H_

#include <QtWidgets>

#include "comm.h"
#include "dbg.h"

class COMM;
class ENCODER;
class MW;
class TRANSLATOR;

class MAIN : public QObject
{
    Q_OBJECT;

public:
    explicit MAIN();
    ~MAIN();


    void                show();
    void                main_quit();
    const QString       root_path() const;
    const QString       app_title() const;
    const QString       version() const;
    const QString       version_short() const;
    QSettings           *qSettings() const;
    TRANSLATOR          *tr();

    MW                  *mw() const;
    COMM                *comm() const;
    ENCODER              *enc() const;

private:
    bool                parse_arguments();
    void                print_help();
    void                print_version();
    void                read_settings();
    void                save_settings();
    QSettings           *open_settings_file();
    void                set_encoding(int enc);

    //--- variables -----------------------------

private:
    MW                  *m_mw;
    QSettings           *m_sett;
    COMM                *m_comm;
    ENCODER              *m_enc;

    QString             m_app_title;                // название приложения
    QString             m_version;                  // о программе
    QString             m_version_short;            // о программе (сокращенно)

    TRANSLATOR          *m_tr;
    QTranslator         *qt_translator;             // translator sybsystem for QT
    QTranslator         *app_translator;            // translator sybsystem for application

private:
    //--- settings from .ini --------------------
    int                 m_settings_console_output_cp;
};

#endif // _MAIN_H_
