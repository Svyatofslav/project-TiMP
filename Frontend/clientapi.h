#ifndef CLIENTAPI_H
#define CLIENTAPI_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QTimer>

class ClientApi : public QObject
{
    Q_OBJECT
public:
    explicit ClientApi(QObject *parent = nullptr);

    void connectToServer(const QString &host, quint16 port);
    bool isConnected() const;

    // высокоуровневые запросы
    void sendAuth(const QString &login, const QString &password);
    void sendRegister(const QString &login, const QString &email,
                      const QString &password1, const QString &password2);
    void sendTask1();
    void sendTask2();
    void sendTask3();
    void sendTask4();
    void sendCheckTask(const QString &answer);
    void sendStat();
    void sendLogout();

signals:
    // результаты
    void authResult(bool ok, const QString &message);
    void regResult(bool ok, const QString &message);
    void task1Received(const QString &funcName, double a, double b, int n);
    void taskInfo(const QString &message); // для task2-4 "в разработке"
    void checkResult(bool ok,
                     double userAnswer,
                     double correctAnswer,
                     const QString &message);
    void statResult(const QString &login,
                    int t1, int t2, int t3, int t4);
    void logoutResult(bool ok, const QString &message);

    // общие
    void errorOccurred(const QString &message);
    void disconnected();

private slots:
    void onReadyRead();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void onReconnectTimeout();

private:
    void sendRaw(const QString &msg);
    void processResponse(const QString &msg);
    void scheduleReconnect();

    QTcpSocket *m_socket;
    QString m_buffer;

    QTimer m_reconnectTimer;
    QString m_host;
    quint16 m_port = 0;
};

#endif // CLIENTAPI_H
