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
    client->connectToServer("127.0.0.1", 33333);

    // Подключаем сигнал от сервера к отладочному выводу
    QObject::connect(client, &SingletonClient::message_from_server,
                     [](QString msg) {
                         qDebug() << "Сообщение от сервера:" << msg;
                     });

    // Пример: отправим сообщение через 1 секунду
    QTimer::singleShot(1000, &a, [&]() {
        client->send_msg_to_server("Hello from SingletonClient!");
    });

    // Пример: закрываем приложение через 10 секунд
    QTimer::singleShot(10000, &a, &QCoreApplication::quit);

    // Запускаем event loop — без него Qt-объекты и сигналы не работают
    return a.exec();
}
