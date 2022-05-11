#include <QMessageBox>
#include <QtEndian>
#include <QVector>
#include "modbustransport.h"

/*
 *  Constant Defnintion
 */

const int waitForReadPeriod = 1000;
const int waitForWritePeriod = 1000;

//  SmartDisplay Modbus Constants
const uint16_t WIDGET_SETTING_SPACE = 100;
const uint16_t WIDGET_SETTING_COUNT = 8;

const uint16_t HOLDING_REGISTER_TYPE = 0;
const uint16_t HOLDING_REGISTER_RESERVED = 1;
const uint16_t HOLDING_REGISTER_POSX = 2;
const uint16_t HOLDING_REGISTER_POSY = 3;
const uint16_t HOLDING_REGISTER_STYLE = 4;
const uint16_t HOLDING_REGISTER_SETVALUE = 6;
const uint16_t HOLDING_REGISTER_GETVALUE = 7;
const uint16_t HOLDING_REGISTER_TEXT_BUFFER = 8;

const uint16_t DEVICE_NAME_ADDRESS = 0;
const uint16_t MAX_DEVICE_NAME_LENGTH = 20;     // bytes
const uint16_t SOFTWARE_VERSION_ADDRESS = 20;
const uint16_t MAX_SOFTWARE_VERSION = 20;
const uint16_t HARDWARE_VERSION_ADDRESS = 10;
const uint16_t MAX_HARDWARE_VERSION = 20;
const uint16_t FEATURE_1_ADDRESS = 100;

const uint16_t VALUE1_MAPPING_ADDRESS = 2000;
const uint16_t MAX_OBJECT_SIZE = 64;
const uint16_t OBJECT_11_ADDRESS = 10000;       // Th widgets after 10 start at 10000

const uint16_t Buzzer_Cycle_ADDRESS = 2100;
const uint16_t Buzzer_High_ADDRESS = 2101;
const uint16_t Buzzer_Low_ADDRESS = 2102;
const uint16_t Buzzer_Active_ADDRESS = 2103;
const uint16_t BACKGROUND_ADDRESS = 2110;
const uint16_t BRIGHTNESS_ADDRESS = 2111;
const uint16_t GOTOPAGE_ADDRESS = 2112;
const uint16_t PAGECOUNT_ADDRESS = 2113;

const uint16_t ENTER_BOOT_MODE_ADDRESS = 2500;
const uint16_t DEVICE_STATE_ADDRESS = 2502;

const uint16_t UPGRADE_FIRMWARE_COMMAND_ADDRESS = 2503;
const uint16_t BOOTLADER_STATUS_ADDRESS = 2504;
const uint16_t SAVE_SETTING_ADDRESS = 2505;
const uint16_t ENTER_TEST_MODE_ADDRESS = 2507;
const uint16_t UPGRADE_FIRMWARE_DATA_ADDRESS = 3000;
const uint16_t MAX_UPGRADE_FIRMWARE_DATA_SIZE = 246;
const uint16_t MAX_BUFFER_SIZE = (57 - 8 + 1);          // Total 50 uint16_t

const uint16_t RESPONSE_FRAME_START_LENGTH = 3;
const uint16_t REQUEST_FRAME_START_LENGTH = 7;

// Modbus function codes
const uint8_t READ_COIL_STATUS = 1;
const uint8_t READ_INPUT_STATUS = 2;
const uint8_t READ_HOLDING_REGISTERS = 3;
const uint8_t READ_INPUT_REGISTERS = 4;
const uint8_t WRITE_SINGLE_COIL = 5;
const uint8_t WRITE_SINGLE_REGISTER = 6;
const uint8_t WRITE_MULTIPLE_COILS = 15;
const uint8_t WRITE_MULTIPLE_REGISTERS = 16;

const uint16_t SYNC_TIME_MS = 50;       // Update UI period
const uint16_t POLLING_TIME_MS = 50;    // Get data from device period

const uint16_t crcTable[] =
{
            0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
            0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
            0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
            0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
            0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
            0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
            0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
            0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
            0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
            0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
            0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
            0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
            0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
            0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
            0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
            0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
            0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
            0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
            0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
            0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
            0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
            0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
            0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
            0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
            0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
            0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
            0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
            0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
            0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
            0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
            0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
            0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
        };


#define ByteArrayToUint16(data, offset) (((((uint16_t)data.at(offset)) << 8) & 0xFF00) + (((uint16_t)data.at(offset+1)) & 0x00FF))

ModbusTransport::ModbusTransport(int objCount)
{
    _widgetCount = objCount;
    _widgetValues = new WidgetValue[objCount];
    _updating = false;

    _slaveID = 0x7B;    // SmartDisplay ID
    _enableSyncData = false;

    _timer.setInterval(SYNC_TIME_MS);
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(requestDeviceValue()));
}

ModbusTransport::~ModbusTransport()
{
    _timer.stop();
    if(_widgetValues)
        delete[] _widgetValues;
}

bool ModbusTransport::StartMaster(uint8_t slaveID, QString comPort, int baudrate)
{
    _slaveID = slaveID;
    _comPort = comPort;
    _baudrate = baudrate;

    _serialPort.setPortName(_comPort);

    if(!_serialPort.open(QIODevice::ReadWrite)) {
        QMessageBox::critical(nullptr, "Error", "Can not open " + _comPort, QMessageBox::Yes);
    }
    else
    {
        _serialPort.setBaudRate(_baudrate);
    }
    _timer.start();
    _requestTimer.start();
    return _serialPort.isOpen();
}

void ModbusTransport::StopMaster()
{
    _enableSyncData = false;
    _timer.stop();
    _serialPort.close();
}

bool ModbusTransport::IsConnected()
{
    return _serialPort.isOpen();
}

bool ModbusTransport::ReadSmartDisplayVersion(SmartDisplayVersion &version)
{
    bool ret = false;
    if(IsConnected())
    {
        QVector<uint16_t> deviceName = ReadInputRegisters(DEVICE_NAME_ADDRESS, MAX_DEVICE_NAME_LENGTH / 2); // # bytes to uint16_t
        QVector<uint16_t> softwareVersion = ReadInputRegisters(SOFTWARE_VERSION_ADDRESS, MAX_SOFTWARE_VERSION / 2);
        QVector<uint16_t> hardwareVersion = ReadInputRegisters(HARDWARE_VERSION_ADDRESS, MAX_HARDWARE_VERSION / 2);
        version.deviceName = Uint16VectorToString(deviceName);
        version.softwareVersion = Uint16VectorToString(softwareVersion);
        version.hardwareVersion = Uint16VectorToString(hardwareVersion);
        ret = true;
    }
    return ret;
}

bool ModbusTransport::ReadSmartDisplaySetting(int objIdx, WidgetSetting &setting)
{
    bool ret = false;

    uint16_t address = (uint16_t)(objIdx * WIDGET_SETTING_SPACE);
    if(objIdx >= 10)
        address += OBJECT_11_ADDRESS;

    QVector<uint16_t> result = ReadHoldingRegisters(address, WIDGET_SETTING_COUNT);

    if(result.count() >= (long long)(WIDGET_SETTING_COUNT))
    {
        setting.type = (uint8_t)result[HOLDING_REGISTER_TYPE];
        setting.posX = (uint16_t)result[HOLDING_REGISTER_POSX];
        setting.posY = (uint16_t)result[HOLDING_REGISTER_POSY];
        setting.category = (uint16_t)result[HOLDING_REGISTER_STYLE];
        setting.value1 = (uint16_t)result[HOLDING_REGISTER_SETVALUE];
        setting.value2 = (uint16_t)result[HOLDING_REGISTER_GETVALUE];

        // Initialization
        _widgetValues[objIdx].uiValue = setting.value1;
        _widgetValues[objIdx].uiUpdated = false;

        ret = true;
    }

    return ret;
}

bool ModbusTransport::SendSmartDisplaySetting(int objIdx, WidgetSetting setting)
{
    bool ret = false;
    ChangeDeviceState(ConfigMode);

    uint16_t address = (uint16_t)(objIdx * WIDGET_SETTING_SPACE);
    if(objIdx >= 10)
        address += OBJECT_11_ADDRESS;
    uint16_t *settingValue = new uint16_t[7];

    settingValue[HOLDING_REGISTER_TYPE] = setting.type;
    settingValue[HOLDING_REGISTER_RESERVED] = 0;
    settingValue[HOLDING_REGISTER_POSX] = setting.posX;
    settingValue[HOLDING_REGISTER_POSY] = setting.posY;
    settingValue[HOLDING_REGISTER_STYLE] = (uint16_t)setting.category;
    settingValue[HOLDING_REGISTER_SETVALUE] = setting.value1;

    ret = WriteMultipleRegisters(address, settingValue, 7);
    delete[] settingValue;

    _widgetValues[objIdx].uiValue = setting.value1;
    _widgetValues[objIdx].uiUpdated = false;

    ChangeDeviceState(DisplayMode);

    return ret;
}

bool ModbusTransport::ReadSmartDisplayBackground(uint8_t &backgroundIdx)
{
    bool ret = false;
    QVector<uint16_t> result = ReadHoldingRegisters(BACKGROUND_ADDRESS, 1);
    if(result.count() > 0)
    {
        backgroundIdx = (uint8_t)result[0];
        ret = true;
    }
    return ret;
}

bool ModbusTransport::SendSmartDisplayBackground(uint8_t backgroundIdx)
{
    return WriteSingleRegister(BACKGROUND_ADDRESS, (uint16_t)backgroundIdx);
}

bool ModbusTransport::ReadSmartDisplayBrightness(uint8_t &brightness)
{
    bool ret = false;
    QVector<uint16_t> result = ReadHoldingRegisters(BRIGHTNESS_ADDRESS, 1);
    if(result.count() > 0)
    {
        brightness = (uint8_t)result[0];
        ret = true;
    }
    return ret;
}

bool ModbusTransport::SendSmartDisplayBrightness(uint8_t brightness)
{
    return WriteSingleRegister(BRIGHTNESS_ADDRESS, (uint16_t)brightness);
}

bool ModbusTransport::ReadPageCount(uint8_t &pageCount)
{
    bool ret = false;
    QVector<uint16_t> result = ReadHoldingRegisters(PAGECOUNT_ADDRESS, 1);
    if(result.count() > 0)
    {
        pageCount = (uint8_t)result[0];
        ret = true;
    }
    return ret;
}

bool ModbusTransport::SendPageIndex(uint8_t pageIdx)
{   
    return WriteSingleRegister(GOTOPAGE_ADDRESS, (uint16_t)pageIdx);
}

bool ModbusTransport::SendBuzzer(SmartDisplayBuzzer buzzer)
{
    WriteSingleRegister(Buzzer_Cycle_ADDRESS, (uint16_t)buzzer.cycle);
    WriteSingleRegister(Buzzer_High_ADDRESS, (uint16_t)buzzer.high);
    WriteSingleRegister(Buzzer_Low_ADDRESS, (uint16_t)buzzer.low);
    WriteSingleRegister(Buzzer_Active_ADDRESS, (uint16_t)buzzer.active);

    return true;
}

bool ModbusTransport::GetFeature(uint32_t &feature)
{
    bool ret = false;
    QVector<uint16_t> result = ReadHoldingRegisters(FEATURE_1_ADDRESS, 2);
    if(result.count() >= 2)
    {
        feature = ((uint32_t)result[0] << 16) | (uint32_t)result[1];
        ret = true;
    }
    return ret;
}

bool ModbusTransport::ChangeDeviceState(DeviceState state)
{    
    return WriteSingleRegister(DEVICE_STATE_ADDRESS, (uint16_t)state);
}

bool ModbusTransport::EnterBootMode()
{
    uint16_t sendData[] = {0x677D, 0x4f45};
    return WriteMultipleRegisters(ENTER_BOOT_MODE_ADDRESS, sendData, ARRAYSIZE(sendData));
}

bool ModbusTransport::SaveSetting()
{
    uint16_t sendData[] = {0x6173, 0x6576};
    return WriteMultipleRegisters(SAVE_SETTING_ADDRESS, sendData, ARRAYSIZE(sendData));
}

void ModbusTransport::EnableSyncData(bool enableSync)
{
    _enableSyncData = enableSync;
}

void ModbusTransport::NotifyValueChange(int objIdx, uint16_t value)
{
    if(IsConnected())
    {
        uint16_t address = (uint16_t)(objIdx * WIDGET_SETTING_SPACE + HOLDING_REGISTER_SETVALUE);
        if(objIdx >= 10)
            address += OBJECT_11_ADDRESS;

        // Just record the values, and let the timer to its work
        _widgetValues[objIdx].uiValue = value;
        _widgetValues[objIdx].uiUpdated = true; 
    }
}

bool ModbusTransport::NotifyTextChange(int objIdx, QString value)
{
    if(IsConnected())
    {
        uint16_t address = (uint16_t)(objIdx * WIDGET_SETTING_SPACE + HOLDING_REGISTER_TEXT_BUFFER);
        if(objIdx >= 10)
            address += OBJECT_11_ADDRESS;

        uint16_t data[MAX_BUFFER_SIZE];
        int count = value.length();
        memcpy(data, value.data(), count * sizeof(QChar));
        WriteMultipleRegisters(address, data, count);
    }
    return true;
}

bool ModbusTransport::NotifyDateTimeChange(int objIdx, QByteArray value)
{
    if(IsConnected())
    {
        uint16_t address = (uint16_t)(objIdx * WIDGET_SETTING_SPACE + HOLDING_REGISTER_TEXT_BUFFER);
        if(objIdx >= 10)
            address += OBJECT_11_ADDRESS;
        int count = (value.length() + 1)/2;

        uint16_t data[MAX_BUFFER_SIZE];
        memcpy(data, value.data(), count * sizeof(uint16_t));
        WriteMultipleRegisters(address, data, count);
    }
    return true;
}

QVector<uint16_t> ModbusTransport::ReadInputRegisters(uint16_t startAddress, uint16_t count)
{
    QByteArray reply;
    QVector<uint16_t> empty;  // return empty result
    QByteArray msg;
    msg.append(_slaveID);
    msg.append(READ_INPUT_REGISTERS);
    msg.append((uint8_t)startAddress >> 8);
    msg.append((uint8_t)startAddress & 0xFF);
    msg.append((uint8_t)count >> 8);
    msg.append((uint8_t)count & 0xFF);
    QByteArray msgToSend = msg.append(CalculateCRC(msg, msg.count()));

    _mutex.lock();
    _serialPort.clear(QSerialPort::Input);
    _serialPort.write(msgToSend);
    reply = ReadResponse();
     _mutex.unlock();

    if(IsOK(reply))
        return ByteArrayToUint16Array(reply, 3, count);
    else
        return empty;
}

QVector<uint16_t> ModbusTransport::ReadHoldingRegisters(uint16_t startAddress, uint16_t count)
{
    QByteArray reply;
    QVector<uint16_t> empty;
    QByteArray msg;
    msg.append(_slaveID);
    msg.append(READ_HOLDING_REGISTERS);
    AppendUint16ToByteArray(startAddress, msg);
    AppendUint16ToByteArray(count, msg);
    QByteArray msgToSend = msg.append(CalculateCRC(msg, msg.count()));

    _mutex.lock();
    _serialPort.clear(QSerialPort::Input);
    _serialPort.write(msgToSend);
    reply = ReadResponse();
    _mutex.unlock();

    /*
    if(CheckCRC(ret)==false) {
        qCritical("CRC error");
    }
    */

    if(IsOK(reply))
        return ByteArrayToUint16Array(reply, 3, count);
    else
        return empty;
}

bool ModbusTransport::WriteSingleRegister(uint16_t address, uint16_t value)
{
    QByteArray msg;

    msg.append(_slaveID);
    msg.append(WRITE_SINGLE_REGISTER);
    AppendUint16ToByteArray(address, msg);
    AppendUint16ToByteArray(value, msg);

    QByteArray msgToSend = msg.append(CalculateCRC(msg, msg.count()));

    _mutex.lock();
    _serialPort.clear(QSerialPort::Input);
    _serialPort.write(msgToSend);
    QByteArray reply = ReadResponse();
    _mutex.unlock();

    return IsOK(reply);
}

bool ModbusTransport::WriteMultipleRegisters(uint16_t address, const uint16_t *startAddress, uint16_t count)
{
    QByteArray msg;
    msg.append(_slaveID);
    msg.append(WRITE_MULTIPLE_REGISTERS);
    AppendUint16ToByteArray(address, msg);
    AppendUint16ToByteArray(count, msg);
    msg.append((uint8_t)(count * sizeof(uint16_t)));    // byte count

    for(uint16_t i = 0; i < count; i++)
    {
        uint16_t value = startAddress[i];
        AppendUint16ToByteArray(value, msg);
    }
    QByteArray msgToSend = msg.append(CalculateCRC(msg, msg.count()));

    _mutex.lock();
    _serialPort.clear(QSerialPort::Input);
    _serialPort.write(msgToSend);
    QByteArray result = ReadResponse();
    _mutex.unlock();

    return IsOK(result);;
}

QByteArray ModbusTransport::CalculateCRC(QByteArray data, int count)
{
    uint16_t crc = 0xFFFF;
    for(int idx = 0; idx < count; idx++)
    {
        uint8_t i = (uint8_t)crc ^ data[idx];
        crc >>= 8;
        crc ^= crcTable[i];
    }
    QByteArray ba;
    ba.append(char(crc & 0xFF));
    ba.append(char(crc >> 8));
    return ba;
}

bool ModbusTransport::CheckCRC(QByteArray data)
{
    QByteArray code = CalculateCRC(data, data.count()-2);
    return (code[0] == data[data.count()-2]) && (code[1] == data[data.count()-1]);
}

QString ModbusTransport::ByteArrayToHexString(QByteArray data)
{
    QString ret(data.toHex().toUpper());
    int len = ret.length()/2;
    for(int i = 1; i < len; i++)
    {
        ret.insert(2*i + i - 1, " ");
    }
    return ret;
}

QString ModbusTransport::ByteArrayToString(QByteArray data, int start)
{
    QString ret;
    for(int i = start; i < data.count(); i+=2)
    {
        if(data.at(i+1) == '\0')
            break;
        else
            ret.append((QChar)data.at(i+1));
        if(data.at(i) == '\0')
            break;
        else
            ret.append((QChar)data.at(i));
    }
    return ret;
}

QString ModbusTransport::Uint16VectorToString(QVector<uint16_t>data)
{
    QString ret;
    for(int i = 0; i < data.count(); i++)
    {
        QChar c = (QChar)((data[i] & 0xFF));
        if(c == '\0')
            break;
        ret.append(c);

        c = (QChar)((data[i] >> 8) & 0xFF);
        if(c == '\0')
            break;
        ret.append(c);

    }
    return ret;
}

uint32_t ModbusTransport::ByteArrayToUint32(QByteArray data, int offset)
{
    uint32_t value = 0;
    for(int i = 3; i >=0; i--)
    {
        value <<= 8;
        value |= ((uint32_t)data.at(offset + i) & 0xFF);
    }
    return value;
}

QVector<uint16_t> ModbusTransport::ByteArrayToUint16Array(QByteArray data, int offset, int count)
{
    QVector<uint16_t> result;
    QByteArray inData = data;

    for(int i = 0; i < count; i++)
    {
        uint16_t  hi, low;
        int idx = offset + i * 2;
        if(idx < data.count())
            hi = ((uint16_t)data[idx]) << 8;
        else
            hi = 0;
        idx++;
        if(idx < data.count())
            low = ((uint16_t)data[idx]) & 0x00FF;
        else
            low = 0;
        int16_t value = hi + low;
        result.append(value);
    }
    return result;
}

void ModbusTransport::AppendUint16ToByteArray(uint16_t data, QByteArray &array)
{
    array.append((uint8_t)((data >> 8) & 0x00FF));
    array.append((uint8_t)(data & 0x00FF));
}

/*
 *  Read response
 *  [0] slave address
 *  [1] function code. A 1 in the highest bit indicates an error
 *  [2] depends on value of [1]
 */
QByteArray ModbusTransport::ReadResponse()
{
    QByteArray result;
    qint64 bytesToRead = RESPONSE_FRAME_START_LENGTH, readBytes = 0, count = 0, total=0;
    char data[120];
    bool available = true;

    // Read the first 3 bytes to find out how many bytes there are
    while(available && readBytes < bytesToRead)
    {
        if(_serialPort.bytesAvailable() == 0)
            available = _serialPort.waitForReadyRead(waitForReadPeriod);
        count = _serialPort.read(data + readBytes, bytesToRead - readBytes);
        readBytes += count;
    }

    total = bytesToRead;
    bytesToRead = ResponseBytesToRead(data);
    readBytes = 0;
    while(available && readBytes < bytesToRead)
    {
        if(_serialPort.bytesAvailable() == 0)
            available = _serialPort.waitForReadyRead(waitForReadPeriod);
        if(available)
        {
            count = _serialPort.read(data + total + readBytes, bytesToRead - readBytes);
            readBytes += count;
        }
    }
    total += bytesToRead;

    result.resize(total + 1);   // Reserve on byte for '\0'
    for(int i = 0; i < total; i++)
        result[i] = data[i];

    return result;
}

qint64 ModbusTransport::RequestBytesToRead(char frameStart[])  // header could be 4 or 7 bytes
{
    int numBytes;
    switch(frameStart[1])
    {
    case READ_COIL_STATUS:
    case READ_INPUT_STATUS:
    case READ_HOLDING_REGISTERS:
    case READ_INPUT_REGISTERS:
    case WRITE_SINGLE_COIL:
    case WRITE_SINGLE_REGISTER:
        numBytes = 1;
        break;
    case WRITE_MULTIPLE_COILS:
    case WRITE_MULTIPLE_REGISTERS:
        numBytes = frameStart[6] + 2;
        break;
    default:
        numBytes = 0; // error
        break;
    }
    return numBytes;
}

/*
 * Determines how many bytes to read in addition to the header
 */
qint64 ModbusTransport::ResponseBytesToRead(char frameStart[])
{
    int numBytes;
    switch(frameStart[1])
    {
    case READ_COIL_STATUS:
    case READ_INPUT_STATUS:
    case READ_HOLDING_REGISTERS:
    case READ_INPUT_REGISTERS:
        if(frameStart[1] & 0x80)    // Error
        {
            numBytes = 2;           // crc
        }
        else
        {
            numBytes = frameStart[2] + 2;  // data + crc
        }
        break;

    case WRITE_SINGLE_COIL:
    case WRITE_SINGLE_REGISTER:
    case WRITE_MULTIPLE_COILS:
    case WRITE_MULTIPLE_REGISTERS:
        if(frameStart[1] & 0x80)    // error
        {
            numBytes = 2;           // crc
        }
        else
        {
            numBytes = 5;
        }
        break;

    default:
        numBytes = 0; // error
        break;
    }
    return numBytes;
}


void ModbusTransport::requestDeviceValue()
{
    static int checkCount = 0;
    static int uiCount = 0;
    static int devCount = 0;

    if(_enableSyncData == false)
        return;

    if(_updating == true)
    {
        return;
    }
    _updating = true;

    checkCount++;

    // Comparing UI with device values. Writing to device if UI changed.
    for(int objIdx = 0; objIdx < _widgetCount; objIdx++)
    {
        if(_widgetValues[objIdx].uiUpdated) // Writing to device
        {
            uiCount++;

            uint16_t address = (uint16_t)(objIdx * WIDGET_SETTING_SPACE + HOLDING_REGISTER_SETVALUE);            
            WriteSingleRegister(address, _widgetValues[objIdx].uiValue);
        }
    }

    // Getting values from the SmartDisplay
    if(_requestTimer.elapsed() >= POLLING_TIME_MS)
    {
        QVector<uint16_t> values = ReadHoldingRegisters(VALUE1_MAPPING_ADDRESS, _widgetCount);

        if(values.count() >= _widgetCount)
        {
            for(int objIdx = 0; objIdx < _widgetCount; objIdx++)
            {
                uint16_t value = values[objIdx];
                if(_widgetValues[objIdx].uiUpdated == false && value != _widgetValues[objIdx].uiValue)
                {
                    devCount++;

                    _widgetValues[objIdx].uiValue = value;
                    emit notifyValueChanged(objIdx, value);
                }
            }
        }
        _requestTimer.start();
    }

    for(int objIdx = 0; objIdx < _widgetCount; objIdx++)
    {
        _widgetValues[objIdx].uiUpdated = false;
    }

    _updating = false;
}

bool ModbusTransport::IsOK(QByteArray response)
{
    return (response.count() > 3 && ((response[1] & 0x80) == 0));
}



