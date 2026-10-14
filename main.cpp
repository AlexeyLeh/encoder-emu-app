/*!
 * \file   main.cpp
 * \author Aleksey Lekhtin
 * \date   07.11.2025
 * \brief  Начало.
 */

#include <QObject>
#include <QShortcut>

#include "dbg.h"
#include "comm.h"
#include "encoder.h"
#include "main.h"
#include "mainwindow.h"
#include "translator.h"
#include "utils.h"
#include "wmsg.h"
#include "ver.h"

//---------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":q_app.png"));
    MAIN Main;
    Main.show();
    int rc = a.exec();
    return rc;
}

//---------------------------------------------------------
MAIN::MAIN() :
    m_mw(0),
    m_sett(0),
    m_comm(0),
    m_enc(0),
    m_tr(new TRANSLATOR()),
    qt_translator(new QTranslator()),
    app_translator(new QTranslator())
{
    m_app_title = tr("Encoder Emulator");
    if (!parse_arguments())
    {
        exit(1);
        return;
    }
    m_tr->install(qt_translator, app_translator);
    read_settings();

    //---------------------------------
#ifdef _WIN32
    DBG(1, QString("GetConsoleOutputCP(): %1   (65001 => UTF8)").arg(GetConsoleOutputCP()));
    DBG(1, QString("SetConsoleOutputCP(%2)").arg(m_settings_console_output_cp));
    SetConsoleOutputCP(m_settings_console_output_cp);   // CP_UTF8
    DBG(1, QString("GetConsoleOutputCP(): %1").arg(GetConsoleOutputCP()));
    set_encoding(m_settings_console_output_cp);
#endif

    //---------------------------------
    m_comm  = new COMM(this);
    m_enc   = new ENCODER(this);
    m_mw    = new MW(this);

    //---------------------------------
    m_app_title = tr("Encoder Emulator");
    m_mw->setWindowTitle(m_app_title);

    //---------------------------------
    auto shortcut = new QShortcut(QKeySequence(Qt::CTRL|Qt::Key_Q), m_mw);
    connect(shortcut, &QShortcut::activated, this, []() {
        DBG(3, "QShortcut(Ctrl+q)");
        qApp->closeAllWindows();
    });

    //---------------------------------
    connect(m_comm, &COMM::signal_showstatus, this, [mw = m_mw](const QString &msg, int sec) {
        mw->showstatus(UTILS::split_message(msg), sec);
    }, Qt::QueuedConnection);

    connect(m_comm, &COMM::signal_message, this, [mw = m_mw](const QString &msg) {
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        WMSG::WMsg(mw, UTILS::split_message(msg), QMessageBox::Information);
        QApplication::restoreOverrideCursor();
    }, Qt::QueuedConnection);

    connect(m_mw, &MW::signal_message, this, [mw = m_mw](QMessageBox::Icon icon, const QString &msg) {
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        WMSG::WMsg(mw, UTILS::split_message(msg), icon);
        QApplication::restoreOverrideCursor();
    }, Qt::QueuedConnection);

    connect(m_mw, &MW::signal_save_settings, this, [=]() {
        save_settings();
    }, Qt::QueuedConnection);

    //---------------------------------
    m_enc->run();
    m_mw->run();
    //QTimer::singleShot(100, m_bto_gui, &BTO_GUI::run);
}

//---------------------------------------------------------
MAIN::~MAIN()
{
    DBG(4, "begin");
    save_settings();

    delete m_comm;
    delete m_enc;
    delete m_mw;
    delete m_sett;

    delete qt_translator;
    delete app_translator;
    delete m_tr;

    if (DBG::QDebug_QFile.isOpen()) {
        DBG::QDebug_QFile.close();
    }
    DBG(3, "end");
}

//-----------------------------------------------------------
TRANSLATOR *MAIN::tr()
{
    return m_tr;
}

//-----------------------------------------------------------
bool MAIN::parse_arguments()
{
    m_version = QString("Ver. %1 %2\n(built %3 %4)\n\n"
                        "built with Qt %5\n"
                        "run-time Qt %6")
            .arg(VER_NUM, VER_DATE, __DATE__, __TIME__, QT_VERSION_STR)
            .arg(qVersion());

    m_version_short = QString("Ver. %1 %2").arg(VER_NUM, VER_DATE);

    QStringList argv = QCoreApplication::arguments();
    if (argv.contains("-h",     Qt::CaseInsensitive) ||
            argv.contains("--help", Qt::CaseInsensitive) ||
            argv.contains("/?",     Qt::CaseInsensitive) ||
            argv.contains("/h",     Qt::CaseInsensitive))
    {
        print_help();
        return false; // закончить работу программы
    }

    if (argv.contains("-v", Qt::CaseInsensitive) ||
            argv.contains("--version", Qt::CaseInsensitive) ||
            argv.contains("/v", Qt::CaseInsensitive))
    {
        print_version();
        return false; // закончить работу программы
    }

    int pos = argv.indexOf("-d");
    if (pos != -1 && pos < argv.size()-1 && !argv[pos+1].startsWith("-"))
    {
        ++pos;
        DBG::debug_level = argv[pos].toInt();
        DBG(0, QString("dbg: %1").arg(DBG::debug_level));
    }

    pos = argv.indexOf("--log");
    if (pos != -1) {
        if (pos < argv.size()-1 && !argv[pos+1].startsWith("-")) {
            ++pos;
            DBG::QDebugOutputFile = argv.at(pos);
        } else {
            DBG::QDebugOutputFile = qApp->applicationDirPath() + "/" + QApplication::applicationName();
        }
    }

    //
    if (!DBG::QDebugOutputFile.isEmpty()) {
        QFileInfo fi(DBG::QDebugOutputFile);
        QString ext = fi.suffix();
        if (ext.isEmpty()) {
            if (DBG::QDebugOutputFile.endsWith(".")) {
                DBG::QDebugOutputFile += "log";
            } else {
                DBG::QDebugOutputFile += ".log";
            }
        }

        QDir dir = fi.dir();
        DBG(0, QString("expected log directory: %1").arg(QDir::toNativeSeparators(fi.dir().absolutePath())));
        if (!dir.exists())  {
            DBG(0, "directory does not exists");
            DBG::QDebugOutputFile = QString("%1/%2").arg(qApp->applicationDirPath()).arg(fi.fileName());
        }

        DBG::QDebugOutputFile = QDir::toNativeSeparators(DBG::QDebugOutputFile);
        DBG(0, QString("log: %1").arg(DBG::QDebugOutputFile));

        DBG::QDebug_QFile.setFileName(DBG::QDebugOutputFile);
        DBG::QDebug_QFile.open(QIODevice::WriteOnly /*| QIODevice::Append*/);
        DBG::debugTextStream.setDevice(&DBG::QDebug_QFile);
    }

    return true;
}

//---------------------------------------------------------
void MAIN::show()
{
    mw()->show();
}

//---------------------------------------------------------
void MAIN::set_encoding(int enc)
{
    if (!DBG::debugTextStream.device()) {
        return;
    }

    if (enc == 1251)
    {
        DBG::debugTextStream.setCodec(QTextCodec::codecForLocale());
    }
    else if (enc == 65001)
    {
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        DBG::debugTextStream.setCodec(codec);
    }
}

//---------------------------------------------------------
void MAIN::main_quit()
{
    DBG(3);
    QApplication::closeAllWindows();
}

//---------------------------------------------------------
const QString MAIN::root_path() const
{
    return QCoreApplication::applicationDirPath();
}

//---------------------------------------------------------
MW *MAIN::mw() const
{
    return m_mw;
}

//---------------------------------------------------------
COMM *MAIN::comm() const
{
    return m_comm;
}

//---------------------------------------------------------
ENCODER *MAIN::enc() const
{
    return m_enc;
}

