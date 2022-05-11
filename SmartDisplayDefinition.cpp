#include <SmartDisplayDefinition.h>


QString GetItemName(int idx, QString items[], int count)
{
    if(idx >=0 && idx < count) {
        return items[idx];
    }
    else {
        return "";
    }
}

static QString widgetTypeNames[] = {
    "Empty",            // 0,
    "Image",            // 1,
    "Gauge",            // 2,
    "BDI",              // 3,
    "Button",           // 4,
    "ToggleButton",     // 5,
    "VerticalSlider",   // 6,
    "HorizontalSlider", // 7,
    "CheckButton",      // 8,
    "Temperature",      // 9,
    "Battery",          // 10,
    "Graph",            // 11,
    "Indicator",        // 12,
    "CircleProgress",   // 13,
    "ImageProgress",    // 14,
    "GroupButton",      // 15,
    "AnimatedImage",    // 16,
    "NumberStr",        // 17,
    "Text",             // 18,
    "CustomWidget",     // 19,
    "DigitalClock",     // 20,
    "PhysicalButton",   // 21,
    "MultiState",       // 22,
};

QString GetWidgetTypeName(WidgetType type)
{
    return GetItemName((int)type, widgetTypeNames, ARRAYSIZE(widgetTypeNames));
}

/*
 *  SmartDisplayDeviceType
 */
static QString smartDisplayDeviceTypeNames[] =
{
    "SmartDisplay5",
    "SmartDisplay7",
    "SmartDisplay3.9",
    "SmartDisplay4.3",
    "SmartDisplay3.5",
    "SmartDisplay10.1",
    "SmartDisplay2.4",
    "SmartDisplayOLED2.42",
    "SmartDisplayOLED3.55",
    "SmartDisplay4.3C",
    "SmartDisplayOLED2.42U",
    "SmartDisplay10.1U",
    "SmartDisplay5U",
    "SmartDisplay7J",
};

QString GetSmartDisplayDeviceTypeName(SmartDisplayDeviceType type)
{
    return GetItemName((int)type, smartDisplayDeviceTypeNames, ARRAYSIZE(smartDisplayDeviceTypeNames));
}

/*
 *  InterfaceType
 */
static QString interfaceTypeNames[] =
{
    "CANbus",
    "RS485",
    "UART",
    "Bluetooth",
};

QString GetInterfaceTypeName(InterfaceType type)
{
    return GetItemName((int)type, interfaceTypeNames, ARRAYSIZE(interfaceTypeNames));
}

