/*!
  \file   encoder_gui.cpp
  \author Aleksey Lekhtin
  \date   09.11.2005
  \brief  Интерфейс с кнопками для управления декодером ББИ
*/

#include <QtGlobal>
#include <QDebug>

#include "dbg.h"
#include "encoder.h"
#include "main.h"
#include "mainwindow.h"
#include "wmsg.h"


//-----------------------------------------------------------
QWidget *ENCODER::create_gui()
{

    m_terminal = new TERMINAL(m_main->mw());
    QGroupBox *terminal_box = new QGroupBox(tr("Terminal"));
    terminal_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *terminal_layout = new QVBoxLayout(terminal_box);
    terminal_layout->addWidget(m_terminal);

    terminal_box->setCheckable(true);
    terminal_box->setChecked(m_terminal_ini);
    if (m_terminal_ini) {
        m_terminal->show();
    } else {
        m_terminal->hide();
    }

    connect(terminal_box, &QGroupBox::clicked,
            this, [=](bool checked) {
        if (checked) {
            m_terminal->show();
        } else {
            m_terminal->hide();
        }
        m_terminal_ini = checked;
        save_settings();
    }, Qt::QueuedConnection);

    //-------------------------------------------
    QWidget *wgt_comport = create_comport_control();
    QWidget *wgt_fields  = create_enc_fields();

    //-------------------------------------------
    QVBoxLayout *VLayout = new QVBoxLayout();
    VLayout->addWidget(wgt_comport);
    VLayout->addWidget(wgt_fields);
    //VLayout->addStretch();
    VLayout->addWidget(terminal_box);
    //VLayout->addStretch();

    //-------------------------------------------
    QWidget *gui_widget = new QWidget();
    gui_widget->setLayout(VLayout);
    return gui_widget;
}

//-----------------------------------------------------------
QWidget *ENCODER::create_comport_control()
{
    m_comport_connect_button = new QPushButton(tr("Connect"));
    m_comport_status_pict = new QLabel();
    m_comport_status_pict->setPixmap(m_pixmap_gray);

    QHBoxLayout *comport_status_layout = new QHBoxLayout();
    comport_status_layout->addWidget(m_comport_connect_button);
    comport_status_layout->addStretch();
    comport_status_layout->addWidget(m_comport_status_pict);

    //-------------------------------------------
    m_comport_list = new QComboBox();
    m_comport_list->setFocusPolicy(Qt::NoFocus);
    m_comport_list->setMaxVisibleItems(8);
    m_comport_list->setDuplicatesEnabled(false);
    m_comport_list->setMinimumContentsLength(12);
    m_comport_list->setCurrentIndex(-1);

    //-------------------------------------------
    m_comport_refresh_button = new QPushButton();
    m_comport_refresh_button->setIcon(QIcon(":q_refresh.png"));
    m_comport_refresh_button->setShortcut(Qt::Key_F5);
    m_comport_refresh_button->setToolTip(tr("Refresh list of available serial port\nand reconnect immediately"));
    m_comport_refresh_button->setToolTip(MW::make_tip(m_comport_refresh_button->toolTip(),
                                                      m_comport_refresh_button->shortcut()));
    //-------------------------------------------
    m_comport_setup_button = new QPushButton();
    m_comport_setup_button->setIcon(QIcon(":q_settings.png"));
    m_comport_setup_button->setShortcut(Qt::ALT|Qt::Key_S);
    m_comport_setup_button->setToolTip(tr("Serial port settings"));
    m_comport_setup_button->setToolTip(MW::make_tip(m_comport_setup_button->toolTip(),
                                                    m_comport_setup_button->shortcut()));
    //-------------------------------------------
    int nrow = 0;
    QGridLayout *grid = new QGridLayout();
    grid->addWidget(m_comport_refresh_button, nrow, 0);
    grid->addWidget(m_comport_list, nrow, 1);
    ++nrow;
    grid->addWidget(m_comport_setup_button, nrow, 0);
    grid->addLayout(comport_status_layout, nrow, 1);

    QGroupBox *box = new QGroupBox("Comport");
    QHBoxLayout *layout = new QHBoxLayout(box);
    layout->addStretch();
    layout->addLayout(grid);
    layout->addStretch();

    //-------------------------------------------
    QHBoxLayout *HLayout = new QHBoxLayout();
    HLayout->addStretch();
    HLayout->addWidget(box);
    HLayout->addStretch();

    auto wgt = new QWidget();
    wgt->setLayout(HLayout);
    return wgt;
}

//-----------------------------------------------------------
QWidget *ENCODER::create_enc_fields()
{
    QLabel *label_freq = new QLabel(tr("Freq (Hz)"));
    m_freq_field = new QSpinBox();
    m_freq_field->setRange(1, 100000);
    m_freq_field->setSingleStep(1);
    m_freq_field->setValue(m_freq_ini);
    connect(m_freq_field, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        QString txt = QString("%1 sec").arg(1.0/value, 0, 'f', 6, QChar('0'));
        m_freq_clocks->setText(txt);
    });

    QString txt = QString("%1 sec").arg(1.0/m_freq_field->value(), 0, 'f', 6, QChar('0'));
    m_freq_clocks = new QLabel(txt);

    //---------------------------------
    QLabel *label_steps = new QLabel(tr("Steps"));
    m_steps_field = new QSpinBox();
    m_steps_field->setRange(-100000, 10000);
    m_steps_field->setSingleStep(1);
    m_steps_field->setValue(m_steps_ini);

    connect(m_steps_field, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
        DBG(2, QString("steps changed to %1").arg(value));
        m_steps_field->findChild<QLineEdit*>()->deselect();
        m_reverse_button->setChecked(value < 0);
    }, Qt::QueuedConnection);

    //---------------------------------
    m_reverse_button = new QPushButton(tr("Reverse"));
    m_reverse_button->setCheckable(true);
    m_reverse_button->setShortcut(Qt::Key_F6);
    m_reverse_button->setToolTip(tr("Change the direction of rotation"));
    m_reverse_button->setToolTip(MW::make_tip(m_reverse_button->toolTip(),
                                              m_reverse_button->shortcut()));
    m_reverse_button->setChecked(m_steps_field->value() < 0);

    connect(m_reverse_button, &QPushButton::toggled, this, [this](bool checked) {
        DBG(2, "reverse pressed");
        emit mw()->clearstatus();
        auto value = m_steps_field->value();
        if (checked) {
            if (value > 0) {
                m_steps_field->setValue(value * -1);
            }
        } else {
            if (value < 0) {
                m_steps_field->setValue(value * -1);
            }
        }
    });

    //-------------------------------------------
    int nrow = 0;
    QGridLayout *grid_fields = new QGridLayout();
    grid_fields->addWidget(label_freq, nrow, 0);
    grid_fields->addWidget(m_freq_field, nrow, 1);
    grid_fields->addWidget(m_freq_clocks, nrow, 2);
    ++nrow;
    grid_fields->addWidget(label_steps, nrow, 0);
    grid_fields->addWidget(m_steps_field, nrow, 1);
    grid_fields->addWidget(m_reverse_button, nrow, 2);

    QHBoxLayout *fields_layout = new QHBoxLayout();
    fields_layout->addStretch();
    fields_layout->addLayout(grid_fields);
    fields_layout->addStretch();

    //-------------------------------------------
    m_start_button = new QPushButton(tr("Start"));
    m_start_button->setShortcut(Qt::CTRL|Qt::Key_R);
    m_start_button->setToolTip(tr("Start pulse generation"));
    m_start_button->setToolTip(MW::make_tip(m_start_button->toolTip(),
                                            m_start_button->shortcut()));

    m_stop_button = new QPushButton(tr("Stop"));
    m_stop_button->setShortcut(Qt::CTRL|Qt::Key_E);
    m_stop_button->setToolTip(tr("Stop generating pulses"));
    m_stop_button->setToolTip(MW::make_tip(m_stop_button->toolTip(),
                                           m_stop_button->shortcut()));

    QHBoxLayout *buttons_plate = new QHBoxLayout();
    buttons_plate->addStretch();
    buttons_plate->addWidget(m_start_button);
    buttons_plate->addWidget(m_stop_button);
    buttons_plate->addStretch();

    QVBoxLayout *buttons_layout = new QVBoxLayout();
    //buttons_layout->addSpacing(50);
    buttons_layout->addLayout(buttons_plate);

    //-------------------------------------------
    QVBoxLayout *VLayout = new QVBoxLayout();
    //VLayout->addStretch();
    VLayout->addLayout(fields_layout);
    VLayout->addLayout(buttons_layout);
    //VLayout->addStretch();

    QHBoxLayout *HLayout = new QHBoxLayout();
    HLayout->addStretch();
    HLayout->addLayout(VLayout);
    HLayout->addStretch();

    //-------------------------------------------
    QWidget *gui_widget = new QWidget();
    gui_widget->setLayout(HLayout);
    return gui_widget;
}


//-----------------------------------------------------------
void ENCODER::set_comport_status_pixmap(const QPixmap &pixmap)
{
    m_comport_status_pict->setPixmap(pixmap);
    if (pixmap.cacheKey() == m_pixmap_green.cacheKey()) {
        m_comport_connect_button->setText(tr("Disconnect"));
        m_comport_connect_button->setToolTip(tr("Port connected"));
    } else if (pixmap.cacheKey() == m_pixmap_gray.cacheKey()) {
        m_comport_connect_button->setText(tr("Connect"));
        m_comport_connect_button->setToolTip(tr("Port disconnected"));
    } else if (pixmap.cacheKey() == m_pixmap_red.cacheKey()) {
        m_comport_connect_button->setText(tr("Connect"));
        m_comport_connect_button->setToolTip(tr("Error"));
    }
}

//---------------------------------------------------------
void ENCODER::slot_comport_openclose(bool checked)
{
    (void)checked;
    if (m_comport_list->currentIndex() == -1) {
        DBG(1, "comport is not selected");
        return;
    }
    DBG(1);
    if (comm()->comport()->connected()) {
        comm()->disconnect_from(QSerialPort::NoError);
    } else {
        comm()->connect_to();
    }
}

//---------------------------------------------------------
void ENCODER::slot_comport_refresh()
{
    DBG(1);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    emit mw()->clearstatus();

    QString selected_comport_name = m_comport_list->currentText();
    port_t port = comm()->comport()->port_settings();
    comm()->disconnect_from();

    auto prev_signals_status = m_comport_list->blockSignals(true);

    m_comport_list->clear();
    QList<port_name_t> portlist = SERIALCONF::get_port_list();
    foreach(port_name_t port, portlist) {
        m_comport_list->addItem(port.name, port.values);
    }

    port.name = "";
    comm()->comport()->set_port_settings(port);
    m_comport_list->setCurrentIndex(-1);

    int pos = m_comport_list->findText(selected_comport_name);
    if (pos != -1) {
        m_comport_list->setCurrentIndex(pos);
        port.name = selected_comport_name;
        comm()->comport()->set_port_settings(port);
    } else {
        m_comport_list->setCurrentIndex(0);
    }
    DBGF(1, "selected '%s'", qPrintable(m_comport_list->currentText()));

    m_comport_list->blockSignals(prev_signals_status);
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
void ENCODER::slot_comport_list_changed(int index)
{
    (void)index;
    DBG(1, "comport list changed");
    QApplication::setOverrideCursor(Qt::WaitCursor);
    emit mw()->clearstatus();
    comm()->disconnect_from();
    port_t port = comm()->comport()->port_settings();
    port.name = m_comport_list->currentText();
    comm()->comport()->set_port_settings(port);
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
void ENCODER::slot_comport_setup()
{
    DBG(1, "comport setup was selected");
    emit mw()->clearstatus();

    QDialog *dlg = new QDialog(mw());

    const port_t &port = comm()->comport()->port_settings();
    SERIALCONF *conf = new SERIALCONF(dlg);
    conf->set_port_settings(port);

    QWidget *wgt = conf->create_fields_layout();

    //-------------------------------------------
    QDialogButtonBox *buttons_group = new QDialogButtonBox(Qt::Horizontal);
    buttons_group->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel);

    QPushButton *btn = buttons_group->button(QDialogButtonBox::Apply);
    btn->setText(tr("&Apply"));
    connect(btn, SIGNAL(clicked()), dlg, SLOT(accept()));

    btn = buttons_group->button(QDialogButtonBox::Cancel);
    btn->setText(tr("&Cancel"));
    connect(btn, SIGNAL(clicked()), dlg, SLOT(reject()));
    btn->setDefault(true);
    btn->setAutoDefault(true);

    //---------------------------------------------------------
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(wgt);
    layout->addSpacing(10);
    layout->addWidget(buttons_group);

    layout->setAlignment(Qt::AlignHCenter);
    layout->setAlignment(layout, Qt::AlignCenter);
    layout->setAlignment(buttons_group, Qt::AlignCenter);

    //---------------------------------------------------------
    dlg->setLayout(layout);
    auto txt = tr("Setup Serial Port");
    txt = tr("Setup Serial Port");
    dlg->setWindowTitle(txt);
    dlg->setWindowIcon(QIcon(":q_comport.png"));
    //QFont fnt = dlg->font();
    //fnt.setPointSize(m_content_font_size);
    //dlg->setFont(fnt);

    dlg->adjustSize();
    dlg->setMinimumWidth(400);
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    if (dlg->exec() == QDialog::Accepted) {
        conf->flush_fields();
        comm()->comport()->set_port_settings(conf->port_settings());
    }
    delete conf;
}

//---------------------------------------------------------
void ENCODER::slot_comport_connected()
{
    DBG(1, QString("state:%1, status:%2").arg(comm()->state()).arg(comm()->state_string()));
    emit mw()->clearstatus();
    set_comport_status_pixmap(m_pixmap_green);
    const auto &port = comm()->comport()->port_settings();
    const auto msg = QString("%1 %2").arg(COMPORT::port_info_short(port)).arg(tr("Port connected"));
    emit comm()->signal_showstatus(msg, 10);
}

//---------------------------------------------------------
void ENCODER::slot_comport_disconnected(int err)
{
    DBG(1);
    emit mw()->clearstatus();
    if (err) {
        set_comport_status_pixmap(m_pixmap_red);
    } else {
        set_comport_status_pixmap(m_pixmap_gray);
    }
    const auto &port = comm()->comport()->port_settings();
    const auto msg = QString("%1 %2 %3")
            .arg(COMPORT::port_info_short(port))
            .arg(tr("Port disconnected"))
            .arg((comm()->comport()->error() == QSerialPort::ResourceError ? "(Resource Error)" : ""));
    emit comm()->signal_showstatus(msg, 10);
};

//---------------------------------------------------------
void ENCODER::slot_comport_got_error(int err, QString err_string)
{
    (void)err_string;
    if (err == QSerialPort::DeviceNotFoundError) {
        port_t port = comm()->comport()->port_settings();
        if (port.name == m_comport_list->currentText()) {
            DBGF(1, "'%s' DeviceNotFoundError - refresh post's list", qPrintable(m_comport_list->currentText()));

            auto prev_signals_status = m_comport_list->blockSignals(true);
            m_comport_list->clear();
            QList<port_name_t> portlist = SERIALCONF::get_port_list();
            foreach(port_name_t _port, portlist) {
                m_comport_list->addItem(_port.name, _port.values);
            }
            m_comport_list->setCurrentIndex(-1);
            port.name = "";
            comm()->comport()->set_port_settings(port);
            m_comport_list->blockSignals(prev_signals_status);
        }
    }
}
