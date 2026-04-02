#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QString>

enum class ResponseType {
    AuthOk,
    AuthError,
    RegOk,
    RegError,
    Task1Ok,
    TaskInfo,
    CheckOk,
    CheckFalse,
    CheckError,
    StatOk,
    LogoutOk,
    LogoutError,
    Unknown
};

struct ParsedResponse
{
    ResponseType type = ResponseType::Unknown;

    // общие поля
    QString message;

    // task1
    QString funcName;
    double a = 0.0;
    double b = 0.0;
    int n = 0;

    // check
    double userAnswer = 0.0;
    double correctAnswer = 0.0;

    // stat
    QString login;
    int t1 = 0;
    int t2 = 0;
    int t3 = 0;
    int t4 = 0;
};

class Functions
{
public:
    static QString hashPassword(const QString &password);

    static QString buildAuthRequest(const QString &login,
                                    const QString &password);
    static QString buildRegisterRequest(const QString &login,
                                        const QString &email,
                                        const QString &password1,
                                        const QString &password2);
    static QString buildTaskRequest(int taskNum);
    static QString buildCheckTaskRequest(const QString &answer);
    static QString buildStatRequest();
    static QString buildLogoutRequest();

    static ParsedResponse parseResponse(const QString &msg);
};

#endif // FUNCTIONS_H
