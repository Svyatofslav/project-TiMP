#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <QPointer>

class ClientApi;
class AuthWidget;
class RegisterWidget;
class MainMenuWidget;
class TaskWidget;
class StatsWidget;

class WindowManager : public QObject
{
    Q_OBJECT
public:
    explicit WindowManager(QObject *parent = nullptr);

    void showAuth();

private:
    void connectSignals();

    ClientApi *m_api;

    QPointer<AuthWidget> m_authWidget;
    QPointer<RegisterWidget> m_registerWidget;
    QPointer<MainMenuWidget> m_mainMenu;
    QPointer<TaskWidget> m_taskWidget;
    QPointer<StatsWidget> m_statsWidget;
};

#endif // WINDOWMANAGER_H
