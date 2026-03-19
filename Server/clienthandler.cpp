// В файле Server/clienthandler.cpp

#include "clienthandler.h"
#include "functions_to_server.h"
#include "DataBase.h"
#include <QDebug>

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent)
    : QThread(parent), m_socketDescriptor(socketDescriptor)
{
}

void ClientHandler::run()
{
    QTcpSocket socket;

    if (!socket.setSocketDescriptor(m_socketDescriptor))
    {
        qDebug() << "Ошибка дескриптора:" << socket.errorString();
        return;
    }

    qDebug() << "Клиент подключён:" << socket.peerAddress().toString();

    // Основной цикл чтения запросов
    while (socket.state() == QAbstractSocket::ConnectedState)
    {
        if (socket.waitForReadyRead(3000))
        {
            QByteArray data;
            while (socket.bytesAvailable() > 0)
                data.append(socket.readAll());

            QString request = QString::fromUtf8(data);
            qDebug() << "Запрос от" << socket.peerAddress().toString() << ":" << request;

            QString response = parsing(request, (int)m_socketDescriptor);
            socket.write(response.toUtf8());
            socket.flush();
        }
        else
        {
            if (socket.state() != QAbstractSocket::ConnectedState)
            {
                qDebug() << "Клиент отключился:" << socket.peerAddress().toString();
                break;
            }
        }
    }

    // Закрываем сокет, если он ещё не отключён
    if (socket.state() != QAbstractSocket::UnconnectedState)
    {
        socket.disconnectFromHost();
        socket.waitForDisconnected(3000);
    }

    // Auto-logout через БД
    QString login = DataBase::getInstance()->getLoginBySocket(
        QString::number(m_socketDescriptor));

    if (!login.isEmpty())
    {
        DataBase::getInstance()->logoutUser(login);
        qDebug() << "[ClientHandler] Авто-logout для:" << login;
    }

    // ЛОГирование закрытия БД‑соединения
    QString connName = QString("connection_%1")
                           .arg((quintptr)QThread::currentThreadId());
    {
        QSqlDatabase db = QSqlDatabase::database(connName);
        if (db.isOpen())
        {
            db.close();
            qDebug() << "[ClientHandler] БД‑соединение для потока" << connName << "закрыто";
        }
    }
    QSqlDatabase::removeDatabase(connName);
    qDebug() << "[ClientHandler] БД‑соединение" << connName << "удалено из пула";

    qDebug() << "Поток клиента завершён";
}
