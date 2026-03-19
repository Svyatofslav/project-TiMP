#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QDebug>

class DataBase
{
public:
    static DataBase* getInstance();

    bool init(const QString& dbPath = "timp.db");

    QString authUser(const QString& login, const QString& password);
    bool registerUser(const QString& login, const QString& password, const QString& email);
    bool updateSocketID(const QString& login, const QString& socketID);
    QString getStatsByLogin(const QString& login);
    bool incrementTask(const QString& login, int taskNum);

    // Найти логин по дескриптору сокета
    QString getLoginBySocket(const QString& socketID);

    // Очистить socketID — выход из аккаунта
    bool logoutUser(const QString& login);

private:
    DataBase() = default;
    DataBase(const DataBase&)            = delete;
    DataBase& operator=(const DataBase&) = delete;

    bool createTables();
    QSqlDatabase getConnection();

    QString      m_dbPath;
    QMutex       m_mutex;
};

#endif // DATABASE_H
