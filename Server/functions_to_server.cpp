#include "functions_to_server.h"
#include "DataBase.h"
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <cmath>

QString handleSHA512(const QString& payload)
{
    QByteArray hash = QCryptographicHash::hash(
        payload.toUtf8(), QCryptographicHash::Sha512);
    return hash.toHex();
}

QString parsing(QString str, int descriptor)
{
    str = str.trimmed();
    QStringList parts = str.split("||");

    if (parts.size() > 20) return "pars_Error||слишком много параметров";

    QString func = parts[0];
    parts.removeFirst();

    qDebug() << "[Server] Запрос:" << func << "| params:" << parts;

    if      (func == "task1")      return task1(parts, descriptor);
    else if (func == "task2")      return task2(parts, descriptor);
    else if (func == "task3")      return task3(parts, descriptor);
    else if (func == "task4")      return task4(parts, descriptor);
    else if (func == "check_task") return check_task(parts, descriptor);
    else if (func == "auth")       return author(parts, descriptor);
    else if (func == "reg")        return reg(parts, descriptor);
    else if (func == "stat")       return stat(parts, descriptor);
    else if (func == "logout")     return logout(parts, descriptor);

    return "pars_Error||неизвестная команда";
}

// ─── Вспомогательные для task1 ───────────────────────────────────────────────

static double evalFunc_task1(int funcId, double x)
{
    switch (funcId) {
    case 0: return x * x;
    case 1: return x * x * x;
    case 2: return 2.0 * x + 1.0;
    case 3: return std::sin(x);
    case 4: return std::cos(x);
    default: return x * x;
    }
}

QString funcName_task1(int funcId)
{
    switch (funcId) {
    case 0: return "f(x) = x^2";
    case 1: return "f(x) = x^3";
    case 2: return "f(x) = 2x + 1";
    case 3: return "f(x) = sin(x)";
    case 4: return "f(x) = cos(x)";
    default: return "f(x) = x^2";
    }
}

// Параметры хранятся как целые сотые (избегаем проблем с локалью при сериализации)
QString get_random_task1()
{
    int funcId   = QRandomGenerator::global()->bounded(0, 5);
    int a_cents  = QRandomGenerator::global()->bounded(-157, 158);  // [-1.57, 1.57]
    int step     = QRandomGenerator::global()->bounded(100, 401);   // [1.00, 4.00]
    int b_cents  = a_cents + step;
    int n        = QRandomGenerator::global()->bounded(2, 9);

    return QString("%1||%2||%3||%4").arg(funcId).arg(a_cents).arg(b_cents).arg(n);
}

double solver_task1(const QString& params)
{
    QStringList p = params.split("||");
    if (p.size() < 4) return 0.0;

    int    funcId = p[0].toInt();
    double a      = p[1].toDouble() / 100.0;
    double b      = p[2].toDouble() / 100.0;
    int    n      = p[3].toInt();

    if (n <= 0) return 0.0;

    double h      = (b - a) / n;
    double result = 0.0;
    for (int i = 1; i <= n; ++i)
        result += evalFunc_task1(funcId, a + h * (i - 0.5));

    return result * h;
}

// ─── Обработчики команд ───────────────────────────────────────────────────────

QString task1(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "task1_Error||не авторизован";

    QString taskParams = get_random_task1();
    QStringList p = taskParams.split("||");

    DataBase::getInstance()->updateCurrTask(login, 1);
    DataBase::getInstance()->updateParams(login, taskParams);

    qDebug() << "[Server] task1 для" << login << "| params:" << taskParams;

    return QString("task1_OK||%1||%2||%3||%4")
        .arg(funcName_task1(p[0].toInt()))
        .arg(p[1].toDouble() / 100.0, 0, 'f', 2)
        .arg(p[2].toDouble() / 100.0, 0, 'f', 2)
        .arg(p[3]);
}

QString task2(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "task2_Error||не авторизован";
    return "task2_Info||задание в разработке";
}

QString task3(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "task3_Error||не авторизован";
    return "task3_Info||задание в разработке";
}

QString task4(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "task4_Error||не авторизован";
    return "task4_Info||задание в разработке";
}

// format: check_task||ответ
QString check_task(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "check_Error||не авторизован";

    if (params.isEmpty() || params[0].trimmed().isEmpty())
        return "check_Error||пустой ответ";

    QStringList taskData = DataBase::getInstance()->getCurrTaskAndParams(login);
    if (taskData.size() < 2 || taskData[0].isEmpty() || taskData[1].isEmpty())
        return "check_Error||нет активного задания, сначала запросите задание (task1)";

    int     currTask   = taskData[0].toInt();
    QString taskParams = taskData[1];

    bool ok;
    double userAnswer = params[0].trimmed().replace(",", ".").toDouble(&ok);
    if (!ok) return "check_Error||некорректный ответ, введите число (например: 0.33)";

    double correctAnswer = 0.0;
    if (currTask == 1)
        correctAnswer = solver_task1(taskParams);
    else {
        DataBase::getInstance()->clearTaskState(login);
        return "check_Error||неизвестный тип задания";
    }

    bool isCorrect = std::abs(userAnswer - correctAnswer) < 0.01;

    qDebug() << "[Task] Пользователь:" << login
             << "| задание:" << currTask
             << "| ответ:" << userAnswer
             << "| верно:" << isCorrect;

    DataBase::getInstance()->updateTaskScore(login, currTask, isCorrect ? 1 : -1);
    DataBase::getInstance()->clearTaskState(login);

    if (isCorrect)
        return QString("check_OK||%1")
            .arg(correctAnswer, 0, 'f', 4);
    else
        return QString("check_False||%1||%2")
            .arg(userAnswer,    0, 'f', 4)
            .arg(correctAnswer, 0, 'f', 4);
}

// format: auth||login||password
QString author(QStringList params, int descriptor)
{
    QString alreadyLogged = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (!alreadyLogged.isEmpty())
        return "auth_Error||вы уже авторизованы как " + alreadyLogged;

    if (params.size() < 2) return "auth_Error||нужны login и password";

    QString login    = params[0];
    QString password = params[1];

    QString hashedPassword = handleSHA512(password);
    QString result = DataBase::getInstance()->authUser(login, hashedPassword);

    if (!result.isEmpty()) {
        // Если у этого логина висит старый дескриптор — сбрасываем его задание
        DataBase::getInstance()->clearTaskState(login);
        DataBase::getInstance()->updateSocketID(login, QString::number(descriptor));
        qDebug() << "[Auth] Успешный вход:" << login << "| сокет:" << descriptor;
        return "auth_OK";
    }

    qWarning() << "[Auth] Неудачная попытка для логина:" << login;
    return "auth_False";
}

// format: reg||login||email||password1||password2
QString reg(QStringList params, int descriptor)
{
    QString alreadyLogged = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (!alreadyLogged.isEmpty())
        return "reg_Error||нельзя зарегистрироваться, вы уже авторизованы";

    if (params.size() < 4)
        return "reg_Error||нужны login, email, password1, password2";

    QString login     = params[0];
    QString email     = params[1];
    QString password1 = params[2];
    QString password2 = params[3];

    if (login.isEmpty() || password1.isEmpty() || email.isEmpty())
        return "reg_Error||поля не могут быть пустыми";

    if (login.length() > 40)
        return "reg_Error||логин слишком длинный";

    if (!email.contains("@") || !email.contains("."))
        return "reg_Error||некорректный email";

    if (password1 != password2)
        return "reg_Error||пароли не совпадают";

    // Проверка до INSERT — точное сообщение об ошибке
    QString existing = DataBase::getInstance()->checkLoginOrEmail(login, email);
    if (existing == "login") return "reg_Error||логин уже занят";
    if (existing == "email") return "reg_Error||email уже занят";

    QString hashedPassword = handleSHA512(password1);
    bool ok = DataBase::getInstance()->registerUser(login, hashedPassword, email);

    if (ok) {
        DataBase::getInstance()->updateSocketID(login, QString::number(descriptor));
        qDebug() << "[Reg] Новый пользователь:" << login << "| email:" << email;
        return "reg_OK";
    }

    return "reg_Error||ошибка регистрации";
}

// format: stat
QString stat(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "stat_Error||не авторизован";

    return DataBase::getInstance()->getStatsByLogin(login);
}

// format: logout
QString logout(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "logout_Error||не авторизован";

    DataBase::getInstance()->clearTaskState(login);
    DataBase::getInstance()->logoutUser(login);
    return "logout_OK";
}
