#include "server.h"
#include "clienthandler.h"
#include <QDebug>

Server::Server(QObject *parent) : QTcpServer(parent) {}

void Server::startServer(quint16 port) {
    if (listen(QHostAddress::Any, port)) {
        qDebug() << "Сервер запущен на порту" << port;
    } else {
        qDebug() << "Ошибка запуска:" << errorString();
    }
}

void Server::incomingConnection(qintptr socketDescriptor) {
    qDebug() << "Новый клиент, дескриптор:" << socketDescriptor;
    ClientHandler *handler = new ClientHandler(socketDescriptor);
    connect(handler, &ClientHandler::finished,
            handler, &ClientHandler::deleteLater);
    handler->start(); // Запускаем в отдельном потоке
}
