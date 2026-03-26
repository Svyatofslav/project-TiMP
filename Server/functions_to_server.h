#ifndef FUNCTIONS_TO_SERVER_H
#define FUNCTIONS_TO_SERVER_H

#include <QString>
#include <QStringList>
#include <QDebug>

// Парсер команд
QString parsing(QString str, int descriptor);

// Задания
QString task1(QStringList params, int descriptor);
QString task2(QStringList params, int descriptor);
QString task3(QStringList params, int descriptor);
QString task4(QStringList params, int descriptor);
QString check_task(QStringList params, int descriptor);

// Авторизация / регистрация / статистика / выход
QString author(QStringList params, int descriptor);
QString reg(QStringList params, int descriptor);
QString stat(QStringList params, int descriptor);
QString logout(QStringList params, int descriptor);

// Хеширование
QString handleSHA512(const QString& payload);

// Вспомогательные для task1
QString get_random_task1();
double  solver_task1(const QString& params);
QString funcName_task1(int funcId);

#endif // FUNCTIONS_TO_SERVER_H
