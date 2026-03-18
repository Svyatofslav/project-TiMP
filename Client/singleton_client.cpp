#include "singleton_client.h"

// Определения статических членов
SingletonClient*         SingletonClient::p_instance = nullptr;
SingletonClientDestroyer SingletonClient::destroyer;

// Деструктор Destroyer — здесь SingletonClient уже полностью объявлен
SingletonClientDestroyer::~SingletonClientDestroyer()
{
    delete p_instance;
}

// Деструктор клиента — корректно закрываем сокет
SingletonClient::~SingletonClient()
{
    if (mTcpSocket)
    {
        if (mTcpSocket->state() != QAbstractSocket::UnconnectedState)
        {
            mTcpSocket->disconnectFromHost();
            // Даём время на graceful shutdown (waitForDisconnected)
            if (mTcpSocket->state() != QAbstractSocket::UnconnectedState)
                mTcpSocket->waitForDisconnected(3000);
        }
        // Не вызываем delete — mTcpSocket имеет parent=this, Qt удалит сам
    }
}

SingletonClient::SingletonClient(QObject* parent) : QObject(parent)
{
    mTcpSocket = new QTcpSocket(this);   // parent=this → автоудаление Qt

    connect(mTcpSocket, &QTcpSocket::readyRead,
            this,        &SingletonClient::slotServerRead);

    connect(mTcpSocket, &QTcpSocket::connected,
            this,        &SingletonClient::slotConnected);

    connect(mTcpSocket, &QTcpSocket::disconnected,
            this,        &SingletonClient::slotDisconnected);

    connect(mTcpSocket, &QAbstractSocket::errorOccurred,
            this,        &SingletonClient::slotError);
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

void SingletonClient::connectToServer(const QString& host, quint16 port)
{
    if (mTcpSocket->state() == QAbstractSocket::UnconnectedState)
        mTcpSocket->connectToHost(host, port);
}

void SingletonClient::disconnectFromServer()
{
    if (mTcpSocket->state() != QAbstractSocket::UnconnectedState)
        mTcpSocket->disconnectFromHost();
}

void SingletonClient::send_msg_to_server(const QString& query)
{
    if (mTcpSocket->state() == QAbstractSocket::ConnectedState)
        mTcpSocket->write(query.toUtf8());
    else
        qWarning() << "[SingletonClient] Не подключён к серверу!";
}

void SingletonClient::slotServerRead()
{
    QByteArray data;
    while (mTcpSocket->bytesAvailable() > 0)
        data.append(mTcpSocket->readAll());

    QString msg = QString::fromUtf8(data);
    qDebug() << "[SingletonClient] Получено:" << msg;
    emit message_from_server(msg);
}

void SingletonClient::slotConnected()
{
    qDebug() << "[SingletonClient] Подключён к серверу";
    emit connected();
}

void SingletonClient::slotDisconnected()
{
    qDebug() << "[SingletonClient] Отключён от сервера";
    emit disconnected();
}

void SingletonClient::slotError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    qWarning() << "[SingletonClient] Ошибка сокета:" << mTcpSocket->errorString();
    emit errorOccurred(mTcpSocket->errorString());
}
