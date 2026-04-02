#include "registerwidget.h"
#include "ui_registerwidget.h"

RegisterWidget::RegisterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registerwidget)
{
    ui->setupUi(this);
    setWindowTitle("Регистрация");
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}

void RegisterWidget::on_btnRegister_clicked()
{
    emit registerRequested(ui->lineLogin->text(),
                           ui->lineEmail->text(),
                           ui->linePassword1->text(),
                           ui->linePassword2->text());
}

void RegisterWidget::on_btnToAuth_clicked()
{
    emit switchToAuth();
}
