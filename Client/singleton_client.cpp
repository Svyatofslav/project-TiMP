#include "singleton_client.h"

// Определения статических переменных — только здесь, не в .h!
SingletonClient* SingletonClient::p_instance = nullptr;
SingletonDestroyer SingletonClient::destroyer;

// Деструктор здесь — SingletonClient уже полностью известен
SingletonDestroyer::~SingletonDestroyer()
{
    delete p_instance;
}

SingletonClient::SingletonClient(QObject *parent) : QObject(parent)
{
    mTcpSocket = new QTcpSocket(this);
    mTcpSocket->connectToHost("127.0.0.1", 33333);
    connect(mTcpSocket, &QTcpSocket::readyRead,
            this, &SingletonClient::slotServerRead);
}

SingletonClient* SingletonClient::getInstance()
{
    if (!p_instance)
    {
        p_instance = new SingletonClient();
        destroyer.initialize(p_instance);
    }
    return p_instance;
}

void SingletonClient::send_msg_to_server(QString query)
{
    mTcpSocket->write(query.toUtf8());
}

void SingletonClient::slotServerRead()
{
    QString msg = "";
    while(mTcpSocket->bytesAvailable() > 0)
    {
        QByteArray array = mTcpSocket->readAll();
        msg.append(array);
    }
    qDebug() << msg;
    emit message_from_server(msg);
}
