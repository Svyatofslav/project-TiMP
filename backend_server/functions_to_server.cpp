// что-то для сервера
#include "func_for_server.h"
#include "DataBase.h"

QString parsing(QString str, int descriptor) {
    QStringList ans = str.split("||");
    qDebug() << str;
    QString func = ans;
    ans.pop_front();
    if (func == "task1") {
        func = task1(ans, descriptor);
        return func;
    }
    else if (func == "task2") {
        func = task2(ans, descriptor);
        return func;
    }
    else if (func == "task3") {
        func = task3(ans, descriptor);
        return func;
    }
    else if (func == "task4") {
        func = task4(ans, descriptor);
        return func;
    }
    else if (func == "athorization") {
        func = author(ans, descriptor);
        return func;
    }
    else if (func == "registration") {
        func = reg(ans, descriptor);
        return func;
    }
    else if (func == "statistics") {
        func = stat(ans, descriptor);
        return func;
    }
    else {
        func = "ERROR!";
        return (func + '\r' + '\n');
    }
}
QString task1(QStringList params, int descriptor) {
    QString right = params[0];
    QString ans = DataBase::getInstance()->sendQuery("SELECT 1");
    return ans;
}
QString task2(QStringList params, int descriptor) {
    QString right = params[0];
    QString ans = DataBase::getInstance()->sendQuery("SELECT 2");
    return ans;
}
QString task3(QStringList params, int descriptor) {
    QString right = params[0];
    QString ans = DataBase::getInstance()->sendQuery("SELECT 3");
    return ans;
}
QString task4(QStringList params, int descriptor) {
    QString right = params[0];
    QString ans = DataBase::getInstance()->sendQuery("SELECT 4");
    return ans;
}
QString author(QStringList params, int descriptor) {
    QString login = params[0];
    QString password = params[1];
    if (DataBase::getInstance()->sendQuery("SELECT id FR...")) {
        return "auth_OK\r\n";
    }
    else {
        return "auth_False\r\n";
    }
}

QString reg(QStringList params, int descriptor) {
    QString login = params[0];
    QString email = params[1];
    QString password1 = params[2];
    QString password2 = params[3];
    if (password1 == password2) {
        if (DataBase::getInstance()->sendQuery("INSERT INT...")) {
            author(params, descriptor);
            return "reg_OK!\r\n";
        }
        else {
            return "reg_False!\r\n";
        }
    }
    else {
        return "Password wrong!\r\n";
    }
}

