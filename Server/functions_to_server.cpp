#include "functions_to_server.h"
#include "DataBase.h"
#include <QCryptographicHash>

QString handleSHA512(const QString& payload)
{
    QByteArray hash = QCryptographicHash::hash(
        payload.toUtf8(),
        QCryptographicHash::Sha512
        );
    return hash.toHex();
}

QString parsing(QString str, int descriptor)
{
    str = str.trimmed();
    QStringList parts = str.split("||");
    if (parts.isEmpty()) return "ERROR: пустой запрос\r\n";

    QString func = parts[0];
    parts.removeFirst();

    qDebug() << "[Server] Запрос:" << func << "| params:" << parts;

    if      (func == "task1")         return task1(parts, descriptor);
    else if (func == "task2")         return task2(parts, descriptor);
    else if (func == "task3")         return task3(parts, descriptor);
    else if (func == "task4")         return task4(parts, descriptor);
    else if (func == "authorization") return author(parts, descriptor);
    else if (func == "registration")  return reg(parts, descriptor);
    else if (func == "statistics")    return stat(parts, descriptor);
    else if (func == "logout")        return logout(parts, descriptor);

    return "ERROR: неизвестная команда\r\n";
}

QString task1(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n";

    DataBase::getInstance()->incrementTask(login, 1);
    return "task1_OK\r\n";
}

QString task2(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n";

    DataBase::getInstance()->incrementTask(login, 2);
    return "task2_OK\r\n";
}

QString task3(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n";

    DataBase::getInstance()->incrementTask(login, 3);
    return "task3_OK\r\n";
}

QString task4(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n";

    DataBase::getInstance()->incrementTask(login, 4);
    return "task4_OK\r\n";
}

// format: "authorization||login||password"
QString author(QStringList params, int descriptor)
{

    // Проверка — вдруг пользователь уже авторизован
    QString alreadyLogged = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (!alreadyLogged.isEmpty())
        return "author_Error: вы уже авторизованы как " + alreadyLogged + "\r\n";

    if (params.size() < 2) return "ERROR: нужны login и password\r\n";

    QString login    = params[0];
    QString password = params[1];

    QString hashedPassword = handleSHA512(password);
    QString result = DataBase::getInstance()->authUser(login, hashedPassword);

    if (!result.isEmpty())
    {
        DataBase::getInstance()->updateSocketID(login, QString::number(descriptor));
        return "auth_OK\r\n";
    }

    return "auth_False\r\n";
}

// format: "registration||login||email||password1||password2"
QString reg(QStringList params, int descriptor)
{

    // Проверка — вдруг пользователь уже авторизован
    QString alreadyLogged = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (!alreadyLogged.isEmpty())
        return "reg_Error: нельзя зарегистрироваться, вы уже авторизованы\r\n";

    if (params.size() < 4)
        return "ERROR: нужны login, email, password1, password2\r\n";

    QString login     = params[0];
    QString email     = params[1];
    QString password1 = params[2];
    QString password2 = params[3];

    if (login.isEmpty() || password1.isEmpty() || email.isEmpty())
        return "reg_Error: поля не могут быть пустыми\r\n";

    if (login.length() > 40)
        return "reg_Error: логин слишком длинный\r\n";

    if (!email.contains("@") || !email.contains("."))
        return "reg_Error: некорректный email\r\n";

    if (password1 != password2)
        return "reg_Error: пароли не совпадают\r\n";

    QString hashedPassword = handleSHA512(password1);

    bool ok = DataBase::getInstance()->registerUser(login, hashedPassword, email);

    if (ok)
    {
        DataBase::getInstance()->updateSocketID(login, QString::number(descriptor));
        return "reg_OK\r\n";
    }

    return "reg_Error: логин или email уже заняты\r\n";
}


// format: "statistics"
QString stat(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n";

    return DataBase::getInstance()->getStatsByLogin(login);
}

// format: "logout"
QString logout(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n";

    DataBase::getInstance()->logoutUser(login);
    return "logout_OK\r\n";
}
