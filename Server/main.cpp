#ifdef _WIN32
#include <windows.h>
#endif
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include "server.h"
#include "DataBase.h"


void customMessageHandler(QtMsgType type,
                          const QMessageLogContext &context,
                          const QString &msg)
{
    Q_UNUSED(context)

    QString timestamp = QDateTime::currentDateTime()
                            .toString("yyyy-MM-dd HH:mm:ss.zzz");

    QString level;
    switch (type) {
    case QtDebugMsg:    level = "DBG"; break;
    case QtInfoMsg:     level = "INF"; break;
    case QtWarningMsg:  level = "WRN"; break;
    case QtCriticalMsg: level = "CRT"; break;
    case QtFatalMsg:    level = "FTL"; break;
    }

    QString line = QString("[%1] [%2] %3\n").arg(timestamp, level, msg);

    // Конвертируем в локальную кодировку системы (CP1251 на русской Windows)
    QByteArray local = line.toLocal8Bit();
    fprintf(stdout, "%s", local.constData());
    fflush(stdout);

    if (type == QtFatalMsg)
        abort();
}


int main(int argc, char *argv[])
{
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    qInstallMessageHandler(customMessageHandler);

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
