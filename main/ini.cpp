/*!
 * \file   ini.cpp
 * \author Aleksey Lekhtin
 * \date   created 06.11.2014
 * \brief  Параметры программы
 */

#include "comm.h"
#include "dbg.h"
#include "main.h"
#include "mainwindow.h"

//---------------------------------------------------------
// Open settings file
QSettings *MAIN::open_settings_file()
{
    auto fname = QString("%1/%2.ini")
            .arg(QCoreApplication::applicationDirPath())
            .arg(QCoreApplication::applicationName());
    fname = QDir::toNativeSeparators(fname);
    DBG(3, fname);
    m_sett = new QSettings(fname, QSettings::IniFormat, 0);
    if (m_sett->status() == QSettings::NoError) return m_sett;
    QString msg = QString("%1 %2\n").arg(tr("can't read settings file"), fname);
    DBG(0, msg);
    return nullptr;
}

//---------------------------------------------------------
QSettings *MAIN::qSettings() const
{
    return m_sett;
}

//---------------------------------------------------------
// Считать текущие настройки из файла
void MAIN::read_settings()
{
    DBG(3);
    open_settings_file();
    if (!m_sett) {
        QString msg = tr("Can't open application status file");
        qApp->setOverrideCursor(Qt::ArrowCursor);
        QMessageBox::critical(nullptr, qApp->applicationName(), msg, QMessageBox::Close);
        return;
    }

#ifdef _WIN32
    m_settings_console_output_cp = m_sett->value("ConsoleOutputCP", 1251).toInt();
#endif
    if (DBG::debug_level == 0) {
        // 0 => в командной строке не задавался
        DBG::debug_level = m_sett->value("DebugLevel", 3).toUInt();
        DBG(0, QString("dbg: %1").arg(DBG::debug_level));
    }
}

//---------------------------------------------------------
// Сохранить настройки
void MAIN::save_settings()
{
    DBG(3);
    // [General] section
#ifdef _WIN32
    qSettings()->setValue("ConsoleOutputCP", m_settings_console_output_cp);
#endif
    qSettings()->setValue("DebugLevel", DBG::debug_level);
}
