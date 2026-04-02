#include <QCoreApplication>
#include <QDebug>

// Включаем наш клиент‑синглтон
#include "singleton_client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Получаем единственный экземпляр клиента
    SingletonClient* client = SingletonClient::getInstance();

    // Подключаемся к серверу
    client->connectToServer("127.0.0.1", 44444);

    // Подключаем сигнал от сервера к отладочному выводу
    QObject::connect(client, &SingletonClient::message_from_server,
                     [](QString msg) {
                         qDebug() << "Сообщение от сервера:" << msg;
                     });

    // Запускаем event loop — без него Qt-объекты и сигналы не работают
    return a.exec();
}
