/*!
  \file   lineedit.h
  \brief  Работа с QLineEdit
  \author Aleksey Lekhtin
  \date   created 25.02.2015
*/

#ifndef _LINE_EDIT_H_
#define _LINE_EDIT_H_

#include <QLineEdit>

//---------------------------------------------------------
class LineEdit30 : public QLineEdit
{
    Q_OBJECT;
public:
    LineEdit30(QWidget *parent = NULL);
    QSize sizeHint() const;
};

//---------------------------------------------------------
class LineEdit22 : public QLineEdit
{
    Q_OBJECT;
public:
    LineEdit22(QWidget *parent = NULL);
    QSize sizeHint() const;
};

//---------------------------------------------------------
class LineEdit20 : public QLineEdit
{
    Q_OBJECT;
public:
    LineEdit20(QWidget *parent = NULL);
    LineEdit20(const QString& text, QWidget *parent = NULL);
    LineEdit20(const QString& text, Qt::Alignment align, QWidget *parent = NULL);
    QSize sizeHint() const;
};

//---------------------------------------------------------
class LineEdit16 : public QLineEdit
{
    Q_OBJECT;
public:
    LineEdit16(QWidget *parent = NULL);
    LineEdit16(const QString& text, QWidget *parent = NULL);
    LineEdit16(const QString& text, Qt::Alignment align, QWidget *parent = NULL);
    QSize sizeHint() const;
};

//---------------------------------------------------------
class LineEdit12 : public QLineEdit
{
    Q_OBJECT;
public:
    LineEdit12(QWidget *parent = NULL);
    LineEdit12(const QString& text, QWidget *parent = NULL);
    LineEdit12(const QString& text, Qt::Alignment align, QWidget *parent = NULL);
    QSize sizeHint() const;
};

//---------------------------------------------------------
class LineEdit8 : public QLineEdit
{
    Q_OBJECT;
public:
    LineEdit8(QWidget *parent = NULL);
    LineEdit8(const QString& text, QWidget *parent = NULL);
    LineEdit8(const QString& text, Qt::Alignment align, QWidget *parent = NULL);
    QSize sizeHint() const;
};

//---------------------------------------------------------
class LineEdit5 : public QLineEdit
{
    Q_OBJECT;
public:
    LineEdit5(QWidget *parent = NULL);
    LineEdit5(const QString& text, QWidget *parent = NULL);
    LineEdit5(const QString& text, Qt::Alignment align, QWidget *parent = NULL);
    QSize sizeHint() const;
};

//---------------------------------------------------------
class LineEdit : public QLineEdit
{
    Q_OBJECT;
public:
    LineEdit(QWidget *parent = NULL);
    QSize sizeHint() const;
};

#endif // _LINE_EDIT_H_
