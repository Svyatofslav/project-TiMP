#ifndef FUNCTIONS_TO_SERVER_H
#define FUNCTIONS_TO_SERVER_H

#include <QString>

// Регистрация: payload = "login,password"
QString handleRegister(const QString &payload);

// Авторизация: payload = "login,password"
QString handleAuth(const QString &payload);

// Статистика: игнорируется payload
QString handleStats(const QString &payload);

// SHA-512: payload = любой текст для хэширования
QString handleSHA512(const QString &payload);

// Деление пополам (вычисление sqrt): payload = строковое число
QString handleDivision(const QString &payload);

// Поиск кратчайшего пути в графе: payload = "s,t"
QString handleShortest(const QString &payload);

#endif // FUNCTIONS_TO_SERVER_H
