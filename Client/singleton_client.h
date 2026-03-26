#ifndef SINGLETON_CLIENT_H
#define SINGLETON_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>

class SingletonClient;

class SingletonClientDestroyer
{
private:
    SingletonClient* p_instance = nullptr;
public:
    ~SingletonClientDestroyer();          // реализация в .cpp — там SingletonClient полностью известен
    void initialize(SingletonClient* p)
    {
        p_instance = p;
    }
};

class SingletonClient : public QObject
{
    Q_OBJECT

private:
    static SingletonClient*        p_instance;
    static SingletonClientDestroyer destroyer;

    QTcpSocket* mTcpSocket;

protected:
    explicit SingletonClient(QObject* parent = nullptr);
    SingletonClient(const SingletonClient&)            = delete;
    SingletonClient& operator=(const SingletonClient&) = delete;
    ~SingletonClient();                   // объявление — реализация в .cpp

    friend class SingletonClientDestroyer;

public:
    static SingletonClient* getInstance();

    void connectToServer(const QString& host, quint16 port);
    void disconnectFromServer();
    void send_msg_to_server(const QString& query);

signals:
    void message_from_server(QString msg);
    void connected();
    void disconnected();
    void errorOccurred(QString errorMsg);

private slots:
    void slotDisconnected();
};

#endif // SINGLETON_CLIENT_H
