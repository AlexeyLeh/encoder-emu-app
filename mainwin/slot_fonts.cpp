/*!
  \file   slot_fonts.cpp
  \brief  Слоты приложения
  \author Aleksey Lekhtin
  \date   30.03.2010 \n
*/

#include <QtCharts/QChartView>

#include "main.h"
#include "mainwindow.h"

//-----------------------------------------------------------
int MW::content_font_pointSize() const
{
    return m_content_font_size;
}

//-----------------------------------------------------------
void MW::get_fonts_size()
{
    // получение текущих "системных" размеров иконок и фонтов
    //QIcon ic(":q_exit.png");
    //QSize sz = ic.actualSize(QSize(24,24), QIcon::Selected, QIcon::On);

    QFont fnt = font();
    m_sys_content_font_size = fnt.pointSize();
    m_sys_content_font = fnt;

    QMenu mn;
    fnt = mn.font();
    m_sys_menu_font_size = fnt.pointSize();

    fnt = statusBar()->font();
    m_sys_statusbar_font_size = fnt.pointSize();

    QMessageBox mbox;
    fnt = mbox.font();
    m_sys_wmsg_font_size = fnt.pointSize();
}

//-----------------------------------------------------------
void MW::slot_font_settings()
{
    int sz;
    QFont fnt;
    auto fontdlg = new QDialog(this);
    fontdlg->setWindowTitle(tr("Setting the font size"));

    //-----------------------------------------------------------
    QLabel *font_menu_label = new QLabel(tr("Menu Font"));
    QComboBox *font_menu_sizes = new QComboBox();
    font_menu_sizes->addItem(tr("System"));
    font_menu_sizes->addItem("8");
    font_menu_sizes->addItem("10");
    font_menu_sizes->addItem("12");
    font_menu_sizes->addItem("14");
    font_menu_sizes->addItem("16");

    if(m_menu_font_size == 8)
        font_menu_sizes->setCurrentIndex(1);
    else if(m_menu_font_size == 10)
        font_menu_sizes->setCurrentIndex(2);
    else if(m_menu_font_size == 12)
        font_menu_sizes->setCurrentIndex(3);
    else if(m_menu_font_size == 14)
        font_menu_sizes->setCurrentIndex(4);
    else if(m_menu_font_size == 16)
        font_menu_sizes->setCurrentIndex(5);

    sz = m_menu_font_size;
    if(sz == 0) sz = m_sys_menu_font_size;
    fnt = font_menu_label->font();
    fnt.setPointSize(sz);
    font_menu_label->setFont(fnt);

    fnt = font_menu_sizes->font();
    fnt.setPointSize(sz);
    font_menu_sizes->setFont(fnt);

    connect(font_menu_sizes, &QComboBox::currentTextChanged, this, [=](const QString &str) {
        auto list = str.split("x");
        m_menu_font_size = list.at(0).toUInt();
        slot_set_menu_font_size();
        emit signal_save_settings();
    });

    //-----------------------------------------------------------
    QLabel *font_content_label = new QLabel(tr("Content Font"));
    QComboBox *font_content_sizes = new QComboBox();
    font_content_sizes->addItem(tr("System"));
    font_content_sizes->addItem("8");
    font_content_sizes->addItem("10");
    font_content_sizes->addItem("12");
    font_content_sizes->addItem("14");
    font_content_sizes->addItem("16");

    if(m_content_font_size == 8)
        font_content_sizes->setCurrentIndex(1);
    else if(m_content_font_size == 10)
        font_content_sizes->setCurrentIndex(2);
    else if(m_content_font_size == 12)
        font_content_sizes->setCurrentIndex(3);
    else if(m_content_font_size == 14)
        font_content_sizes->setCurrentIndex(4);
    else if(m_content_font_size == 16)
        font_content_sizes->setCurrentIndex(5);

    sz = m_menu_font_size;
    if(sz == 0) sz = m_sys_menu_font_size;
    fnt = font_content_label->font();
    fnt.setPointSize(sz);
    font_content_label->setFont(fnt);

    fnt = font_content_sizes->font();
    fnt.setPointSize(sz);
    font_content_sizes->setFont(fnt);

    connect(font_content_sizes, &QComboBox::currentTextChanged, this, [=](const QString &str) {
        auto list = str.split("x");
        m_content_font_size = list.at(0).toUInt();
        slot_set_content_font_size();
        emit signal_save_settings();
    });

    //-----------------------------------------------------------
    QLabel *font_wmsg_label = new QLabel(tr("Message Font"));
    QComboBox *font_wmsg_sizes = new QComboBox();
    font_wmsg_sizes->addItem(tr("System"));
    font_wmsg_sizes->addItem("8");
    font_wmsg_sizes->addItem("10");
    font_wmsg_sizes->addItem("12");
    font_wmsg_sizes->addItem("14");
    font_wmsg_sizes->addItem("16");

    if(m_wmsg_font_size == 8)
        font_wmsg_sizes->setCurrentIndex(1);
    else if(m_wmsg_font_size == 10)
        font_wmsg_sizes->setCurrentIndex(2);
    else if(m_wmsg_font_size == 12)
        font_wmsg_sizes->setCurrentIndex(3);
    else if(m_wmsg_font_size == 14)
        font_wmsg_sizes->setCurrentIndex(4);
    else if(m_wmsg_font_size == 16)
        font_wmsg_sizes->setCurrentIndex(5);

    connect(font_wmsg_sizes, &QComboBox::currentTextChanged, this, [=](const QString &str) {
        auto list = str.split("x");
        m_wmsg_font_size = list.at(0).toUInt();
        slot_set_wmsg_font_size();
        emit signal_save_settings();
    });

    //-----------------------------------------------------------
    QLabel *font_statusbar_label = new QLabel(tr("Status Bar Font"));
    QComboBox *font_statusbar_sizes = new QComboBox();
    font_statusbar_sizes->addItem(tr("System"));
    font_statusbar_sizes->addItem("8");
    font_statusbar_sizes->addItem("10");
    font_statusbar_sizes->addItem("12");
    font_statusbar_sizes->addItem("14");
    font_statusbar_sizes->addItem("16");

    if(m_statusbar_font_size == 8)
        font_statusbar_sizes->setCurrentIndex(1);
    else if(m_statusbar_font_size == 10)
        font_statusbar_sizes->setCurrentIndex(2);
    else if(m_statusbar_font_size == 12)
        font_statusbar_sizes->setCurrentIndex(3);
    else if(m_statusbar_font_size == 14)
        font_statusbar_sizes->setCurrentIndex(4);
    else if(m_statusbar_font_size == 16)
        font_statusbar_sizes->setCurrentIndex(5);

    sz = m_menu_font_size;
    if(sz == 0) sz = m_sys_menu_font_size;
    fnt = font_statusbar_label->font();
    fnt.setPointSize(sz);
    font_statusbar_label->setFont(fnt);

    fnt = font_statusbar_sizes->font();
    fnt.setPointSize(sz);
    font_statusbar_sizes->setFont(fnt);

    connect(font_statusbar_sizes, &QComboBox::currentTextChanged, this, [=](const QString &str) {
        auto list = str.split("x");
        m_statusbar_font_size = list.at(0).toUInt();
        slot_set_statusbar_font_size();
        showstatus(tr("Status bar font changed"));
        emit signal_save_settings();
    });

    //-----------------------------------------------------------
    sz = m_menu_font_size;
    if(sz == 0) sz = m_sys_menu_font_size;

    QLabel label;
    fnt = label.font();
    fnt.setPointSize(sz);
    font_menu_label->setFont(fnt);
    font_content_label->setFont(fnt);
    font_statusbar_label->setFont(fnt);
    font_wmsg_label->setFont(fnt);

    QComboBox qbox;
    fnt = qbox.font();
    fnt.setPointSize(sz);
    font_menu_sizes->setFont(fnt);
    font_content_sizes->setFont(fnt);
    font_statusbar_sizes->setFont(fnt);
    font_wmsg_sizes->setFont(fnt);

    //-----------------------------------------------------------
    QDialogButtonBox *buttons_group = new QDialogButtonBox(Qt::Horizontal);
    buttons_group->setStandardButtons(QDialogButtonBox::Close);
    buttons_group->setMinimumSize(300, 24);

    QPushButton *btn = buttons_group->button(QDialogButtonBox::Close);
    btn->setText(QCoreApplication::tr("Close"));
    btn->setDefault(true);
    btn->setAutoDefault(true);

    connect(buttons_group, SIGNAL(accepted()), fontdlg, SLOT(accept()));
    connect(buttons_group, SIGNAL(rejected()), fontdlg, SLOT(reject()));

    //-----------------------------------------------------------
    int nrow = 0;
    QGridLayout *grid = new QGridLayout();
    grid->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    grid->addWidget(font_menu_label,    nrow, 0);
    grid->addWidget(font_menu_sizes,    nrow, 1);
    ++nrow;
    grid->addWidget(font_content_label, nrow, 0);
    grid->addWidget(font_content_sizes, nrow, 1);
    ++nrow;
    grid->addWidget(font_wmsg_label,    nrow, 0);
    grid->addWidget(font_wmsg_sizes,    nrow, 1);
    ++nrow;
    grid->addWidget(font_statusbar_label, nrow, 0);
    grid->addWidget(font_statusbar_sizes, nrow, 1);

    //-----------------------------------------------------------
    QVBoxLayout *mainLayout = new QVBoxLayout();
    //mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addLayout(grid);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(buttons_group);

    mainLayout->setAlignment(Qt::AlignHCenter);
    mainLayout->setAlignment(grid, Qt::AlignCenter);
    mainLayout->setAlignment(buttons_group, Qt::AlignCenter);
    fontdlg->setLayout(mainLayout);

    //-----------------------------------------------------------
    fnt = btn->font();
    fnt.setPointSize(sz);
    btn->setFont(fnt);

    fontdlg->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    fontdlg->adjustSize();
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    fontdlg->exec();
    delete fontdlg;
    fontdlg = NULL;
}

//-----------------------------------------------------------
void MW::slot_set_menu_font_size()
{
    int sz = m_menu_font_size;
    if(sz != 8 && sz != 10 && sz != 12 && sz != 14 && sz != 16) sz = 0;
    if(sz == 0) sz = m_sys_menu_font_size;
    m_menu_font_size = sz;
    QFont fnt = menuBar()->font();
    fnt.setPointSize(sz);
    menuBar()->setFont(fnt);
}

//-----------------------------------------------------------
void MW::slot_set_content_font_size()
{
    int sz = m_content_font_size;
    QString mono_font;
#ifdef MONO_FONT_FAMILY
    mono_font = MONO_FONT_FAMILY;
#endif
    if(sz != 8 && sz != 10 && sz != 12 && sz != 14 && sz != 16) sz = 0;
    if(sz == 0) sz = m_sys_content_font_size;
    m_content_font_size = sz;
    QFont fnt = this->font();
    fnt.setPointSize(sz);
    this->setFont(fnt);
}

//-----------------------------------------------------------
void MW::slot_set_wmsg_font_size()
{
    int sz = m_wmsg_font_size;
    if(sz != 8 && sz != 10 && sz != 12 && sz != 14 && sz != 16) sz = 0;
    if(sz == 0) sz = m_sys_wmsg_font_size;
    m_wmsg_font_size = sz;

    // QString c_style =
    //    QString("QMessageBox { font: %1pt; }\n"
    //             "QPushButton { font: %2pt; }  ")
    //    .arg(sz).arg(sz);

    QString c_style =  QString("QMessageBox { font: %1pt; }\n").arg(sz);
    //"QPushButton { max-height: 8px; }\n"
    //qDebug() << c_style;
    qApp->setStyleSheet(c_style);
}

//-----------------------------------------------------------
void MW::slot_set_statusbar_font_size()
{
    int sz = m_statusbar_font_size;
    if(sz != 8 && sz != 10 && sz != 12 && sz != 14 && sz != 16) sz = 0;
    if(sz == 0) sz = m_sys_statusbar_font_size;
    m_statusbar_font_size = sz;
    QFont fnt = statusBar()->font();
    fnt.setPointSize(sz);
    statusBar()->setFont(fnt);
    //m_status_bar_permanent_Label->setFont(fnt);
}
