#include "configuring_network_gui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    configuring_network_gui w;
    w.show();
    return a.exec();
}
