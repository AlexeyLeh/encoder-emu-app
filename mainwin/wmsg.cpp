/*!
  \file   wmsg.cpp
  \author Aleksey Lekhtin
  \date   27.11.2024
  \brief  Window with message
*/

#include <QGridLayout>
#include <utils.h>

#include "cfg.h"
#include "dbg.h"
#include "mainwindow.h"
#include "wmsg.h"

void WMSG::WMsg(const QString &msg)
{
    WMsg(Q_NULLPTR, QString(), msg, QMessageBox::Information);
}

void WMSG::WMsg(const QString &title, const QString &msg)
{
    WMsg(Q_NULLPTR, title, msg, QMessageBox::Information);
}

void WMSG::WMsg(const QString &msg, QMessageBox::Icon icon)
{
    WMsg(Q_NULLPTR, QString(), msg, icon);
}

void WMSG::WMsg(QWidget *parent, const QString &msg, QMessageBox::Icon icon)
{
    WMsg(parent, QString(), msg, icon);
}

void WMSG::WMsg(QWidget *parent, const QString &title, const QString &msg, QMessageBox::Icon icon)
{
    if (DBG::debug_level >= 3) {
        QString bf = msg;
        DBG(3, bf.replace('\n', ';'));
    }
    int ww = 0;
    auto *msg_box = new QMessageBox(parent);
    msg_box->setIcon(icon);
    msg_box->setStandardButtons(QMessageBox::Close);
    msg_box->setDefaultButton(QMessageBox::Close);
    if (!title.isEmpty()) msg_box->setWindowTitle(title);

    int pos = msg.indexOf("<html>");
    if (pos == -1) {
        QStringList lst = msg.split("\n");
        if (lst.size() > 0) {
            int ln = 0;
            int idx = 0;
            for(int i = 0; i < lst.size(); ++i) {
                if (lst.at(i).length() > ln) {
                    ln = lst.at(i).length();
                    idx = i;
                }
            }
            QFontMetrics fm = msg_box->fontMetrics();
            ww = fm.horizontalAdvance(lst.at(idx)) + 50;
        }
        msg_box->setText(msg);
    } else {
        QString msg1 = msg.left(pos);
        QString msg2;
        int pos2 = msg.indexOf("</html>");
        if (pos == -1) {
            msg2 = msg.mid(pos) + "</html>";
        } else {
            int num = pos2 - pos + 7;
            msg2 = msg.mid(pos, num);
            const QString remainstr = msg.mid(pos + num);
            if (!remainstr.isEmpty()) {
                msg1 += "\n" + remainstr;
            }
        }
        msg_box->setText(msg1);
        msg_box->setInformativeText(msg2);
    }

    auto *layout = (QGridLayout*)msg_box->layout();
    ww = (ww < 350 ? 350 : ww);
    ww = (ww > 800 ? 800 : ww);
    msg_box->adjustSize();
    layout->setColumnMinimumWidth(2, ww);
    msg_box->exec();
    delete msg_box;
}
