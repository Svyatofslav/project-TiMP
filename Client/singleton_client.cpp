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

    connect(mTcpSocket, &QTcpSocket::disconnected,
            this,        &SingletonClient::slotDisconnected);
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
    {
        mTcpSocket->connectToHost(host, port);
        if (mTcpSocket->waitForConnected(3000))
        {
            qDebug() << "[SingletonClient] Подключён к серверу";
            emit connected();
        }
        else
        {
            qWarning() << "[SingletonClient] Не удалось подключиться:" << mTcpSocket->errorString();
            emit errorOccurred(mTcpSocket->errorString());
        }
    }
}

void SingletonClient::disconnectFromServer()
{
    if (mTcpSocket->state() != QAbstractSocket::UnconnectedState)
        mTcpSocket->disconnectFromHost();
}

void SingletonClient::send_msg_to_server(const QString& query)
{
    if (mTcpSocket->state() != QAbstractSocket::ConnectedState)
    {
        qWarning() << "[SingletonClient] Не подключён к серверу!";
        emit errorOccurred("Нет соединения с сервером");
        return;
    }

    // Синхронная отправка
    mTcpSocket->write(query.toUtf8());
    if (!mTcpSocket->waitForBytesWritten(3000))
    {
        qWarning() << "[SingletonClient] Таймаут отправки:" << mTcpSocket->errorString();
        emit errorOccurred("Таймаут отправки");
        return;
    }

    // Синхронное ожидание ответа
    if (!mTcpSocket->waitForReadyRead(5000))
    {
        qWarning() << "[SingletonClient] Таймаут ожидания ответа:" << mTcpSocket->errorString();
        emit errorOccurred("Таймаут ответа сервера");
        return;
    }

    QByteArray data = mTcpSocket->readAll();
    QString msg = QString::fromUtf8(data);
    qDebug() << "[SingletonClient] Получено:" << msg;
    emit message_from_server(msg);
}

void SingletonClient::slotDisconnected()
{
    qDebug() << "[SingletonClient] Отключён от сервера";
    emit disconnected();
}
