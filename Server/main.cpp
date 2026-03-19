#include <QCoreApplication>
#include "server.h"
#include "DataBase.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // ← ОБЯЗАТЕЛЬНО до startServer()
    if (!DataBase::getInstance()->init("timp.db"))
    {
        qCritical() << "Не удалось открыть БД, завершение";
        return 1;
    }

    Server server;
    server.startServer(44444);

    return a.exec();
}
