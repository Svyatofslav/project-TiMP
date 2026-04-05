#include <QApplication>
#include "windowmanager.h"
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/images/Icon/icon_integral.png"));

    WindowManager manager;
    manager.showAuth(); // стартуем с окна авторизации

    return a.exec();
}
