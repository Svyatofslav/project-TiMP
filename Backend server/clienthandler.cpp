#include "clienthandler.h"
#include "functions_to_server.h"
#include <QDebug>

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent)
    : QThread(parent), m_socketDescriptor(socketDescriptor) {}

void ClientHandler::run() {
    QTcpSocket socket;

    if (!socket.setSocketDescriptor(m_socketDescriptor)) {
        qDebug() << "Ошибка дескриптора:" << socket.errorString();
        return;
    }

    qDebug() << "Клиент подключён:" << socket.peerAddress().toString();

    while (socket.state() == QAbstractSocket::ConnectedState) {
        if (socket.waitForReadyRead(3000)) {
            QString request = QString::fromUtf8(socket.readAll());
            qDebug() << "Запрос:" << request;

            // Передаём запрос в твою функцию parsing() из functions_to_server.cpp
            QString response = parsing(request, (int)m_socketDescriptor);

            socket.write(response.toUtf8());
            socket.flush();
        }
    }

    qDebug() << "Клиент отключился";
    socket.disconnectFromHost();
}
