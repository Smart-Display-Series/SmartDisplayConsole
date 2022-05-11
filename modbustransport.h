#ifndef MODBUSTRANSPORT_H
#define MODBUSTRANSPORT_H

#include <QObject>
#include <QVector>
#include <QSerialPort>
#include <QTimer>
#include <QElapsedTimer>
#include <QMutex>
//#include <QStringConverter>

#include "SmartDisplayDefinition.h"


typedef struct _WidgetValue
{
    bool uiUpdated;
    uint16_t uiValue;
} WidgetValue;

class ModbusTransport : public QObject
{
    Q_OBJECT

public:
    ModbusTransport(int widgetCount);
    ~ModbusTransport();

    bool StartMaster(uint8_t slaveID, QString comPort, int baudrate);
    void StopMaster();
    bool IsConnected();

    bool ReadSmartDisplayVersion(SmartDisplayVersion &version);
    bool ReadSmartDisplaySetting(int objIdx, WidgetSetting &setting);
    bool SendSmartDisplaySetting(int objIdx, WidgetSetting setting);
    bool ReadSmartDisplayBackground(uint8_t &backgroundIdx);
    bool SendSmartDisplayBackground(uint8_t backgroundIdx);
    bool ReadSmartDisplayBrightness(uint8_t &brigntness);
    bool SendSmartDisplayBrightness(uint8_t brigntness);
    bool ReadPageCount(uint8_t &pageCount);
    bool SendPageIndex(uint8_t pageIdx);
    bool SendBuzzer(SmartDisplayBuzzer buzzer);
    bool GetFeature(uint32_t &feature);

    bool ChangeDeviceState(DeviceState state);
    bool EnterBootMode();
    bool SaveSetting();
    void EnableSyncData(bool enableSync);

    void NotifyValueChange(int objIdx, uint16_t value);
    bool NotifyTextChange(int objIdx, QString value);
    bool NotifyDateTimeChange(int objIdx, QByteArray value);

signals:
    void notifyValueChanged(int widgetID, int value);
    void showInformation(QString);

public slots:
    void requestDeviceValue();

protected:
    QVector<uint16_t> ReadInputRegisters(uint16_t startAddress, uint16_t count);
    QVector<uint16_t> ReadHoldingRegisters(uint16_t startAddress, uint16_t count);
    bool WriteSingleRegister(uint16_t address, uint16_t value);    
    bool WriteMultipleRegisters(uint16_t address, const uint16_t *startAddress, uint16_t count);
    QByteArray ReadResponse();
    qint64 RequestBytesToRead(char frameStart[]);
    qint64 ResponseBytesToRead(char frameStart[]);
    QByteArray CalculateCRC(QByteArray data, int count);
    bool CheckCRC(QByteArray data);
    QString ByteArrayToHexString(QByteArray data);
    QString ByteArrayToString(QByteArray data, int start);
    QString Uint16VectorToString(QVector<uint16_t>data);

    uint32_t ByteArrayToUint32(QByteArray data, int offset);
    void AppendUint16ToByteArray(uint16_t data, QByteArray &array);
    QVector<uint16_t>ByteArrayToUint16Array(QByteArray data, int offset, int count);
    bool IsOK(QByteArray response);

private:
    QSerialPort _serialPort;
    QMutex _mutex;
    uint8_t _slaveID;
    QString _comPort;
    int _baudrate;
    bool _enableSyncData;

    QTimer _timer;
    QElapsedTimer _requestTimer;    // Timer for requseting data from the SmartDisplay

    int _widgetCount;
    WidgetValue *_widgetValues;
    bool _updating;
};

#endif // MODBUSTRANSPORT_H
