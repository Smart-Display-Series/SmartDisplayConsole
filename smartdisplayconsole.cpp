#include <QDebug>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QtGlobal>

#include "smartdisplayconsole.h"
#include "ui_smartdisplayconsole.h"
#include "modbustransport.h"

const uint8_t slaveID = 0x7B;
const int MAX_CATEGORY = 10;
const QString VERSION = "v0.1.0.0";

// Please define your port name here
#ifdef Q_PROCESSOR_X86
const QString portName = "ttyUSB0";     // Windows/Ubuntu
#else
const QString portName = "ttymxc7";     // MX6UL-H210
#endif

const int baudrate = QSerialPort::Baud115200;


SmartDisplayConsole::SmartDisplayConsole(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SmartDisplayConsole)
{
    _loading = false;   // dont update UI when loading data

    ui->setupUi(this);

    _buzzer.cycle = 1;
    _buzzer.high = 20;
    _buzzer.low = 0;
    _buzzer.active = true;

    createUI();
    EnableHeaderControl(false);

    modbus = new ModbusTransport(NUM_WIDGET);
    QObject::connect(modbus, SIGNAL(notifyValueChanged(int,int)), this, SLOT(notifyValueChanged(int,int)));
    QObject::connect(modbus, SIGNAL(showInformation(QString)), this, SLOT(showInformation(QString)));
}

SmartDisplayConsole::~SmartDisplayConsole()
{
    delete ui;
}

void SmartDisplayConsole::showInformation(QString msg)
{
    _lblInfo->setText(msg);
}

void SmartDisplayConsole::createUI()
{
    _vertLayout = new QVBoxLayout(centralWidget());

    // Header buttons
    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->setSpacing(4);
    headerLayout->addWidget(_lblDeviceName = new QLabel("Not connected."), 10, Qt::AlignLeft);
    headerLayout->addWidget(_lblFirmwareVersion = new QLabel("00.00.00"), 10, Qt::AlignLeft);
    headerLayout->addWidget(_lblInfo  = new QLabel(VERSION), 10, Qt::AlignLeft);
    headerLayout->addWidget(new QLabel("Page"));
    headerLayout->addWidget(_cboPage = new QComboBox(), 5);
    headerLayout->addWidget(new QLabel("Bg"));
    headerLayout->addWidget(_cboBackground = new QComboBox(), 5);

    _cboPage->addItem("0");
    for(int i = 0; i < 5; i++)
    {
        _cboBackground->addItem(QString::number(i));
    }

    headerLayout->addWidget(new QLabel("Brightness"));
    _sbBrightness = new QSlider();
    _sbBrightness->setOrientation(Qt::Horizontal);
    _sbBrightness->setValue(100);
    headerLayout->addWidget(_sbBrightness, 15);

    headerLayout->addWidget(_btnBuzzer = new QPushButton("Buzzer"));
    headerLayout->addWidget(_btnSaveSetting= new QPushButton("Save"));
    headerLayout->addWidget(_btnConnect = new QPushButton("Connect"), 10, Qt::AlignRight);
    _vertLayout->addLayout(headerLayout);

    for(int id = 0; id < NUM_WIDGET; id++)
    {
        WidgetControl *widgetControl = _widgets[id] = new WidgetControl(id);

        QObject::connect(widgetControl, SIGNAL(showInformation(QString)), this, SLOT(showInformation(QString)));

        QHBoxLayout *widgetLayout = new QHBoxLayout;
        QHBoxLayout *settingLayout = new QHBoxLayout;
        QHBoxLayout *valueLayout = new QHBoxLayout;

        widgetLayout->addLayout(settingLayout, 50);
        widgetLayout->addLayout(valueLayout, 50);

        //widgetLayout->setSpacing(50);
        //widget->setLayoutControl(widgetLayout);

        // Create controls
        settingLayout->addWidget(new QLabel(QString::number(id)), 0);

        QComboBox *cboType = new QComboBox();
        settingLayout->addWidget(cboType, 10);
        widgetControl->setTypeControl(cboType);

        QComboBox *cboCategory = new QComboBox();
        settingLayout->addWidget(cboCategory, 0);
        widgetControl->setCategoryControl(cboCategory);

        QLineEdit *edPosX = new QLineEdit();
        edPosX->setAlignment(Qt::AlignRight);
        settingLayout->addWidget(edPosX, 10);
        widgetControl->setPosXControl(edPosX);

        QLineEdit *edPosY = new QLineEdit();
        edPosY->setAlignment(Qt::AlignRight);
        settingLayout->addWidget(edPosY, 10);
        widgetControl->setPosYControl(edPosY);

        QLabel *lbNotified = new QLabel("  ");
        valueLayout->addWidget(lbNotified, 5);
        widgetControl->setNotifiedValueControl(lbNotified);

        QSlider *sbValue = new QSlider();
        sbValue->setOrientation(Qt::Horizontal);
        valueLayout->addWidget(sbValue, 40);
        widgetControl->setValueSliderControl(sbValue);

        QLineEdit *edText = new QLineEdit();
        valueLayout->addWidget(edText, 50);
        widgetControl->setValueTextControl(edText);
        edText->setVisible(false); 

        QLabel *lbValue = new QLabel();
        valueLayout->addWidget(lbValue, 5);
        widgetControl->setValueLabelControl(lbValue);
        _vertLayout->addLayout(widgetLayout);

        // Set contents
        // Widget type
        for(int type = 0; type < static_cast<int>(NUM_WIDGET_TYPE); type++)
        {
            cboType->addItem(GetWidgetTypeName((WidgetType)type));
        }

        // Widget category
        for(int i = 0; i < MAX_CATEGORY; i++)
            cboCategory->addItem(QString::number(i));  // at least one item

        // Position
        edPosX->setText("0");
        edPosY->setText("0");

        // sbValue
        sbValue->setMinimum(0);
        sbValue->setMaximum(100);

        // Connect
        QObject::connect(cboType, SIGNAL(currentIndexChanged(int)), widgetControl, SLOT(widgetTypeChanged(int)));
        QObject::connect(cboCategory, SIGNAL(currentIndexChanged(int)), widgetControl, SLOT(widgetCategoryChanged(int)));
        QObject::connect(edPosX, SIGNAL(editingFinished()), widgetControl, SLOT(widgetPositionChanged()));
        QObject::connect(edPosY, SIGNAL(editingFinished()), widgetControl, SLOT(widgetPositionChanged()));
        QObject::connect(sbValue, SIGNAL(valueChanged(int)), widgetControl, SLOT(widgetValueChanged(int)));
        QObject::connect(sbValue, SIGNAL(sliderPressed()), this, SLOT(sliderPressed()));
        QObject::connect(sbValue, SIGNAL(sliderReleased()), this, SLOT(sliderReleased()));
        QObject::connect(edText, SIGNAL(editingFinished()), widgetControl, SLOT(widgetTextChanged()));
    }

    // Connect
    QObject::connect(_btnBuzzer, &QPushButton::clicked, [=]() {
        _buzzer.active = !_buzzer.active;
        modbus->SendBuzzer(_buzzer);
    });
    QObject::connect(_btnSaveSetting, &QPushButton::clicked, [=]() {
        QMessageBox msgBox;
        if(modbus->SaveSetting())
        {
           msgBox.setText("Setting has been saved");
        }
        else
        {
           msgBox.setText("Can not save setting");
        }
        msgBox.exec();
    });
    QObject::connect(_btnConnect, SIGNAL(clicked()), this, SLOT(btnConnectClicked()));   
    QObject::connect(_cboPage, SIGNAL(currentIndexChanged(int)), this, SLOT(pageIndexChanged(int)));
    QObject::connect(_cboBackground, SIGNAL(currentIndexChanged(int)), this, SLOT(backgroundIndexChanged(int)));
    QObject::connect(_sbBrightness, SIGNAL(valueChanged(int)), this, SLOT(brightnessChanged(int)));
    QObject::connect(_sbBrightness, SIGNAL(sliderPressed()), this, SLOT(sliderPressed()));
    QObject::connect(_sbBrightness, SIGNAL(sliderReleased()), this, SLOT(sliderReleased()));
}

void SmartDisplayConsole::updateUI()
{
    WidgetSetting setting;
    for(int widgetID = 0; widgetID < NUM_WIDGET; widgetID++)
    {
        if(modbus->ReadSmartDisplaySetting(widgetID, setting))
        {
            _widgets[widgetID]->connectDevice(modbus);
            _widgets[widgetID]->updateSetting(&setting);
        }
    }
}

void SmartDisplayConsole::EnableHeaderControl(bool enable)
{
    _cboPage->setEnabled(enable);
    _cboBackground->setEnabled(enable);
    _sbBrightness->setEnabled(enable);
    _btnSaveSetting->setEnabled(enable);
    _btnBuzzer->setEnabled(enable);
}

void SmartDisplayConsole::btnConnectClicked()
{
    if(modbus->IsConnected())
    {
        modbus->StopMaster();
    }
    else
    {
        _loading = true;    //

        if(modbus->StartMaster(slaveID, portName, baudrate) == false)
        {
            QMessageBox::critical(this, "Can not connect to device", "Error", QMessageBox::Ok);
        }
        else
        {
            SmartDisplayVersion version;
            if(modbus->ReadSmartDisplayVersion(version))
            {
                _lblDeviceName->setText(version.deviceName);
                _lblFirmwareVersion->setText(version.softwareVersion);

                // Read page count
                uint8_t pageCount;
                if(modbus->ReadPageCount(pageCount))
                {
                    _cboPage->clear();
                    for(int i = 0; i < pageCount; i++)
                    {
                        _cboPage->addItem(QString::number(i));
                    }
                }

                // Background index
                uint8_t backgroundIdx;
                if(modbus->ReadSmartDisplayBackground(backgroundIdx))
                {
                    _cboBackground->setCurrentIndex((int)backgroundIdx);
                }

                // Brightness
                uint8_t brightness;
                if(modbus->ReadSmartDisplayBrightness(brightness) && brightness <= 100)
                {
                    _sbBrightness->setValue((int)brightness);
                }

                updateUI();
            }
        }
    }

    if(modbus->IsConnected())
    {
        _btnConnect->setText("Disconnect");
        EnableHeaderControl(true);
        modbus->EnableSyncData(true);
    }
    else
    {
        _btnConnect->setText("Connect");
        _lblDeviceName->setText("Not connected.");
        _lblFirmwareVersion->setText("");
        EnableHeaderControl(false);
    }
    _loading = false;
}

void SmartDisplayConsole::pageIndexChanged(int index)
{
    if(_loading == false && modbus->IsConnected() && index >=0 && index < _cboPage->count())
    {
        modbus->ChangeDeviceState(ConfigMode);
        modbus->SendPageIndex(index);
        modbus->ChangeDeviceState(DisplayMode);

        updateUI();
    }
}

void SmartDisplayConsole::backgroundIndexChanged(int index)
{
    if(_loading == false && modbus->IsConnected() && index >=0 && index < _cboBackground->count())
    {
        modbus->ChangeDeviceState(ConfigMode);
        modbus->SendSmartDisplayBackground((uint8_t)index);
        modbus->ChangeDeviceState(DisplayMode);
    }
}

void SmartDisplayConsole::brightnessChanged(int value)
{
    if(_loading == false && modbus->IsConnected() && value >=0 && value <= 100)
    {
        modbus->SendSmartDisplayBrightness((uint8_t)value);
    }
}

void SmartDisplayConsole::sliderPressed()
{
    _sliderDragging = true;
}

void SmartDisplayConsole::sliderReleased()
{
    _sliderDragging = false;
}

// Updated by device
void SmartDisplayConsole::notifyValueChanged(int widgetID, int value)
{
    if(_sliderDragging == false)
        _widgets[widgetID]->notifyValueChanged((uint16_t)value);
}




