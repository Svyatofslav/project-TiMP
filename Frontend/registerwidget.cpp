#include "registerwidget.h"
#include "ui_registerwidget.h"

RegisterWidget::RegisterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registerwidget)
{
    ui->setupUi(this);
    setWindowTitle("Регистрация");

    connect(ui->lineLogin, &QLineEdit::returnPressed,
            this, [this]() { ui->lineEmail->setFocus(Qt::TabFocusReason); });

    connect(ui->lineEmail, &QLineEdit::returnPressed,
            this, [this]() { ui->linePassword1->setFocus(Qt::TabFocusReason); });

    connect(ui->linePassword1, &QLineEdit::returnPressed,
            this, [this]() { ui->linePassword2->setFocus(Qt::TabFocusReason); });

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
