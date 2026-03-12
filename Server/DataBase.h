#ifndef DATABASE_H
#define DATABASE_H

#include <QString>

class DataBase {
public:
    static DataBase* getInstance() {
        static DataBase instance;
        return &instance;
    }
    // Заглушка: непустая строка чтобы if-условия работали
    QString sendQuery(const QString &) { return "Заглушка для базы данных\r\n"; }
private:
    DataBase() {}
};

#endif // DATABASE_H
