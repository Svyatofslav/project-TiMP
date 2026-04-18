#include "taskwidget.h"
#include "ui_taskwidget.h"

TaskWidget::TaskWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::taskwidget)
{
    ui->setupUi(this);
    setWindowTitle("Задание");

    // Enter в поле ввода → нажать кнопку Отправить ответ
    connect(ui->lineAnswer, &QLineEdit::returnPressed,
            ui->btnSend, &QPushButton::click);
}

TaskWidget::~TaskWidget()
{
    delete ui;
}

void TaskWidget::setTask(const QString &funcName, double a, double b, int n, Method method)
{
    QString methodText;
    if (method == Method::MiddleRectangles)
        methodText = "методом средних прямоугольников";
    else
        methodText = "методом левых прямоугольников";

    QString text = QString(
                       "Вычислите приближённое значение определённого интеграла\n"
                       "%1 на отрезке [%2, %3]\n"
                       "%4 при n = %5 разбиениях.")
                       .arg(funcName)
                       .arg(a, 0, 'f', 2)
                       .arg(b, 0, 'f', 2)
                       .arg(methodText)
                       .arg(n);
    ui->labelTaskText->setText(text);
    ui->lineAnswer->clear();
}

void TaskWidget::on_btnSend_clicked()
{
    emit checkRequested(ui->lineAnswer->text());
}

void TaskWidget::on_btnBack_clicked()
{
    emit backToMenuRequested();
}
