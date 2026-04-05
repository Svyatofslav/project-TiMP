#include "registerwidget.h"
#include "ui_registerwidget.h"

RegisterWidget::RegisterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registerwidget)
{
    ui->setupUi(this);
    setWindowTitle("Регистрация");

    connect(ui->lineLogin, &QLineEdit::returnPressed,
            ui->lineEmail, QOverload<>::of(&QWidget::setFocus));

    connect(ui->lineEmail, &QLineEdit::returnPressed,
            ui->linePassword1, QOverload<>::of(&QWidget::setFocus));

    connect(ui->linePassword1, &QLineEdit::returnPressed,
            ui->linePassword2, QOverload<>::of(&QWidget::setFocus));

    // Enter в последнем поле → нажать кнопку "Зарегистрироваться"
    connect(ui->linePassword2, &QLineEdit::returnPressed,
            ui->btnRegister, &QPushButton::click);
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
