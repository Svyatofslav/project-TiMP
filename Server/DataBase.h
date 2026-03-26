#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QDebug>
#include <QThread>

class DataBase
{
public:
    static DataBase* getInstance();

    bool init(const QString& dbPath = "timp.db");
    void closeThreadConnection();   // вызывать в конце каждого QThread::run()

    // Авторизация / регистрация
    QString authUser(const QString& login, const QString& password);
    bool    registerUser(const QString& login, const QString& password, const QString& email);
    QString checkLoginOrEmail(const QString& login, const QString& email);

    // Сессии (socketID хранится в БД)
    bool    updateSocketID(const QString& login, const QString& socketID);
    QString getLoginBySocket(const QString& socketID);
    bool    logoutUser(const QString& login);

    // Статистика
    QString getStatsByLogin(const QString& login);

    // Задания
    bool        updateCurrTask(const QString& login, int taskNum);
    bool        updateParams(const QString& login, const QString& params);
    QStringList getCurrTaskAndParams(const QString& login);
    bool        updateTaskScore(const QString& login, int taskNum, int delta);
    bool        clearTaskState(const QString& login);

private:
    DataBase() = default;
    DataBase(const DataBase&)            = delete;
    DataBase& operator=(const DataBase&) = delete;

    QString m_dbPath;
    QMutex  m_connMutex;  // защита только при создании соединений

    QSqlDatabase dbForThread();
    QString      threadConnName() const;
};

#endif // DATABASE_H
