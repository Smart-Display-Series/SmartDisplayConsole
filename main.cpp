#include "smartdisplayconsole.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SmartDisplayConsole w;
    w.show();
    return a.exec();
}
