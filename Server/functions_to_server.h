#ifndef FUNCTIONS_TO_SERVER_H
#define FUNCTIONS_TO_SERVER_H

#include <QString>
#include <QStringList>
#include <QDebug>

QString parsing(QString str, int descriptor);
QString task1(QStringList params, int descriptor);
QString task2(QStringList params, int descriptor);
QString task3(QStringList params, int descriptor);
QString task4(QStringList params, int descriptor);
QString author(QStringList params, int descriptor);
QString reg(QStringList params, int descriptor);
QString stat(QStringList params, int descriptor);
QString logout(QStringList params, int descriptor);

// Заготовки под реализацию задач
QString handleRegister(const QString& payload);
QString handleAuth(const QString& payload);
QString handleStats(const QString& payload);
QString handleSHA512(const QString& payload);
QString handleDivision(const QString& payload);
QString handleShortest(const QString& payload);

#endif // FUNCTIONS_TO_SERVER_H
