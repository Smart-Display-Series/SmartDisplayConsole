/*
 * 一個widget所需要的 UI controls
 */

#include "WidgetControls.h"

const QString _emptyStr("---");

// 接受的資料形態與範圍
typedef enum {
    DisableInput,       // 關閉輸入欄位
    GaugeSliderInput,   // 數字 0 - 100
    NumberSliderInput,  // 0 - 65535
    OnOffInput,         // 0 - 1
    MultiStateInput,    // 0 - 4
    TextInput,          // 文字形態
} InputStyle;

// 每一種widget相對的輸入形態
static InputStyle _inputStyle[] =
{
    DisableInput,       // Empty = 0,
    DisableInput,       // Image = 1,
    GaugeSliderInput,   // Gauge = 2,
    DisableInput,       // BDI = 3,
    OnOffInput,         // Button = 4,
    OnOffInput,         // ToggleButton = 5,
    GaugeSliderInput,   // VerticalSlider = 6,
    GaugeSliderInput,   // HorizontalSlider = 7,
    OnOffInput,         // CheckButton = 8,
    GaugeSliderInput,   // Temperature = 9,
    GaugeSliderInput,   // Battery = 10,
    GaugeSliderInput,   // Graph = 11,
    OnOffInput,         // Indicator = 12,
    GaugeSliderInput,   // CircleProgress = 13,
    GaugeSliderInput,   // ImageProgress = 14,
    OnOffInput,         // GroupButton = 15,
    GaugeSliderInput,   // AnimatedImage = 16,
    NumberSliderInput,  // NumberStr = 17,
    TextInput,          // Text = 18,
    GaugeSliderInput,   // CustomWidget = 19,
    TextInput,          // DigitalClock = 20,
    OnOffInput,         // PhysicalButton = 21,
    MultiStateInput,    // MultiState = 22,
};

bool WidgetControl::updateSetting(WidgetSetting *setting )
{
    _uiSetting = _devSetting = *setting;

    _cboType->setCurrentIndex((int)setting->type);
    _cboCategory->setCurrentIndex((int)setting->category);
    _edPosX->setText(QString::number(setting->posX));
    _edPosY->setText(QString::number(setting->posY));

    switch(_inputStyle[setting->type])
    {
    case DisableInput:
        _lbNotified->setText(_emptyStr);
        _sbValue->setVisible(true);
        _sbValue->setEnabled(false);
        _edText->setVisible(false);
        break;

    case GaugeSliderInput:
        _lbNotified->setText(_emptyStr);
        _sbValue->setVisible(true);
        _sbValue->setEnabled(true);
        _sbValue->setMaximum(100);
        _edText->setVisible(false);        
        break;

    case NumberSliderInput:
        _lbNotified->setText(_emptyStr);
        _sbValue->setVisible(true);
        _sbValue->setEnabled(true);
        _sbValue->setMaximum(65535);
        _edText->setVisible(false);
        break;

    case OnOffInput:
        _lbNotified->setText(_emptyStr);
        _sbValue->setVisible(true);
        _sbValue->setEnabled(true);
        _sbValue->setMaximum(1);
        _edText->setVisible(false);
        break;

    case MultiStateInput:
        _lbNotified->setText(_emptyStr);
        _sbValue->setVisible(true);
        _sbValue->setEnabled(true);
        _sbValue->setMaximum(4);
        _edText->setVisible(false);
        break;

    case TextInput:
        _lbNotified->setText(_emptyStr);
        _sbValue->setVisible(false);
        _sbValue->setEnabled(false);
        _edText->setVisible(true);

        if(_cboType->currentIndex() == Text)
        {
            _edText->setText(setting->textBuffer);
        }
        else if(_cboType->currentIndex() == DigitalClock)
        {
            QDateTime now = QDateTime::currentDateTime();
            _edText->setText(now.toString("yyyy-MM-dd HH:mm:ss"));
        }
        break;
    }

    _sbValue->setValue(setting->value1);
    updateValueLabel(setting->value1);

    return true;
}

bool WidgetControl::updateValueLabel(uint16_t value)
{
    switch((int)_inputStyle[_cboType->currentIndex()])
    {
    case GaugeSliderInput:
    case MultiStateInput:
    case NumberSliderInput:
        _lbValue->setText(QString::number(value));
        break;

    case OnOffInput:
        _lbValue->setText(value ? "ON":"OFF");
        break;

    default:
        _lbValue->setText(_emptyStr);
        break;
    }
    return true;
}

// The SmartDisplay value has been changed to change the UI display
void WidgetControl::notifyValueChanged(int value)
{
    if(_uiSetting.value1 != value)
    {
        _uiSetting.value1 = _devSetting.value1 = value;

        switch((int)_inputStyle[_cboType->currentIndex()])
        {
        case GaugeSliderInput:
        case MultiStateInput:
        case NumberSliderInput:
            _lbNotified->setText(QString::number(value));
            break;

        case OnOffInput:
            _lbNotified->setText(value ? "ON":"OFF");
            break;

        default:
            _lbValue->setText(_emptyStr);  
            break;
        }
        _sbValue->setValue(value);
        updateValueLabel(value);
    }
}

// Change the widget type via the UI
void WidgetControl::widgetTypeChanged(int type)
{
    _uiSetting.type = type;
    if(type != _devSetting.type)
    {
        _devSetting.type = type;
        if(_modbus->SendSmartDisplaySetting(_id, _uiSetting))
        {
            updateSetting(&_uiSetting);
        }
    }
}

// Change the widget category via the UI
void WidgetControl::widgetCategoryChanged(int category)
{
    _uiSetting.category = category;
    if(category != _devSetting.category)
    {
        _devSetting.category = category;
        if(_modbus->SendSmartDisplaySetting(_id, _uiSetting))
        {
            updateSetting(&_uiSetting);
        }
    }
}

// Change the widget position via the UI
void WidgetControl::widgetPositionChanged()
{
    _uiSetting.posX = _edPosX->text().toInt();
    _uiSetting.posY = _edPosY->text().toInt();
    if(_uiSetting.posX != _devSetting.posX || _uiSetting.posY != _devSetting.posY)
    {
        _devSetting.posX = _uiSetting.posX;
        _devSetting.posY = _uiSetting.posY;
        if(_modbus->SendSmartDisplaySetting(_id, _uiSetting))
        {
            updateSetting(&_uiSetting);
        }
    }
}

// Change the widget value via the UI
void WidgetControl::widgetValueChanged(int value)
{
    if(_uiSetting.value1 != value)
    {
        _uiSetting.value1 = value;
        if(value != _devSetting.value1)
        {
            _devSetting.value1 = value;
            _modbus->NotifyValueChange(_id, value);
        }
        updateValueLabel(value);
    }
}

// Changed from UI
void WidgetControl::widgetTextChanged()
{
    int value = 0;

    switch((int)_inputStyle[_cboType->currentIndex()])
    {
    case GaugeSliderInput:
    case MultiStateInput:
    case NumberSliderInput:
    case OnOffInput:
        value = _edText->text().toInt();
        widgetValueChanged(value);
        break;

    default:
        if(_cboType->currentIndex() == Text)
        {
            _uiSetting.textBuffer = _devSetting.textBuffer = _edText->text();
            _modbus->NotifyTextChange(_id, _edText->text());
        }
        else if(_cboType->currentIndex() == DigitalClock)
        {
            QDateTime t = QDateTime::fromString(_edText->text(), "yyyy-MM-dd HH:mm:ss");
            QByteArray data;
            data.resize(8);
            data[0] = t.date().year() - 2000;
            data[1] = t.date().month();
            data[2] = t.date().day();
            data[3] = t.date().dayOfWeek();
            data[4] = t.time().hour();
            data[5] = t.time().minute();
            data[6] = t.time().second();
            data[7] = 0;
            _modbus->NotifyDateTimeChange(_id, data);
        }
        break;
    }
    _sbValue->setValue(value);
    updateValueLabel(value);
}

