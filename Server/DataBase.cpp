#include "DataBase.h"
#include <QThread>

DataBase* DataBase::getInstance()
{
    static DataBase instance;
    return &instance;
}

bool DataBase::init(const QString& dbPath)
{
    m_dbPath = dbPath;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "main_thread");
    db.setDatabaseName(m_dbPath);

    if (!db.open())
    {
        qCritical() << "[DataBase] Ошибка открытия БД:" << db.lastError().text();
        return false;
    }

    // Включение WAL и busy timeout
    QSqlQuery pragma(db);
    pragma.exec("PRAGMA journal_mode=WAL;");
    pragma.exec("PRAGMA busy_timeout=5000;");

    qDebug() << "[DataBase] БД открыта:" << dbPath;

    QSqlQuery query(db);
    bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS Person ("
        "  login    VARCHAR(40) NOT NULL PRIMARY KEY,"
        "  password CHAR(128)   NOT NULL,"
        "  email    VARCHAR(40) NOT NULL UNIQUE,"
        "  role     VARCHAR(40) NOT NULL DEFAULT 'user',"
        "  socketID VARCHAR(40),"
        "  task1    INTEGER NOT NULL DEFAULT 0,"
        "  task2    INTEGER NOT NULL DEFAULT 0,"
        "  task3    INTEGER NOT NULL DEFAULT 0,"
        "  task4    INTEGER NOT NULL DEFAULT 0,"
        "  currtask INTEGER,"
        "  params   VARCHAR(40)"
        ")"
        );

    if (!ok)
    {
        QSqlError err = query.lastError();
        qCritical() << "[DataBase] Ошибка создания таблицы:" << err.text();
        // SQLITE_BUSY/SQLITE_LOCKED будут в тексте err.text(),
        // не можем пользоваться err.number() в старых Qt
        return false;
    }
    else
        qDebug() << "[DataBase] Таблица Person готова";

    return ok;
}

QSqlDatabase DataBase::getConnection()
{
    QString name = QString("connection_%1").arg((quintptr)QThread::currentThreadId());

    if (QSqlDatabase::contains(name))
        return QSqlDatabase::database(name);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", name);
    db.setDatabaseName(m_dbPath);

    if (!db.open())
        qCritical() << "[DataBase] getConnection ошибка:" << db.lastError().text();

    qDebug() << "[DataBase] Новое соединение для потока:" << name;

    return db;
}

QString DataBase::authUser(const QString& login, const QString& password)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(getConnection());
    query.prepare("SELECT login FROM Person WHERE login = :login AND password = :password");
    query.bindValue(":login",    login);
    query.bindValue(":password", password);

    if (!query.exec())
    {
        QSqlError err = query.lastError();
        qWarning() << "[DataBase] authUser ошибка:" << err.text();
        // SQLITE_BUSY/SQLITE_LOCKED будут в тексте err.text()
        return "";
    }

    if (query.next())
        return "OK";

    return "";
}

bool DataBase::registerUser(const QString& login,
                            const QString& password,
                            const QString& email)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(getConnection());
    query.prepare(
        "INSERT INTO Person (login, password, email, role, task1, task2, task3, task4) "
        "VALUES (:login, :password, :email, 'user', 0, 0, 0, 0)"
        );
    query.bindValue(":login",    login);
    query.bindValue(":password", password);
    query.bindValue(":email",    email);

    if (!query.exec())
    {
        QSqlError err = query.lastError();
        QString errText = err.text();
        qWarning() << "[DataBase] registerUser ошибка:" << errText;

        if (errText.contains("Person.login", Qt::CaseInsensitive))
            qWarning() << "[DataBase] registerUser: логин уже занят:" << login;
        else if (errText.contains("Person.email", Qt::CaseInsensitive))
            qWarning() << "[DataBase] registerUser: email уже занят:" << email;
        else
            qWarning() << "[DataBase] registerUser ошибка:" << errText;

        return false;
    }

    return true;
}

bool DataBase::updateSocketID(const QString& login, const QString& socketID)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(getConnection());
    query.prepare("UPDATE Person SET socketID = :sid WHERE login = :login");
    query.bindValue(":sid",   socketID);
    query.bindValue(":login", login);

    if (!query.exec())
    {
        QSqlError err = query.lastError();
        qWarning() << "[DataBase] updateSocketID ошибка:" << err.text();
        return false;
    }

    return true;
}

QString DataBase::getStatsByLogin(const QString& login)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(getConnection());
    query.prepare("SELECT login, task1, task2, task3, task4 FROM Person WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec() || !query.next())
    {
        QSqlError err = query.lastError();
        qWarning() << "[DataBase] getStatsByLogin ошибка:" << err.text();
        return "ERROR: пользователь не найден\r\n";
    }

    return QString("login: %1 | task1: %2 | task2: %3 | task3: %4 | task4: %5\r\n")
        .arg(query.value(0).toString())
        .arg(query.value(1).toInt())
        .arg(query.value(2).toInt())
        .arg(query.value(3).toInt())
        .arg(query.value(4).toInt());
}

bool DataBase::incrementTask(const QString& login, int taskNum)
{
    QMutexLocker locker(&m_mutex);

    if (taskNum < 1 || taskNum > 4)
    {
        return false;
    }

    QString col = QString("task%1").arg(taskNum);

    QSqlQuery query(getConnection());
    query.prepare(QString("UPDATE Person SET %1 = %1 + 1 WHERE login = :login").arg(col));
    query.bindValue(":login", login);

    if (!query.exec())
    {
        QSqlError err = query.lastError();
        qWarning() << "[DataBase] incrementTask ошибка:" << err.text();
        return false;
    }

    return true;
}

QString DataBase::getLoginBySocket(const QString& socketID)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(getConnection());
    query.prepare("SELECT login FROM Person WHERE socketID = :sid");
    query.bindValue(":sid", socketID);

    if (!query.exec() || !query.next())
        return "";

    return query.value(0).toString();
}

bool DataBase::logoutUser(const QString& login)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(getConnection());
    query.prepare("UPDATE Person SET socketID = NULL, currtask = NULL, params = NULL WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec())
    {
        QSqlError err = query.lastError();
        qWarning() << "[DataBase] logoutUser ошибка:" << err.text();
        return false;
    }

    return true;
}

bool DataBase::updateCurrTask(const QString& login, int taskNum)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(getConnection());
    query.prepare("UPDATE Person SET currtask = :ct WHERE login = :login");
    query.bindValue(":ct",    taskNum);
    query.bindValue(":login", login);

    if (!query.exec())
    {
        QSqlError err = query.lastError();
        qWarning() << "[DataBase] updateCurrTask ошибка:" << err.text();
        return false;
    }
    return true;
}

bool DataBase::updateParams(const QString& login, const QString& params)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(getConnection());
    query.prepare("UPDATE Person SET params = :params WHERE login = :login");
    query.bindValue(":params", params);
    query.bindValue(":login",  login);

    if (!query.exec())
    {
        QSqlError err = query.lastError();
        qWarning() << "[DataBase] updateParams ошибка:" << err.text();
        return false;
    }
    return true;
}

QStringList DataBase::getCurrTaskAndParams(const QString& login)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(getConnection());
    query.prepare("SELECT currtask, params FROM Person WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec() || !query.next())
        return QStringList();

    return QStringList()
           << query.value(0).toString()   // currtask
           << query.value(1).toString();  // params
}

bool DataBase::updateTaskScore(const QString& login, int taskNum, int delta)
{
    QMutexLocker locker(&m_mutex);

    if (taskNum < 1 || taskNum > 4)
        return false;

    QString col = QString("task%1").arg(taskNum);

    QSqlQuery query(getConnection());
    // delta может быть +1 или -1 (поле принимает отрицательные значения)
    query.prepare(QString("UPDATE Person SET %1 = %1 + :delta WHERE login = :login").arg(col));
    query.bindValue(":delta", delta);
    query.bindValue(":login", login);

    if (!query.exec())
    {
        QSqlError err = query.lastError();
        qWarning() << "[DataBase] updateTaskScore ошибка:" << err.text();
        return false;
    }
    return true;
}

bool DataBase::clearTaskState(const QString& login)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(getConnection());
    query.prepare("UPDATE Person SET currtask = NULL, params = NULL WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec())
    {
        QSqlError err = query.lastError();
        qWarning() << "[DataBase] clearTaskState ошибка:" << err.text();
        return false;
    }
    return true;
}
