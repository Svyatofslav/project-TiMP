#include "functions_to_server.h"
#include "DataBase.h"
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <cmath>

//  SHA-512

QString handleSHA512(const QString& payload)
{
    QByteArray hash = QCryptographicHash::hash(
        payload.toUtf8(),
        QCryptographicHash::Sha512);
    return hash.toHex();
}

//  Парсер команд

QString parsing(QString str, int descriptor)
{
    str = str.trimmed();
    QStringList parts = str.split("||");
    if (parts.isEmpty()) return "ERROR: пустой запрос\r\n";

    QString func = parts[0];
    parts.removeFirst();

    qDebug() << "[Server] Запрос:" << func << "| params:" << parts;

    if      (func == "task1")      return task1(parts, descriptor);
    else if (func == "task2")      return task2(parts, descriptor);
    else if (func == "task3")      return task3(parts, descriptor);
    else if (func == "task4")      return task4(parts, descriptor);
    else if (func == "check_task") return check_task(parts, descriptor); // ★ НОВОЕ
    else if (func == "auth")       return author(parts, descriptor);
    else if (func == "reg")        return reg(parts, descriptor);
    else if (func == "stat")       return stat(parts, descriptor);
    else if (func == "logout")     return logout(parts, descriptor);

    return "ERROR: неизвестная команда\r\n";
}

//  task1 — вспомогательные функции

// Поддерживаемые функции (id → формула)
// 0: f(x) = x^2
// 1: f(x) = x^3
// 2: f(x) = 2x + 1
// 3: f(x) = sin(x)
// 4: f(x) = cos(x)

static double evalFunc_task1(int funcId, double x)
{
    switch (funcId)
    {
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
    switch (funcId)
    {
    case 0: return "f(x) = x^2";
    case 1: return "f(x) = x^3";
    case 2: return "f(x) = 2x + 1";
    case 3: return "f(x) = sin(x)";
    case 4: return "f(x) = cos(x)";
    default: return "f(x) = x^2";
    }
}

// Генерация случайных параметров: "funcId||a||b||n"
// a ∈ [-3;3], b = a + [1;4], n ∈ [2;8]
// Формат хранится в поле params (VARCHAR(40)) — умещается
QString get_random_task1()
{
    int funcId = QRandomGenerator::global()->bounded(0, 5); // 0-4
    int a      = QRandomGenerator::global()->bounded(-3, 4); // -3..3
    int b      = a + QRandomGenerator::global()->bounded(1, 5); // a+1..a+4
    int n      = QRandomGenerator::global()->bounded(2, 9); // 2..8

    return QString("%1||%2||%3||%4").arg(funcId).arg(a).arg(b).arg(n);
}

// Вычисление интеграла методом средних прямоугольников
// Формула: I ≈ h * Σ f(a + h*(i - 0.5)), i = 1..n,  h = (b-a)/n
double solver_task1(const QString& params)
{
    QStringList p = params.split("||");
    if (p.size() < 4) return 0.0;

    int    funcId = p[0].toInt();
    double a      = p[1].toDouble();
    double b      = p[2].toDouble();
    int    n      = p[3].toInt();

    double h      = (b - a) / n;
    double result = 0.0;

    for (int i = 1; i <= n; ++i)
    {
        double x = a + h * (i - 0.5); // середина i-го отрезка
        result  += evalFunc_task1(funcId, x);
    }

    return result * h;
}


//  task1 — get_task (выдать задание)

QString task1(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n"; // пользователь не авторизован

    // Генерируем параметры задания
    QString taskParams = get_random_task1();
    QStringList p = taskParams.split("||");

    // Сохраняем в БД: currtask = 1, params = taskParams
    DataBase::getInstance()->updateCurrTask(login, 1);
    DataBase::getInstance()->updateParams(login, taskParams);

    qDebug() << "[Server] task1 для" << login << "| params:" << taskParams;

    // Возвращаем задание пользователю
    return QString(
               "=== ЗАДАНИЕ 1: Найти значение интеграла методом средних прямоугольников ===\r\n"
               "Функция: %1\r\n"
               "Отрезок: [%2, %3]\r\n"
               "Количество отрезков n = %4\r\n"
               "Формула: I ≈ h * Σ f(a + h*(i - 0.5)), h = (b-a)/n\r\n"
               "Введите ответ (округлите до 2 знаков): check_task||ваш_ответ\r\n"
               )
        .arg(funcName_task1(p[0].toInt()))
        .arg(p[1]).arg(p[2]).arg(p[3]);
}

//  task2, task3, task4 — заготовки

QString task2(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n";

    // реализовать по аналогии с task1
    return "task2: задание в разработке\r\n";
}

QString task3(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n";

    // реализовать по аналогии с task1
    return "task3: задание в разработке\r\n";
}

QString task4(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n";

    // реализовать по аналогии с task1
    return "task4: задание в разработке\r\n";
}

//  check_task — проверка ответа пользователя

QString check_task(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n"; // не авторизован

    if (params.isEmpty() || params[0].trimmed().isEmpty())
        return "ERROR: нужен ответ (check_task||ваш_ответ)\r\n";

    // Получаем currtask и params из БД
    QStringList taskData = DataBase::getInstance()->getCurrTaskAndParams(login);

    if (taskData.size() < 2 || taskData[0].isEmpty() || taskData[1].isEmpty())
        return "ERROR: нет активного задания. Сначала запросите задание (task1)\r\n";

    int     currTask   = taskData[0].toInt();
    QString taskParams = taskData[1];

    // Разбираем ответ пользователя
    bool ok;
    double userAnswer = params[0].trimmed().replace(",", ".").toDouble(&ok);
    if (!ok) return "ERROR: некорректный ответ. Введите число (например: 0.33)\r\n";

    // Вычисляем правильный ответ
    double correctAnswer = 0.0;
    if (currTask == 1)
        correctAnswer = solver_task1(taskParams);
    // else if (currTask == 2) correctAnswer = solver_task2(taskParams);
    // else if (currTask == 3) correctAnswer = solver_task3(taskParams);
    // else if (currTask == 4) correctAnswer = solver_task4(taskParams);
    else
    {
        DataBase::getInstance()->clearTaskState(login);
        return "ERROR: неизвестный тип задания\r\n";
    }

    // Сравниваем с точностью 0.01 (2 знака после запятой)
    bool isCorrect = std::abs(userAnswer - correctAnswer) < 0.01;

    // Обновляем счёт: +1 если верно, -1 если неверно (поле может быть отрицательным)
    DataBase::getInstance()->updateTaskScore(login, currTask, isCorrect ? 1 : -1);

    // Очищаем currtask и params
    DataBase::getInstance()->clearTaskState(login);

    if (isCorrect)
    {
        return QString("Ответ верный! Правильный ответ: %1\r\n")
            .arg(correctAnswer, 0, 'f', 4);
    }
    else
    {
        return QString("Ответ неверный. Ваш ответ: %1 | Правильный ответ: %2\r\n")
            .arg(userAnswer, 0, 'f', 4)
            .arg(correctAnswer, 0, 'f', 4);
    }
}

//  Авторизация

QString author(QStringList params, int descriptor)
{
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

//  Регистрация

QString reg(QStringList params, int descriptor)
{
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

//  Статистика

QString stat(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n";

    return DataBase::getInstance()->getStatsByLogin(login);
}

//  Выход

QString logout(QStringList params, int descriptor)
{
    QString login = DataBase::getInstance()->getLoginBySocket(QString::number(descriptor));
    if (login.isEmpty()) return "ERROR: не авторизован\r\n";

    DataBase::getInstance()->logoutUser(login);
    return "logout_OK\r\n";
}
