#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QThread>
#include <QTcpSocket>

class ClientHandler : public QThread
{
    Q_OBJECT
public:
    explicit ClientHandler(qintptr socketDescriptor,
                           QObject *parent = nullptr);
protected:
    void run() override;

private:
    qintptr m_socketDescriptor;
};

#endif // CLIENTHANDLER_H
