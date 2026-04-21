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

// ─── Вспомогательные для task2 ───────────────────────────────────────────────

static double evalFunc_task2(int funcId, double x)
{
    return evalFunc_task1(funcId, x);
}

QString funcName_task2(int funcId)
{
    return funcName_task1(funcId);
}

struct Task3Variant {
    QString function;
    double a;
    double b;
    QString answer;
};

static const Task3Variant task3_variants[] = {
    {"1/√x", 0.0, 1.0, "да"},
    {"sin(x)/√x", 0.0, 1.0, "да"},
    {"ln(x)", 0.0, 1.0, "да"},
    {"1/√(1-x)", 0.0, 1.0, "да"},
    {"1/√(1-x²)", -1.0, 1.0, "да"},

    {"1/x", 0.0, 1.0, "нет"},
    {"1/x²", 0.0, 1.0, "нет"},
    {"1/(1-x)", 0.0, 1.0, "нет"},
    {"1/(x-1)²", 0.0, 2.0, "нет"},
    {"1/x^1.5", 0.0, 1.0, "нет"},

    {"x²", 0.0, 1.0, "да"},
    {"x³", 0.0, 1.0, "да"},
    {"sin(x)", 0.0, 3.14159, "да"},
    {"cos(x)", 0.0, 3.14159, "да"},
    {"e^x", 0.0, 2.0, "да"},
    {"2x+1", 0.0, 2.0, "да"}
};

static const int TASK3_COUNT = sizeof(task3_variants) / sizeof(Task3Variant);

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

QString get_random_task3()
{
    int idx = QRandomGenerator::global()->bounded(0, TASK3_COUNT);
    const Task3Variant& v = task3_variants[idx];

    return QString("%1||%2||%3||%4")
        .arg(v.function)
        .arg(v.a, 0, 'f', 3)
        .arg(v.b, 0, 'f', 3)
        .arg(v.answer);
}
// Метод средних прямоугольников
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

// Метод левых прямоугольников
double solver_task2(const QString& params)
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
    for (int i = 0; i < n; ++i) {
        double x_i = a + i * h;
        result += evalFunc_task2(funcId, x_i);
    }

    return result * h;
}

QString solver_task3(const QString& params)
{
    QStringList parts = params.split("||");
    if (parts.size() < 4) return "нет";
    return parts[3];
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

    QString taskParams = get_random_task1();
    QStringList p = taskParams.split("||");

    DataBase::getInstance()->updateCurrTask(login, 2);
    DataBase::getInstance()->updateParams(login, taskParams);

    qDebug() << "[Server] task2 для" << login << "| params:" << taskParams;

    return QString("task2_OK||%1||%2||%3||%4")
        .arg(funcName_task1(p[0].toInt()))      // использую тот же генератор, что и в task1
        .arg(p[1].toDouble() / 100.0, 0, 'f', 2)
        .arg(p[2].toDouble() / 100.0, 0, 'f', 2)
        .arg(p[3]);
}

QString task3(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "task3_Error||не авторизован";

    QString taskParams = get_random_task3();
    QStringList p = taskParams.split("||");

    DataBase::getInstance()->updateCurrTask(login, 3);
    DataBase::getInstance()->updateParams(login, taskParams);

    qDebug() << "[Server] task3 для" << login << "| params:" << taskParams;

    return QString("task3_OK||%1||%2||%3")
        .arg(p[0])
        .arg(p[1])
        .arg(p[2]);
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
        return "check_Error||нет активного задания, сначала запросите задание";

    int     currTask   = taskData[0].toInt();
    QString taskParams = taskData[1];

    // ─── Task3: строковый ответ да/нет ───────────────────────────────────────
    if (currTask == 3) {
        QString userAnswer   = params[0].trimmed().toLower();
        QString correctAnswer = solver_task3(taskParams).toLower();

        bool isCorrect = (userAnswer == correctAnswer);

        qDebug() << "[Task] Пользователь:" << login
                 << "| задание: 3"
                 << "| ответ:" << userAnswer
                 << "| верно:" << isCorrect;

        DataBase::getInstance()->updateTaskScore(login, 3, isCorrect ? 1 : -1);
        DataBase::getInstance()->clearTaskState(login);

        if (isCorrect)
            return QString("check_OK||%1").arg(correctAnswer);
        else
            return QString("check_False||%1||%2")
                .arg(userAnswer)
                .arg(correctAnswer);
    }

    // ─── Task1, Task2: числовой ответ ────────────────────────────────────────
    bool ok;
    double userAnswer = params[0].trimmed().replace(",", ".").toDouble(&ok);
    if (!ok) return "check_Error||некорректный ответ, введите число (например: 0.33)";

    double correctAnswer = 0.0;
    if (currTask == 1)
        correctAnswer = solver_task1(taskParams);
    else if (currTask == 2)
        correctAnswer = solver_task2(taskParams);
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
        return QString("check_OK||%1").arg(correctAnswer, 0, 'f', 4);
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
