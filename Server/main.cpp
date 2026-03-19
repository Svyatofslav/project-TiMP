#include <QCoreApplication>
#include <QDebug>
#include "server.h"
#include "DataBase.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Путь по умолчанию (локальный запуск)
    QString dbPath = "timp.db";

    // Если запущено в Docker и передана переменная окружения DB_PATH,
    // используем её (например: /app/data/timp.db)
    QByteArray envPath = qgetenv("DB_PATH");
    if (!envPath.isEmpty())
        dbPath = QString::fromUtf8(envPath);

    qDebug() << "Используем БД по пути:" << dbPath;

    if (!DataBase::getInstance()->init(dbPath))
    {
        qCritical() << "Не удалось открыть БД, завершение";
        return 1;
    }

    Server server;
    server.startServer(44444);

    return a.exec();
}
