# src.pri

INCLUDEPATH +=  \
    .           \
    encoder/    \
    comm/       \
    dbg/        \
    help/       \
    main/       \
    mainwin/    \
    serialport/ \
    settings/   \
    terminal/   \

HEADERS += \
    $$PWD/comm/comport.h \
    $$PWD/main.h \
    $$PWD/ver.h \
    \
    $$PWD/dbg/dbg.h \
    \
    $$PWD/comm/comm.h \
    \
    $$PWD/encoder/encoder.h \
    \
    $$PWD/main/translator.h \
    $$PWD/main/utils.h \
    \
    $$PWD/mainwin/lineedit.h \
    $$PWD/mainwin/mainwindow.h \
    $$PWD/mainwin/progress.h \
    $$PWD/mainwin/wmsg.h \
    \
    $$PWD/serialport/serial_conf.h \
    \
    $$PWD/terminal/terminal.h


SOURCES += \
    $$PWD/comm/comm_slots.cpp \
    $$PWD/comm/comport.cpp \
    $$PWD/encoder/encoder_gui.cpp \
    $$PWD/main.cpp \
    \
    $$PWD/comm/comm.cpp \
    \
    $$PWD/dbg/dbg.cpp \
    \
    $$PWD/encoder/encoder.cpp \
    \
    $$PWD/main/help.cpp \
    $$PWD/main/ini.cpp \
    $$PWD/main/translator.cpp \
    $$PWD/main/utils.cpp \
    $$PWD/main/ver.cpp \
    \
    $$PWD/mainwin/create_action.cpp\
    $$PWD/mainwin/create_menu.cpp\
    $$PWD/mainwin/lineedit.cpp\
    $$PWD/mainwin/mainwindow.cpp\
    $$PWD/mainwin/progress.cpp\
    $$PWD/mainwin/slot_fonts.cpp\
    $$PWD/mainwin/wmsg.cpp \
    \
    $$PWD/serialport/serial_conf.cpp \
    \
    $$PWD/terminal/terminal.cpp \

FORMS +=
