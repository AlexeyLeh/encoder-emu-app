#ifndef WMSG_H__
#define WMSG_H__

#include <QMessageBox>
#include <QString>

class WMSG
{
public:
    static void WMsg(const QString &msg);
    static void WMsg(const QString &title, const QString &msg);
    static void WMsg(const QString &msg, QMessageBox::Icon icon);
    static void WMsg(QWidget *parent, const QString &msg, QMessageBox::Icon icon = QMessageBox::Information);
    static void WMsg(QWidget *parent, const QString &title, const QString &msg, QMessageBox::Icon icon);

    static const char *secs_to_date_str(uint32_t secs);
};

#endif // WMSG_H__
