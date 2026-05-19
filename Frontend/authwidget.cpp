#include "authwidget.h"
#include "ui_authwidget.h"
#include <QMessageBox>

AuthWidget::AuthWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::authwidget)
{
    ui->setupUi(this);
    setWindowTitle("Авторизация");

    connect(ui->lineLogin, &QLineEdit::returnPressed,
            this, [this]() {
                ui->linePassword->setFocus(Qt::TabFocusReason);
            });

    // Enter в пароле → нажать кнопку Войти
    connect(ui->linePassword, &QLineEdit::returnPressed,
            ui->btnLogin, &QPushButton::click);
}

AuthWidget::~AuthWidget()
{
    delete ui;
}

void AuthWidget::on_btnLogin_clicked()
{
    QString login = ui->lineLogin->text().trimmed();
    QString password = ui->linePassword->text();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Авторизация", "Заполните все поля");
        return;
    }

    emit loginRequested(login, password);
}

void AuthWidget::on_btnToRegister_clicked()
{
    emit switchToRegister();
}

void AuthWidget::clearFields()
{
    ui->lineLogin->clear();
    ui->linePassword->clear();
}
