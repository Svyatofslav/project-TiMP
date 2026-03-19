#include "clienthandler.h"
#include "functions_to_server.h"
#include "DataBase.h"
#include <QDebug>

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent)
    : QThread(parent), m_socketDescriptor(socketDescriptor) {}

void ClientHandler::run()
{
    QTcpSocket socket;

    if (!socket.setSocketDescriptor(m_socketDescriptor))
    {
        qDebug() << "Ошибка дескриптора:" << socket.errorString();
        return;
    }

    qDebug() << "Клиент подключён:" << socket.peerAddress().toString();

    while (socket.state() == QAbstractSocket::ConnectedState)
    {
        if (socket.waitForReadyRead(3000))
        {
            // Читаем все доступные данные (TCP может дробить пакеты)
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
            // waitForReadyRead вернул false — разбираемся почему
            if (socket.state() != QAbstractSocket::ConnectedState)
            {
                // Клиент реально отключился
                qDebug() << "Клиент отключился:" << socket.peerAddress().toString();
                break;
            }
            // Иначе — просто таймаут (3 сек тишины), клиент ещё подключён
            // Продолжаем ждать — не прерываем цикл
        }
    }

    // Корректное закрытие со стороны сервера
    if (socket.state() != QAbstractSocket::UnconnectedState)
    {
        socket.disconnectFromHost();
        socket.waitForDisconnected(3000);
    }

    // После выхода из while-цикла — принудительный logout
    QString login = DataBase::getInstance()->getLoginBySocket(
        QString::number(m_socketDescriptor));
    if (!login.isEmpty())
    {
        DataBase::getInstance()->logoutUser(login);
        qDebug() << "[ClientHandler] Авто-logout для:" << login;
    }


    qDebug() << "Поток клиента завершён";
}
