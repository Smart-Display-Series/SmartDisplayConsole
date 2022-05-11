#ifndef SMARTDISPLAYDEFINITION_H
#define SMARTDISPLAYDEFINITION_H

#include <QString>

#define ARRAYSIZE(a)    (sizeof(a)/sizeof(a[0]))

QString GetItemName(int idx, QString items[], int count);

/*
 * SmartDisplayVersion
 */
typedef struct _SmartDisplayVersion {
    QString deviceName;
    QString softwareVersion;
    QString hardwareVersion;
} SmartDisplayVersion;

/*
 *  WidgetType
 */
typedef enum {
    Empty = 0,
    Image = 1,
    Gauge = 2,
    BDI = 3,
    Button = 4,
    ToggleButton = 5,
    VerticalSlider = 6,
    HorizontalSlider = 7,
    CheckButton = 8,
    Temperature = 9,
    Battery = 10,
    Graph = 11,
    Indicator = 12,
    CircleProgress = 13,
    ImageProgress = 14,
    GroupButton = 15,
    AnimatedImage = 16,
    NumberStr = 17,
    Text = 18,
    CustomWidget = 19,
    DigitalClock = 20,
    PhysicalButton = 21,
    MultiState = 22,
} WidgetType;

#define NUM_WIDGET_TYPE 23
QString GetWidgetTypeName(WidgetType type);

/*
 *  WidgetSetting
 */
typedef struct _WidgetSetting {
    uint8_t     type;
    uint16_t    posX;
    uint16_t    posY;
    uint8_t     category;
    uint16_t    value1;
    uint16_t    value2;
    QString     textBuffer;
} WidgetSetting;

/*
 *  SmartDisplayType
 */
typedef enum {
    SmartDisplay_Unknown    = (-1),
    SmartDisplay5           = 0,
    SmartDisplay7           = 1,
    SmartDisplay3_9         = 2,
    SmartDisplay4_3         = 3,
    SmartDisplay3_5         = 4,
    SmartDisplay10_1        = 5,
    SmartDisplay2_4         = 6,
    SmartDisplayOLED2_42    = 7,
    SmartDisplayOLED3_55    = 8,
    SmartDisplay4_3C        = 9,
    SmartDisplayOLED2_42U   = 10,
    SmartDisplay10_1U       = 11,
    SmartDisplay5U          = 12,
    SmartDisplay7J          = 13,
} SmartDisplayDeviceType;

#define NUM_SMARTDISPLAY_TYPE 9
QString GetSmartDisplayDeviceTypeName(SmartDisplayDeviceType type);

/*
 *  InterfaceType
 */
typedef enum {
    CANbus = 0,
    RS485 = 1,
    UART = 2,
    Bluetooth = 3,
} InterfaceType;

#define NUM_INTERFACE_TYPE 4
QString GetInterfaceTypeName(InterfaceType type);

/*
 *  DeviceState
 */
typedef enum {
    ConfigMode = 0,
    DisplayMode = 1,
    StartView = 0xFF
} DeviceState;

typedef struct _SmartDisplayBuzzer {
    uint8_t cycle;
    uint8_t high;
    uint8_t low;
    uint8_t active;
} SmartDisplayBuzzer;


// Feature bits
#define Feature_TextString           (1 << 0)
#define Feature_PageInfoV1_0_0_1     (1 << 1)
#define Feature_DigitalClock         (1 << 2)
#define Feature_PropertySetting      (1 << 3)
#define Feature_Max64Object          (1 << 4)


#endif // SMARTDISPLAYDEFINITION_H
