#ifndef SMARTDISPLAYCONSOLE_H
#define SMARTDISPLAYCONSOLE_H

#include <QMainWindow>
#include <QSerialPort>
#include <QVector>
#include "WidgetControls.h"
#include "modbustransport.h"
#include "SmartDisplayDefinition.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SmartDisplayConsole; }
QT_END_NAMESPACE

const int NUM_WIDGET = 10;

class SmartDisplayConsole : public QMainWindow
{
    Q_OBJECT

public:
    SmartDisplayConsole(QWidget *parent = nullptr);
    ~SmartDisplayConsole();

public slots:
    void showInformation(QString);

    // device updated, set ui value
    void notifyValueChanged(int widgetID, int value);

    // UI changed, send to device
    void btnConnectClicked();
    void pageIndexChanged(int index);
    void backgroundIndexChanged(int index);
    void brightnessChanged(int value);

    // UI
    void sliderPressed();
    void sliderReleased();

private:
    void createUI();
    void updateUI();
    void EnableHeaderControl(bool enable);

private:
    bool _loading;
    bool _sliderDragging;

    Ui::SmartDisplayConsole *ui;

    QVBoxLayout *_vertLayout;
    QLabel *_lblDeviceName;
    QLabel *_lblFirmwareVersion;
    QLabel *_lblInfo;

    QComboBox *_cboPage;
    QComboBox *_cboBackground;

    QSlider *_sbBrightness;
    QPushButton *_btnBuzzer;
    QPushButton *_btnSaveSetting;
    QPushButton *_btnConnect;
    WidgetControl *_widgets[NUM_WIDGET];

    SmartDisplayBuzzer _buzzer;

    ModbusTransport *modbus;
};
#endif // SMARTDISPLAYCONSOLE_H
