#include "functions.h"
#include <QCryptographicHash>
#include <QStringList>

QString Functions::hashPassword(const QString &password)
{
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(), QCryptographicHash::Sha512);
    return hash.toHex();
}

QString Functions::buildAuthRequest(const QString &login,
                                    const QString &password)
{
    // формат: auth||login||password(уже хеш)
    return QString("auth||%1||%2").arg(login, password);
}

QString Functions::buildRegisterRequest(const QString &login,
                                        const QString &email,
                                        const QString &password1,
                                        const QString &password2)
{
    // формат: reg||login||email||password1||password2
    return QString("reg||%1||%2||%3||%4")
        .arg(login, email, password1, password2);
}

QString Functions::buildTaskRequest(int taskNum)
{
    switch (taskNum) {
    case 1: return "task1";
    case 2: return "task2";
    case 3: return "task3";
    case 4: return "task4";
    default: return "task1";
    }
}

QString Functions::buildCheckTaskRequest(const QString &answer)
{
    // формат: check_task||ответ
    return QString("check_task||%1").arg(answer);
}

QString Functions::buildStatRequest()
{
    return "stat";
}

QString Functions::buildLogoutRequest()
{
    return "logout";
}

ParsedResponse Functions::parseResponse(const QString &msg)
{
    ParsedResponse r;

    QString trimmed = msg.trimmed();
    QStringList parts = trimmed.split("||");
    if (parts.isEmpty())
        return r;

    QString prefix = parts[0];

    auto take = [&parts](int index) -> QString {
        return (parts.size() > index) ? parts[index] : QString();
    };

    if (prefix == "auth_OK") {
        r.type = ResponseType::AuthOk;
    } else if (prefix.startsWith("auth_Error")) {
        r.type = ResponseType::AuthError;
        r.message = take(1);
    } else if (prefix == "auth_False") {
        r.type = ResponseType::AuthError;
        r.message = QStringLiteral("Неверный логин или пароль");
    } else if (prefix == "reg_OK") {
        r.type = ResponseType::RegOk;
    } else if (prefix.startsWith("reg_Error")) {
        r.type = ResponseType::RegError;
        r.message = take(1);
    } else if (prefix == "task1_OK") {
        // task1_OK||funcName||a||b||n
        r.type = ResponseType::Task1Ok;
        r.funcName = take(1);
        r.a = take(2).toDouble();
        r.b = take(3).toDouble();
        r.n = take(4).toInt();
    } else if (prefix.startsWith("task2_Info") ||
               prefix.startsWith("task3_Info") ||
               prefix.startsWith("task4_Info")) {
        r.type = ResponseType::TaskInfo;
        r.message = take(1);
    } else if (prefix == "check_OK") {
        // check_OK||correct
        r.type = ResponseType::CheckOk;
        r.correctAnswer = take(1).toDouble();
        r.userAnswer = r.correctAnswer;
        r.message = QStringLiteral("Ответ верный");
    } else if (prefix == "check_False") {
        // check_False||user||correct
        r.type = ResponseType::CheckFalse;
        r.userAnswer = take(1).toDouble();
        r.correctAnswer = take(2).toDouble();
        r.message = QStringLiteral("Ответ неверный");
    } else if (prefix.startsWith("check_Error")) {
        r.type = ResponseType::CheckError;
        r.message = take(1);
    } else if (prefix == "stat_OK") {
        // stat_OK||login||t1||t2||t3||t4
        r.type = ResponseType::StatOk;
        r.login = take(1);
        r.t1 = take(2).toInt();
        r.t2 = take(3).toInt();
        r.t3 = take(4).toInt();
        r.t4 = take(5).toInt();
    } else if (prefix == "stat_Error") {
        r.type = ResponseType::CheckError;
        r.message = take(1);
    } else if (prefix == "logout_OK") {
        r.type = ResponseType::LogoutOk;
    } else if (prefix.startsWith("logout_Error")) {
        r.type = ResponseType::LogoutError;
        r.message = take(1);
    } else {
        r.type = ResponseType::Unknown;
        r.message = msg;
    }

    return r;
}
