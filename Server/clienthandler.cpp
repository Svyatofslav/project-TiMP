#include "clienthandler.h"
#include "functions_to_server.h"
#include "DataBase.h"
#include <QDebug>

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent)
    : QThread(parent), m_socketDescriptor(socketDescriptor) {}

void ClientHandler::run()
{
    QTcpSocket socket;

    if (!socket.setSocketDescriptor(m_socketDescriptor)) {
        qDebug() << "Ошибка дескриптора:" << socket.errorString();
        return;
    }

    qDebug() << "Клиент подключён:" << socket.peerAddress().toString();

    while (socket.state() == QAbstractSocket::ConnectedState)
    {
        if (socket.waitForReadyRead(3000))
        {
            QByteArray data = socket.readAll();
            QString request = QString::fromUtf8(data).trimmed();

            if (!request.isEmpty()) {
                qDebug() << "Запрос от" << socket.peerAddress().toString() << ":" << request;
                QString response = parsing(request, (int)m_socketDescriptor);
                socket.write(response.toUtf8());
                socket.flush();
            }
        }
    }

    if (socket.state() != QAbstractSocket::UnconnectedState) {
        socket.disconnectFromHost();
        socket.waitForDisconnected(3000);
    }

    // Авто-logout при обрыве соединения
    QString login = DataBase::getInstance()->getLoginBySocket(
        QString::number(m_socketDescriptor));
    if (!login.isEmpty()) {
        DataBase::getInstance()->clearTaskState(login);
        DataBase::getInstance()->logoutUser(login);
        qDebug() << "[ClientHandler] Авто-logout для:" << login;
    }

    // Закрываем соединение с БД для этого потока
    DataBase::getInstance()->closeThreadConnection();

    qDebug() << "Поток клиента завершён";
}
