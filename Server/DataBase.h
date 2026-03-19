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

    // Существующие методы
    QString authUser(const QString& login, const QString& password);
    bool registerUser(const QString& login, const QString& password, const QString& email);
    bool updateSocketID(const QString& login, const QString& socketID);
    QString getStatsByLogin(const QString& login);
    bool incrementTask(const QString& login, int taskNum);
    QString getLoginBySocket(const QString& socketID);
    bool logoutUser(const QString& login);

    // ★ НОВЫЕ методы для системы заданий ★
    bool updateCurrTask(const QString& login, int taskNum);
    bool updateParams(const QString& login, const QString& params);
    QStringList getCurrTaskAndParams(const QString& login);
    bool updateTaskScore(const QString& login, int taskNum, int delta); // delta: +1 или -1
    bool clearTaskState(const QString& login); // очистить currtask + params

private:
    DataBase() = default;
    DataBase(const DataBase&)            = delete;
    DataBase& operator=(const DataBase&) = delete;

    bool createTables();
    QSqlDatabase getConnection();

    QString  m_dbPath;
    QMutex   m_mutex;
};

#endif // DATABASE_H
