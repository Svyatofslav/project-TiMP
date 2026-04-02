#include "DataBase.h"

DataBase* DataBase::getInstance()
{
    static DataBase instance;
    return &instance;
}

QString DataBase::threadConnName() const
{
    return QString("conn_%1")
    .arg(reinterpret_cast<qulonglong>(QThread::currentThreadId()));
}

// Потокобезопасный: каждый поток получает своё соединение
QSqlDatabase DataBase::dbForThread()
{
    const QString name = threadConnName();

    // Быстрый путь: соединение уже есть
    if (QSqlDatabase::contains(name))
        return QSqlDatabase::database(name);

    // Медленный путь: создаём новое соединение под мьютексом
    QMutexLocker locker(&m_connMutex);

    // Double-checked locking
    if (QSqlDatabase::contains(name))
        return QSqlDatabase::database(name);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", name);
    db.setDatabaseName(m_dbPath);

    if (!db.open()) {
        qCritical() << "[DataBase] Поток" << QThread::currentThreadId()
            << "не смог открыть БД:" << db.lastError().text();
        return QSqlDatabase();
    }

    QSqlQuery pragma(db);
    pragma.exec("PRAGMA journal_mode=WAL;");
    pragma.exec("PRAGMA synchronous=NORMAL;");
    pragma.exec("PRAGMA foreign_keys=ON;");
    pragma.exec("PRAGMA busy_timeout=5000;");

    qDebug() << "[DataBase] Новое соединение для потока" << QThread::currentThreadId();
    return db;
}

void DataBase::closeThreadConnection()
{
    const QString name = threadConnName();
    if (!QSqlDatabase::contains(name))
        return;

    {
        QSqlDatabase db = QSqlDatabase::database(name);
        db.close();
    } // db уничтожен здесь — обязательно до removeDatabase

    QMutexLocker locker(&m_connMutex);
    QSqlDatabase::removeDatabase(name);
    qDebug() << "[DataBase] Соединение закрыто для потока" << QThread::currentThreadId();
}

bool DataBase::init(const QString& dbPath)
{
    QMutexLocker locker(&m_connMutex);
    m_dbPath = dbPath;

    const QString initConn = "init_connection";
    bool success = false;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", initConn);
        db.setDatabaseName(m_dbPath);

        if (!db.open()) {
            qCritical() << "[DataBase] Ошибка открытия БД:" << db.lastError().text();
        } else {
            QSqlQuery pragma(db);
            pragma.exec("PRAGMA journal_mode=WAL;");
            pragma.exec("PRAGMA synchronous=NORMAL;");
            pragma.exec("PRAGMA cache_size=10000;");
            pragma.exec("PRAGMA foreign_keys=ON;");
            pragma.exec("PRAGMA busy_timeout=5000;");

            QSqlQuery query(db);
            bool ok = query.exec(
                "CREATE TABLE IF NOT EXISTS Person ("
                "  login    VARCHAR(40)  NOT NULL PRIMARY KEY,"
                "  password CHAR(128)    NOT NULL,"
                "  email    VARCHAR(100) NOT NULL UNIQUE,"
                "  role     VARCHAR(40)  NOT NULL DEFAULT 'user',"
                "  socketID VARCHAR(40),"
                "  task1    INTEGER NOT NULL DEFAULT 0,"
                "  task2    INTEGER NOT NULL DEFAULT 0,"
                "  task3    INTEGER NOT NULL DEFAULT 0,"
                "  task4    INTEGER NOT NULL DEFAULT 0,"
                "  currtask INTEGER,"
                "  params   VARCHAR(256)"
                ")"
                );

            if (!ok) {
                qCritical() << "[DataBase] Ошибка создания таблицы:"
                            << query.lastError().text();
            } else {
                // Сброс незавершённых заданий при перезапуске сервера
                QSqlQuery cleanup(db);
                cleanup.exec(
                    "UPDATE Person SET currtask = NULL, params = NULL "
                    "WHERE currtask IS NOT NULL OR params IS NOT NULL");
                qDebug() << "[DataBase] Сброшено незавершённых заданий:"
                         << cleanup.numRowsAffected();

                // Сброс socketID при перезапуске
                QSqlQuery socketCleanup(db);
                socketCleanup.exec(
                    "UPDATE Person SET socketID = NULL WHERE socketID IS NOT NULL");
                qDebug() << "[DataBase] Сброшено активных сессий:"
                         << socketCleanup.numRowsAffected();

                success = true;
            }
            db.close();
        }
    } // db уничтожен здесь

    QSqlDatabase::removeDatabase(initConn);

    if (success)
        qDebug() << "[DataBase] init() завершён, dbPath =" << m_dbPath;

    return success;
}

QString DataBase::authUser(const QString& login, const QString& password)
{
    QSqlDatabase db = dbForThread();
    QSqlQuery query(db);
    query.prepare(
        "SELECT login FROM Person WHERE login = :login AND password = :password LIMIT 1");
    query.bindValue(":login",    login);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qWarning() << "[DataBase] authUser ошибка:" << query.lastError().text();
        return "";
    }
    return query.next() ? "OK" : "";
}

bool DataBase::registerUser(const QString& login, const QString& password,
                            const QString& email)
{
    QSqlDatabase db = dbForThread();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO Person (login, password, email, role, task1, task2, task3, task4) "
        "VALUES (:login, :password, :email, 'user', 0, 0, 0, 0)");
    query.bindValue(":login",    login);
    query.bindValue(":password", password);
    query.bindValue(":email",    email);

    if (!query.exec()) {
        qWarning() << "[DataBase] registerUser ошибка:" << query.lastError().text();
        return false;
    }
    return true;
}

QString DataBase::checkLoginOrEmail(const QString& login, const QString& email)
{
    QSqlDatabase db = dbForThread();
    QSqlQuery query(db);
    query.prepare(
        "SELECT 'login' FROM Person WHERE login = :login "
        "UNION ALL SELECT 'email' FROM Person WHERE email = :email LIMIT 1");
    query.bindValue(":login", login);
    query.bindValue(":email", email);

    if (!query.exec() || !query.next()) return "";
    return query.value(0).toString();
}

bool DataBase::updateSocketID(const QString& login, const QString& socketID)
{
    QSqlDatabase db = dbForThread();
    QSqlQuery query(db);
    query.prepare("UPDATE Person SET socketID = :sid WHERE login = :login");
    query.bindValue(":sid",   socketID);
    query.bindValue(":login", login);

    if (!query.exec()) {
        qWarning() << "[DataBase] updateSocketID ошибка:" << query.lastError().text();
        return false;
    }
    return true;
}

QString DataBase::getLoginBySocket(const QString& socketID)
{
    QSqlDatabase db = dbForThread();
    QSqlQuery query(db);
    query.prepare("SELECT login FROM Person WHERE socketID = :sid LIMIT 1");
    query.bindValue(":sid", socketID);

    if (!query.exec() || !query.next()) return "";
    return query.value(0).toString();
}

bool DataBase::logoutUser(const QString& login)
{
    QSqlDatabase db = dbForThread();
    QSqlQuery query(db);
    query.prepare("UPDATE Person SET socketID = NULL WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec()) {
        qWarning() << "[DataBase] logoutUser ошибка:" << query.lastError().text();
        return false;
    }
    return true;
}

QString DataBase::getStatsByLogin(const QString& login)
{
    QSqlDatabase db = dbForThread();
    QSqlQuery query(db);
    query.prepare(
        "SELECT login, task1, task2, task3, task4 FROM Person WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec() || !query.next())
        return "stat_Error||пользователь не найден";

    // Формат: stat_OK||login||task1||task2||task3||task4
    // GUI парсит по "||"
    return QString("stat_OK||%1||%2||%3||%4||%5")
        .arg(query.value(0).toString())
        .arg(query.value(1).toInt())
        .arg(query.value(2).toInt())
        .arg(query.value(3).toInt())
        .arg(query.value(4).toInt());
}

bool DataBase::updateCurrTask(const QString& login, int taskNum)
{
    QSqlDatabase db = dbForThread();
    QSqlQuery query(db);
    query.prepare("UPDATE Person SET currtask = :ct WHERE login = :login");
    query.bindValue(":ct",    taskNum);
    query.bindValue(":login", login);
    return query.exec();
}

bool DataBase::updateParams(const QString& login, const QString& params)
{
    QSqlDatabase db = dbForThread();
    QSqlQuery query(db);
    query.prepare("UPDATE Person SET params = :params WHERE login = :login");
    query.bindValue(":params", params);
    query.bindValue(":login",  login);
    return query.exec();
}

QStringList DataBase::getCurrTaskAndParams(const QString& login)
{
    QSqlDatabase db = dbForThread();
    QSqlQuery query(db);
    query.prepare("SELECT currtask, params FROM Person WHERE login = :login");
    query.bindValue(":login", login);

    QStringList result;
    if (!query.exec() || !query.next()) return result;
    result << query.value(0).toString() << query.value(1).toString();
    return result;
}

bool DataBase::updateTaskScore(const QString& login, int taskNum, int delta)
{
    if (taskNum < 1 || taskNum > 4) return false;

    QSqlDatabase db = dbForThread();
    QString col = QString("task%1").arg(taskNum);
    QSqlQuery query(db);
    query.prepare(
        QString("UPDATE Person SET %1 = %1 + :delta WHERE login = :login").arg(col));
    query.bindValue(":delta", delta);
    query.bindValue(":login", login);
    return query.exec();
}

bool DataBase::clearTaskState(const QString& login)
{
    QSqlDatabase db = dbForThread();
    QSqlQuery query(db);
    query.prepare(
        "UPDATE Person SET currtask = NULL, params = NULL WHERE login = :login");
    query.bindValue(":login", login);
    return query.exec();
}
