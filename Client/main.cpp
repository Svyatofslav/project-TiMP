#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include "singleton_client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SingletonClient* client = SingletonClient::getInstance();

    // Состояние подключения
    bool is_connected = false;
    bool task1_sent   = false;

    // Таймер таймаута ожидания ответа на task1
    QTimer task1Timer;
    task1Timer.setSingleShot(true);
    task1Timer.setInterval(8000); // 8 секунд ожидания ответа

    // Таймер авто‑переподключения при ошибке
    QTimer reconnectTimer;
    reconnectTimer.setSingleShot(true);
    reconnectTimer.setInterval(3000); // через 3 сек после ошибки попробовать снова

    // Обработка подключения
    QObject::connect(client, &SingletonClient::connected, [&]() {
        qDebug() << "Клиент подключён к серверу";

        is_connected = true;
        task1_sent   = false;

        // Сбрасываем таймер таймаута task1, если он был запущен
        if (task1Timer.isActive())
            task1Timer.stop();

        // Отправляем task1 только после подключения
        qDebug() << "Отправляю task1...";
        client->send_msg_to_server("task1\r\n");
        task1_sent = true;

        // Запускаем таймер таймаута ответа
        task1Timer.start();
    });

    // Тайм‑аут ожидания ответа на task1
    QObject::connect(&task1Timer, &QTimer::timeout, [&]() {
        if (task1_sent && !is_connected)
        {
            qWarning() << "Тайм‑аут ожидания ответа на task1: сервер не отвечает / соединение оборвано";
        }
        else if (task1_sent && is_connected)
        {
            qWarning() << "Тайм‑аут ожидания ответа на task1: возможно, сервер завис или не отвечает";
            // При желании можно здесь:
            // client->disconnectFromServer(); // и дальше reconnectTimer запустить
        }
        task1_sent = false; // сбрасываем флаг
    });

    // Обработка отключения
    QObject::connect(client, &SingletonClient::disconnected, [&]() {
        qDebug() << "Клиент отключён от сервера, возможно, сервер упал или соединение оборвалось";
        is_connected = false;
        task1_sent   = false;

        // Останавливаем таймер таймаута task1, если он активен
        if (task1Timer.isActive())
            task1Timer.stop();

        // Запускаем таймер повторного подключения
        reconnectTimer.start();
    });

    // Обработка ошибок
    QObject::connect(client, &SingletonClient::errorOccurred, [&reconnectTimer, &is_connected](QString errorMsg) {
        qWarning() << "Ошибка клиента:" << errorMsg;

        is_connected = false;

        // Если ещё не активен, запускаем таймер повторного подключения
        if (!reconnectTimer.isActive())
            reconnectTimer.start();
    });

    // Авто‑переподключение к серверу
    QObject::connect(&reconnectTimer, &QTimer::timeout, [&client, &reconnectTimer]() {
        qDebug() << "Пытаюсь переподключиться к серверу...";
        client->connectToServer("127.0.0.1", 44444);

        // Если снова произойдёт ошибка/отключение, таймер запустится снова
    });

    // Обработка сообщений от сервера
    QObject::connect(client, &SingletonClient::message_from_server,
                     [&task1Timer, &task1_sent](QString msg) {
                         qDebug() << "Сообщение от сервера:" << msg;

                         // Если пришёл ответ на task1, останавливаем таймер таймаута
                         if (task1_sent &&
                             (msg.contains("=== ЗАДАНИЕ 1") || msg.contains("ERROR")))
                         {
                             task1Timer.stop();
                             task1_sent = false;
                         }
                     });

    return a.exec();
}
