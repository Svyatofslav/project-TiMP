#include "windowmanager.h"
#include "clientapi.h"
#include "authwidget.h"
#include "registerwidget.h"
#include "mainmenuwidget.h"
#include "taskwidget.h"
#include "statswidget.h"

#include <QMessageBox>

WindowManager::WindowManager(QObject *parent)
    : QObject(parent),
    m_api(new ClientApi(this))
{
    // подключение к серверу при старте
    m_api->connectToServer("127.0.0.1", 44444);
    connectSignals();
}

void WindowManager::showAuth()
{
    if (!m_authWidget) {
        m_authWidget = new AuthWidget;
        m_authWidget->setAttribute(Qt::WA_DeleteOnClose, false);
    }
    m_authWidget->show();
    m_authWidget->raise();
    m_authWidget->activateWindow();

    if (m_registerWidget)
        m_registerWidget->hide();
    if (m_mainMenu)
        m_mainMenu->hide();
    if (m_taskWidget)
        m_taskWidget->hide();
    if (m_statsWidget)
        m_statsWidget->hide();
}

void WindowManager::connectSignals()
{
    // AuthWidget создаётся при первом showAuth
    if (!m_authWidget) {
        m_authWidget = new AuthWidget;
    }

    // --- сигналы от AuthWidget ---
    connect(m_authWidget, &AuthWidget::loginRequested,
            m_api, &ClientApi::sendAuth);

    connect(m_authWidget, &AuthWidget::switchToRegister,
            [this]() {
                if (!m_registerWidget) {
                    m_registerWidget = new RegisterWidget;
                    // сигналы регистрации
                    connect(m_registerWidget, &RegisterWidget::registerRequested,
                            m_api, &ClientApi::sendRegister);
                    connect(m_registerWidget, &RegisterWidget::switchToAuth,
                            [this]() {
                                if (m_registerWidget)
                                    m_registerWidget->hide();
                                showAuth();
                            });
                }
                m_authWidget->hide();
                m_registerWidget->show();
                m_registerWidget->raise();
                m_registerWidget->activateWindow();
            });

    // --- результаты auth / reg ---
    connect(m_api, &ClientApi::authResult,
            [this](bool ok, const QString &message) {
                if (ok) {
                    // успех → открываем меню
                    if (!m_mainMenu) {
                        m_mainMenu = new MainMenuWidget;

                        // кнопки меню
                        connect(m_mainMenu, &MainMenuWidget::task1Clicked,
                                m_api, &ClientApi::sendTask1);
                        connect(m_mainMenu, &MainMenuWidget::task2Clicked,
                                m_api, &ClientApi::sendTask2);
                        connect(m_mainMenu, &MainMenuWidget::task3Clicked,
                                m_api, &ClientApi::sendTask3);
                        connect(m_mainMenu, &MainMenuWidget::task4Clicked,
                                m_api, &ClientApi::sendTask4);
                        connect(m_mainMenu, &MainMenuWidget::statsClicked,
                                m_api, &ClientApi::sendStat);
                        connect(m_mainMenu, &MainMenuWidget::logoutClicked,
                                m_api, &ClientApi::sendLogout);
                    }
                    if (m_authWidget) m_authWidget->hide();
                    if (m_registerWidget) m_registerWidget->hide();
                    m_mainMenu->show();
                    m_mainMenu->raise();
                    m_mainMenu->activateWindow();
                } else {
                    QMessageBox::warning(nullptr, "Авторизация", message.isEmpty()
                                                                                        ? "Ошибка авторизации" : message);
                }
            });

    connect(m_api, &ClientApi::regResult,
            [this](bool ok, const QString &message) {
                if (ok) {
                    QMessageBox::information(nullptr, "Регистрация",
                                             "Регистрация успешна, вы авторизованы.");
                    if (!m_mainMenu) {
                        m_mainMenu = new MainMenuWidget;
                        connect(m_mainMenu, &MainMenuWidget::task1Clicked,
                                m_api, &ClientApi::sendTask1);
                        connect(m_mainMenu, &MainMenuWidget::task2Clicked,
                                m_api, &ClientApi::sendTask2);
                        connect(m_mainMenu, &MainMenuWidget::task3Clicked,
                                m_api, &ClientApi::sendTask3);
                        connect(m_mainMenu, &MainMenuWidget::task4Clicked,
                                m_api, &ClientApi::sendTask4);
                        connect(m_mainMenu, &MainMenuWidget::statsClicked,
                                m_api, &ClientApi::sendStat);
                        connect(m_mainMenu, &MainMenuWidget::logoutClicked,
                                m_api, &ClientApi::sendLogout);
                    }
                    if (m_authWidget) m_authWidget->hide();
                    if (m_registerWidget) m_registerWidget->hide();
                    m_mainMenu->show();
                    m_mainMenu->raise();
                    m_mainMenu->activateWindow();
                } else {
                    QMessageBox::warning(nullptr, "Регистрация", message.isEmpty()
                                                                                        ? "Ошибка регистрации" : message);
                }
            });

    // --- task1 ---
    connect(m_api, &ClientApi::task1Received,
            [this](const QString &funcName, double a, double b, int n) {
                if (!m_taskWidget) {
                    m_taskWidget = new TaskWidget;
                    connect(m_taskWidget, &TaskWidget::checkRequested,
                            m_api, &ClientApi::sendCheckTask);
                    connect(m_taskWidget, &TaskWidget::backToMenuRequested,
                            [this]() {
                                if (m_taskWidget) m_taskWidget->hide();
                                if (m_mainMenu) {
                                    m_mainMenu->show();
                                    m_mainMenu->raise();
                                    m_mainMenu->activateWindow();
                                }
                            });
                }
                if (m_mainMenu) m_mainMenu->hide();
                m_taskWidget->setTask(funcName, a, b, n);
                m_taskWidget->show();
                m_taskWidget->raise();
                m_taskWidget->activateWindow();
            });

    // --- task2-4 в разработке ---
    connect(m_api, &ClientApi::taskInfo,
            [this](const QString &message) {
                QMessageBox::information(nullptr, "Задание", message);
            });

    // --- результат проверки ---
    connect(m_api, &ClientApi::checkResult,
            [this](bool ok, double userAns, double correctAns, const QString &msg) {
                QString text;
                if (ok) {
                    text = QString("%1\nПравильный ответ: %2")
                               .arg(msg)
                               .arg(correctAns, 0, 'f', 4);
                } else {
                    text = QString("%1\nВаш ответ: %2\nПравильный ответ: %3")
                               .arg(msg)
                               .arg(userAns, 0, 'f', 4)
                               .arg(correctAns, 0, 'f', 4);
                }
                QMessageBox::information(nullptr, "Проверка", text);
            });

    // --- статистика ---
    connect(m_api, &ClientApi::statResult,
            [this](const QString &login,
                   int t1, int t2, int t3, int t4) {
                if (!m_statsWidget) {
                    m_statsWidget = new StatsWidget;
                    connect(m_statsWidget, &StatsWidget::backToMenuRequested,
                            [this]() {
                                if (m_statsWidget) m_statsWidget->hide();
                                if (m_mainMenu) {
                                    m_mainMenu->show();
                                    m_mainMenu->raise();
                                    m_mainMenu->activateWindow();
                                }
                            });
                }
                if (m_mainMenu) m_mainMenu->hide();
                m_statsWidget->setStats(login, t1, t2, t3, t4);
                m_statsWidget->show();
                m_statsWidget->raise();
                m_statsWidget->activateWindow();
            });

    // --- выход из аккаунта ---
    connect(m_api, &ClientApi::logoutResult,
            [this](bool ok, const QString &message) {
                if (ok) {
                    // Успешный выход — возвращаем к авторизации
                    if (m_mainMenu)
                        m_mainMenu->hide();
                    if (m_taskWidget)
                        m_taskWidget->hide();
                    if (m_statsWidget)
                        m_statsWidget->hide();
                    showAuth();
                } else {
                    QMessageBox::warning(nullptr, "Выход", message.isEmpty()
                                                                             ? "Ошибка выхода" : message);
                }
            });

    // --- общие ошибки / disconnect ---
    connect(m_api, &ClientApi::errorOccurred,
            [this](const QString &msg) {
                QMessageBox::warning(nullptr, "Ошибка", msg);
            });

    connect(m_api, &ClientApi::disconnected,
            [this]() {
                QMessageBox::warning(nullptr, "Соединение",
                                     "Соединение с сервером потеряно.\nВы будете возвращены к авторизации.");
                showAuth();
            });
}
