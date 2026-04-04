#include "authwidget.h"
#include "ui_authwidget.h"

AuthWidget::AuthWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::authwidget)
{
    ui->setupUi(this);
    setWindowTitle("Авторизация");
}

AuthWidget::~AuthWidget()
{
    delete ui;
}

void AuthWidget::on_btnLogin_clicked()
{
    emit loginRequested(ui->lineLogin->text(),
                        ui->linePassword->text());
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
