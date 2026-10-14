/*!
  \file   serial_conf.cpp
  \brief  Serial port settings form
  \author Aleksey Lekhtin
  \date   created 17.11.2014
*/

#include <QApplication>
#include <QDebug>
#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>

#include "dbg.h"
#include "serial_conf.h"

//-----------------------------------------------------------
SERIALCONF::SERIALCONF(QWidget *parent) : QWidget(parent), m_port()
{
    m_intValidator = new QIntValidator(0, 4000000, this);
}

//-----------------------------------------------------------
SERIALCONF::~SERIALCONF()
{
    DBG(4);
}

//-----------------------------------------------------------
QWidget *SERIALCONF::create_fields_layout()
{
    m_parametersBox = new QGroupBox(SERIALCONF::tr("Parameters"), this);
    m_baudRateLabel = new QLabel(SERIALCONF::tr("BaudRate:"),    m_parametersBox);
    m_dataBitsLabel = new QLabel(SERIALCONF::tr("Data bits:"),   m_parametersBox);
    m_parityLabel   = new QLabel(SERIALCONF::tr("Parity:"),      m_parametersBox);
    m_stopBitsLabel = new QLabel(SERIALCONF::tr("Stop bits:"),   m_parametersBox);
    m_flowCtrlLabel = new QLabel(SERIALCONF::tr("Flow control:"),m_parametersBox);

    m_baudRateBox = new QComboBox(m_parametersBox);
    m_dataBitsBox = new QComboBox(m_parametersBox);
    m_parityBox   = new QComboBox(m_parametersBox);
    m_stopBitsBox = new QComboBox(m_parametersBox);
    m_flowCtrlBox = new QComboBox(m_parametersBox);
    m_baudRateBox->setInsertPolicy(QComboBox::NoInsert);
    set_form_values();

    m_baudRateBox->setDisabled(false);
    m_dataBitsBox->setDisabled(true);
    m_stopBitsBox->setDisabled(true);
    m_parityBox->setDisabled(true);
    m_flowCtrlBox->setDisabled(true);

    connect(m_baudRateBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_checkCustomBaudRatePolicy(int)));

    update_form_fields();

    //--- раскладка ---------------------------------------------
    QGridLayout *gridLayout_2 = new QGridLayout(m_parametersBox);
    gridLayout_2->setAlignment(Qt::AlignCenter);
    gridLayout_2->addWidget(m_baudRateLabel,   0, 0, 1, 1);
    gridLayout_2->addWidget(m_baudRateBox,     0, 1, 1, 1);
    gridLayout_2->addWidget(m_dataBitsLabel,   1, 0, 1, 1);
    gridLayout_2->addWidget(m_dataBitsBox,     1, 1, 1, 1);
    gridLayout_2->addWidget(m_stopBitsLabel,   2, 0, 1, 1);
    gridLayout_2->addWidget(m_stopBitsBox,     2, 1, 1, 1);
    gridLayout_2->addWidget(m_parityLabel,     3, 0, 1, 1);
    gridLayout_2->addWidget(m_parityBox,       3, 1, 1, 1);
    gridLayout_2->addWidget(m_flowCtrlLabel,   4, 0, 1, 1);
    gridLayout_2->addWidget(m_flowCtrlBox,     4, 1, 1, 1);

    //-----------------------------------------------------------
    QGridLayout *mainlayout = new QGridLayout(this);
    mainlayout->addWidget(m_parametersBox,  0, 1, 1, 1, Qt::AlignTop);
    mainlayout->setRowStretch(3,100);

    QWidget *wgt = new QWidget();
    wgt->setLayout(mainlayout);
    return wgt;
}

//-----------------------------------------------------------
void SERIALCONF::set_form_values()
{
    m_baudRateBox->addItem(QStringLiteral("1200"),  QSerialPort::Baud1200 ); // 1200
    m_baudRateBox->addItem(QStringLiteral("2400"),  QSerialPort::Baud2400 ); // 2400
    m_baudRateBox->addItem(QStringLiteral("4800"),  QSerialPort::Baud4800 ); // 4800
    m_baudRateBox->addItem(QStringLiteral("9600"),  QSerialPort::Baud9600 ); // 9600
    m_baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200); // 19200
    m_baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400); // 38400
    m_baudRateBox->addItem(QStringLiteral("115200"),QSerialPort::Baud115200); // 115200
    m_baudRateBox->addItem(QStringLiteral("Custom"));
    m_baudRateBox->setCurrentIndex(3);

    //-----------------------------------------------------------
    m_dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5); // 5
    m_dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6); // 6
    m_dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7); // 7
    m_dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8); // 8
    m_dataBitsBox->addItem(QStringLiteral("9"), 9                 ); // 9
    m_dataBitsBox->setCurrentIndex(3);

    //-----------------------------------------------------------
    m_parityBox->addItem(QStringLiteral("None"), QSerialPort::NoParity  );      // 0
    m_parityBox->addItem(QStringLiteral("Even"), QSerialPort::EvenParity);      // 2
    m_parityBox->addItem(QStringLiteral("Odd"),  QSerialPort::OddParity );      // 3
    m_parityBox->addItem(QStringLiteral("Space"),QSerialPort::SpaceParity);     // 4
    m_parityBox->addItem(QStringLiteral("Mark"), QSerialPort::MarkParity);      // 5
    m_parityBox->addItem(QStringLiteral("Auto"), QSerialPort::UnknownParity);   // -1

    //-----------------------------------------------------------
    m_stopBitsBox->addItem(QStringLiteral("1"),  QSerialPort::OneStop);       // 1
#ifdef Q_OS_WIN
    m_stopBitsBox->addItem(QStringLiteral("1.5"),QSerialPort::OneAndHalfStop);// 2
#endif
    m_stopBitsBox->addItem(QStringLiteral("2"),  QSerialPort::TwoStop);       // 3

    //-----------------------------------------------------------
    m_flowCtrlBox->addItem(QStringLiteral("None"), QSerialPort::NoFlowControl);    // 0
    m_flowCtrlBox->addItem(QStringLiteral("RTS/CTS"), QSerialPort::HardwareControl);  // 1
    m_flowCtrlBox->addItem(QStringLiteral("XON/XOFF"), QSerialPort::SoftwareControl);  // 2
}

//---------------------------------------------------------
void SERIALCONF::update_form_fields()
{
    int pos = m_baudRateBox->findText(QString("%1").arg(m_port.baudRate));
    if(pos > -1) {
        m_baudRateBox->setCurrentIndex(pos);
    } else {
        pos = m_baudRateBox->count()-1;
        m_baudRateBox->setCurrentIndex(pos);
        m_baudRateBox->setItemText(pos, QString("%1").arg(m_port.baudRate));
        m_baudRateBox->setItemData(pos, m_port.baudRate, Qt::UserRole);
    }

    pos = m_dataBitsBox->findText(QString("%1").arg(m_port.dataBits));
    if(pos > -1) m_dataBitsBox->setCurrentIndex(pos);

    pos = m_stopBitsBox->findText(QString("%1").arg(m_port.stopBits));
    if(pos > -1) m_stopBitsBox->setCurrentIndex(pos);

    pos = m_parityBox->findText(QString("%1").arg(m_port.parity));
    if(pos > -1) m_parityBox->setCurrentIndex(pos);

    pos = m_flowCtrlBox->findText(QString("%1").arg(m_port.flowCtrl));
    if(pos > -1) m_flowCtrlBox->setCurrentIndex(pos);
}

//-----------------------------------------------------------
void SERIALCONF::flush_fields()
{
    int pos, val;

    if(m_baudRateBox->currentIndex() == m_baudRateBox->count()-1) {
        m_port.baudRate = m_baudRateBox->currentText().toInt();
    } else {
        pos = m_baudRateBox->currentIndex();
        val = m_baudRateBox->itemData(pos, Qt::UserRole).toInt();
        m_port.baudRate = static_cast<QSerialPort::BaudRate>(val);
    }


    pos = m_dataBitsBox->currentIndex();
    val = m_dataBitsBox->itemData(pos, Qt::UserRole).toInt();
    m_port.dataBits = static_cast<QSerialPort::DataBits>(val);

    pos = m_stopBitsBox->currentIndex();
    val = m_stopBitsBox->itemData(pos, Qt::UserRole).toInt();
    m_port.stopBits = static_cast<QSerialPort::StopBits>(val);

    pos = m_parityBox->currentIndex();
    val = m_parityBox->itemData(pos, Qt::UserRole).toInt();
    m_port.parity = static_cast<QSerialPort::Parity>(val);

    pos = m_flowCtrlBox->currentIndex();
    val = m_flowCtrlBox->itemData(pos, Qt::UserRole).toInt();
    m_port.flowCtrl = static_cast<QSerialPort::FlowControl>(val);
    //m_port_settings.localEcho = m_localEchoCheckBox->isChecked();
}

//-----------------------------------------------------------
QList<port_name_t> SERIALCONF::get_port_list()
{
    QList<port_name_t> portlist;
    static const QString blankString = SERIALCONF::tr("N/A");
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        QString description  = info.description();
        QString manufacturer = info.manufacturer();
        QString serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty()  ? description  : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation().replace("\\\\", "")
             << (info.vendorIdentifier() ?
                     QString::number(info.vendorIdentifier(),16):blankString)
             << (info.productIdentifier() ?
                     QString::number(info.productIdentifier(),16):blankString);

        auto port = port_name_t{list.first(), list};
        portlist.append(port);
    }
    return portlist;
}

//-----------------------------------------------------------
const port_t &SERIALCONF::port_settings() const
{
    return m_port;
}

//-----------------------------------------------------------
void SERIALCONF::set_port_settings(const port_t & port)
{
    m_port = port;
}

//-----------------------------------------------------------
// Переключение на ручной ввод скорости порта
// \ param[in] index - номер выбраного параметра
//
void SERIALCONF::slot_checkCustomBaudRatePolicy(int index)
{
    bool isCustomBaudRate = !m_baudRateBox->itemData(index).isValid();
    m_baudRateBox->setEditable(isCustomBaudRate);
    if(isCustomBaudRate)
    {
        m_baudRateBox->clearEditText();
        QLineEdit *edit = m_baudRateBox->lineEdit();
        edit->setValidator(m_intValidator);
    }
}


