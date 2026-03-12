#ifndef SINGLETON_CLIENT_H
#define SINGLETON_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>

class SingletonClient;

class SingletonDestroyer
{
private:
    SingletonClient * p_instance = nullptr;
public:
    ~SingletonDestroyer();                        // ← только объявление
    void initialize(SingletonClient * p)
    {
        p_instance = p;
    }
};

class SingletonClient : public QObject
{
    Q_OBJECT
private:
    static SingletonClient * p_instance;
    static SingletonDestroyer destroyer;
    QTcpSocket * mTcpSocket;
protected:
    SingletonClient(QObject *parent = nullptr);
    SingletonClient(const SingletonClient&) = delete;
    SingletonClient& operator = (SingletonClient &) = delete;
    ~SingletonClient() {}
    friend class SingletonDestroyer;
public:
    static SingletonClient* getInstance();
    void send_msg_to_server(QString query);

signals:
    void message_from_server(QString msg);
private slots:
    void slotServerRead();
};

#endif // SINGLETON_CLIENT_H
