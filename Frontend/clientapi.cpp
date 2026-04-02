#include "clientapi.h"
#include "functions.h"
#include <QDebug>

ClientApi::ClientApi(QObject *parent)
    : QObject(parent),
    m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::readyRead,
            this, &ClientApi::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected,
            this, &ClientApi::onDisconnected);
}

void ClientApi::connectToServer(const QString &host, quint16 port)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        return;

    m_socket->connectToHost(host, port);
    if (!m_socket->waitForConnected(3000)) {
        emit errorOccurred(tr("Не удалось подключиться к серверу: %1")
                               .arg(m_socket->errorString()));
    } else {
        qDebug() << "[ClientApi] Connected to server";
    }
}

bool ClientApi::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void ClientApi::sendRaw(const QString &msg)
{
    if (!isConnected()) {
        emit errorOccurred(tr("Нет соединения с сервером"));
        return;
    }
    QByteArray data = msg.toUtf8();
    m_socket->write(data);
    if (!m_socket->waitForBytesWritten(3000)) {
        emit errorOccurred(tr("Таймаут отправки: %1")
                               .arg(m_socket->errorString()));
    }
}

// -------- высокоуровневые запросы --------

void ClientApi::sendAuth(const QString &login, const QString &password)
{
    //QString hashed = Functions::hashPassword(password); // SHA-512
    QString msg = Functions::buildAuthRequest(login, password);
    sendRaw(msg);
}

void ClientApi::sendRegister(const QString &login, const QString &email,
                             const QString &password1, const QString &password2)
{
    QString msg = Functions::buildRegisterRequest(login, email,
                                                  password1, password2);
    sendRaw(msg);
}

void ClientApi::sendTask1()
{
    QString msg = Functions::buildTaskRequest(1);
    sendRaw(msg);
}

void ClientApi::sendTask2()
{
    QString msg = Functions::buildTaskRequest(2);
    sendRaw(msg);
}

void ClientApi::sendTask3()
{
    QString msg = Functions::buildTaskRequest(3);
    sendRaw(msg);
}

void ClientApi::sendTask4()
{
    QString msg = Functions::buildTaskRequest(4);
    sendRaw(msg);
}

void ClientApi::sendCheckTask(const QString &answer)
{
    QString msg = Functions::buildCheckTaskRequest(answer);
    sendRaw(msg);
}

void ClientApi::sendStat()
{
    QString msg = Functions::buildStatRequest();
    sendRaw(msg);
}

void ClientApi::sendLogout()
{
    QString msg = Functions::buildLogoutRequest();
    sendRaw(msg);
}

// -------- приём ответа --------

void ClientApi::onReadyRead()
{
    m_buffer += QString::fromUtf8(m_socket->readAll());

    // простой вариант: считаем, что сервер всегда шлёт один ответ целиком
    // и не использует разделители; у тебя протокол короткий, этого хватит
    QString msg = m_buffer.trimmed();
    if (!msg.isEmpty()) {
        processResponse(msg);
        m_buffer.clear();
    }
}

void ClientApi::onDisconnected()
{
    emit disconnected();
}

void ClientApi::processResponse(const QString &msg)
{
    ParsedResponse r = Functions::parseResponse(msg);

    if (r.type == ResponseType::AuthOk) {
        emit authResult(true, QString());
    } else if (r.type == ResponseType::AuthError) {
        emit authResult(false, r.message);
    } else if (r.type == ResponseType::RegOk) {
        emit regResult(true, QString());
    } else if (r.type == ResponseType::RegError) {
        emit regResult(false, r.message);
    } else if (r.type == ResponseType::Task1Ok) {
        emit task1Received(r.funcName, r.a, r.b, r.n);
    } else if (r.type == ResponseType::TaskInfo) {
        emit taskInfo(r.message);
    } else if (r.type == ResponseType::CheckOk) {
        emit checkResult(true, r.userAnswer, r.correctAnswer, r.message);
    } else if (r.type == ResponseType::CheckFalse) {
        emit checkResult(false, r.userAnswer, r.correctAnswer, r.message);
    } else if (r.type == ResponseType::CheckError) {
        emit errorOccurred(r.message);
    } else if (r.type == ResponseType::StatOk) {
        emit statResult(r.login, r.t1, r.t2, r.t3, r.t4);
    } else if (r.type == ResponseType::LogoutOk) {
        emit logoutResult(true, QString());
    } else if (r.type == ResponseType::LogoutError) {
        emit logoutResult(false, r.message);
    } else if (r.type == ResponseType::Unknown) {
        emit errorOccurred(tr("Неизвестный ответ сервера: %1").arg(msg));
    }
}
