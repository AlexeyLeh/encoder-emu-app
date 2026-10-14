/*!
  \file   comm.cpp
  \author Aleksey Lekhtin
  \brief  Коммуникация с устройствами
  \date   10.05.2023
*/


#include <QtGlobal>

#include <sstream>
#include <string>

#include "comm.h"
#include "comport.h"
#include "encoder.h"
#include "main.h"
#include "mainwindow.h"
#include "utils.h"

constexpr int period_50ms = 50; // ms
constexpr int period_100ms = 100;
constexpr int period_200ms = 200;
constexpr int period_500ms = 500;
constexpr int period_2000ms = 2000;

//---------------------------------
void COMM::slot_comm_connected(){
    DBG(1, QString("state:%1, status:%2").arg(state()).arg(state_string()));
};

//---------------------------------
void COMM::slot_comm_disconnected(int err)
{
    if (err) {
        DBGF(1, "got COMPORT::signal_comm_disconnected (err: %d)", err);
    } else {
        DBG(1);
    }
}

//---------------------------------
void COMM::slot_comm_got_error(int err, QString err_string)
{
    (void)err;
    DBG(1, QString("'%1' err:%2 %3").arg(name()).arg(err).arg(err_string));
    if (err == QSerialPort::ResourceError) {
        disconnect_from(err);
        return;
    }
    emit signal_showstatus(err_string, 60);
    emit signal_message(UTILS::split_message(err_string));
}

//---------------------------------
void COMM::slot_comm__not_responded(QString err_string)
{
    DBG(1, QString("%1").arg(err_string));
    emit signal_showstatus(err_string, 60);
    emit signal_message(UTILS::split_message(err_string));
}

