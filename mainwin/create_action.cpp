/*!
 * \file   create_action.cpp
 * \author Aleksey Lekhtin
 * \date   created 17.11.2014
 * \brief  Обработчики объектов управления
 */

#include "main.h"
#include "mainwindow.h"

#include "encoder.h"

//---------------------------------------------------------
QString MW::make_tip(QAction *act)
{
    // QString tip = act->toolTip
    QString tip = act->text();
    tip.replace("&", "");
    if (!act->shortcut().toString().isEmpty())
        tip = tip + " [" + act->shortcut().toString() + "]";
    return tip;
}

//---------------------------------------------------------
QString MW::make_tip(const QString &tip, QKeySequence keyseq)
{
    QString _tip = tip;
    _tip.replace("&", "");
    if (!keyseq.toString().isEmpty())
        _tip = _tip + "\n[" + keyseq.toString() + "]";
    return _tip;
}

