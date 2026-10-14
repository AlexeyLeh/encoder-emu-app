/*!
 * \file   create_menu.cpp
 * \author Aleksey Lekhtin
 * \date   created 17.11.2014
 * \brief  Создание меню
 */

#include <QMenuBar>

#include "dbg.h"
#include "mainwindow.h"

//---------------------------------------------------------
void MW::create_menu()
{
    get_fonts_size();
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showContextMenu(const QPoint&)));
}

//---------------------------------------------------------
void MW::showContextMenu(const QPoint &pos)
{
    DBG(4);
    QMenu *contextMenu = new QMenu("Context menu", this);
    QAction action_fonts("Font Settings", this);
    QAction action_about("About", this);

    QFont fnt;
    fnt.setPointSize(m_content_font_size);
    action_fonts.setFont(fnt);
    action_about.setFont(fnt);

    // Connect actions to your slots
    connect(&action_fonts, SIGNAL(triggered()), this, SLOT(slot_font_settings()));
    connect(&action_about, SIGNAL(triggered()), this, SLOT(slot_about()));

    contextMenu->addAction(&action_fonts);
    contextMenu->addAction(&action_about);

    // Convert widget coordinates to global screen coordinates
    contextMenu->exec(mapToGlobal(pos));

    //contextMenu->popup(QCursor::pos());

    delete contextMenu;
}
