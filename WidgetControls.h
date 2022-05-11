#ifndef WIDGETCONTROLS_H
#define WIDGETCONTROLS_H

#include "QLabel"
#include "QComboBox"
#include "QLineEdit"
#include "QCheckBox"
#include "QSlider"
#include "QPushButton"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QSpacerItem"
#include "QDateTime"
#include "SmartDisplayDefinition.h"
#include "modbustransport.h"

class WidgetControl : public QObject
{
    Q_OBJECT

public:
    WidgetControl(int id) { _id = id; };

    void connectDevice(ModbusTransport *modbus) { _modbus = modbus;}

    bool updateSetting(WidgetSetting *setting );    

    // ID
    bool getID() const { return _id;}

    // Type
    void setTypeControl(QComboBox *cboType)
    {
        _cboType = cboType;
    };
    QComboBox *getTypeControl() const {return _cboType;}

    // Category
    void setCategoryControl(QComboBox *cboCategory)
    {
        _cboCategory = cboCategory;
    };
    QComboBox *getCategoryControl() const {return _cboCategory;}

    // PosX
    void setPosXControl(QLineEdit *edPosX)
    {
        _edPosX = edPosX;
    }
    QLineEdit *GetPosXControl() const { return _edPosX; }

    // PosY
    void setPosYControl(QLineEdit *edPosY)
    {
        _edPosY = edPosY;
    }
    QLineEdit *getPosYControl() const { return _edPosY; }

    // _lbNotified
    void setNotifiedValueControl(QLabel *lbNotified)
    {
        _lbNotified = lbNotified;
    }
    QLabel* getNotifiedValueControl() const { return _lbNotified; }

     // value slider
    void setValueSliderControl(QSlider *sbValue)
    {
        _sbValue = sbValue;
    }
    QSlider *getValueSliderControl() const { return _sbValue; }

    // value label
    void setValueLabelControl(QLabel *lbValue)
    {
        _lbValue = lbValue;
    }

    // value text
    void setValueTextControl(QLineEdit *edText)
    {
        _edText = edText;
    }

    // device changed, set ui
    void notifyValueChanged(int value);

signals:
    void showInformation(QString);

public slots:
    // ui updated, set device value
    void widgetTypeChanged(int type);
    void widgetCategoryChanged(int category);
    void widgetPositionChanged();
    void widgetValueChanged(int value);
    void widgetTextChanged();

private:
    bool updateValueLabel(uint16_t value);

private:
    int _id;
    ModbusTransport *_modbus;
    QComboBox *_cboType;
    QComboBox *_cboCategory;
    QLineEdit *_edPosX;
    QLineEdit *_edPosY;
    QLabel *_lbNotified;        // Value from device
    QSlider *_sbValue;
    QLabel *_lbValue;           // Slider value
    QLineEdit *_edText;

    WidgetSetting _uiSetting;   // from UI
    WidgetSetting _devSetting;  // from device
};


#endif // WIDGETCONTROLS_H
