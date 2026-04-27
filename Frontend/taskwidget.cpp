#include "taskwidget.h"
#include "ui_taskwidget.h"
#include <QMessageBox>

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
    QString text;
    m_currentMethod = method;

    if (method == Method::Task3TrapezoidCheck) {
        text = QString(
                   "Дано: интеграл от функции, имеющей особенность на конце отрезка.\n"
                   "Функция: %1\n"
                   "Отрезок: [%2, %3]\n\n"
                   "Найти: применим ли метод трапеций?\n"
                   "Введите ответ: да или нет.")
                   .arg(funcName)
                   .arg(a, 0, 'f', 3)
                   .arg(b, 0, 'f', 3);
    } else {
        QString methodText;
        if (method == Method::MiddleRectangles)
            methodText = "методом средних прямоугольников";
        else
            methodText = "методом левых прямоугольников";

        text = QString(
                   "Вычислите приближённое значение определённого интеграла\n"
                   "%1 на отрезке [%2, %3]\n"
                   "%4 при n = %5 разбиениях.")
                   .arg(funcName)
                   .arg(a, 0, 'f', 2)
                   .arg(b, 0, 'f', 2)
                   .arg(methodText)
                   .arg(n);
    }

    ui->labelTaskText->setText(text);
    ui->lineAnswer->clear();
}

void TaskWidget::on_btnSend_clicked()
{
    QString answer = ui->lineAnswer->text().trimmed().toLower();

    if (m_currentMethod == Method::Task3TrapezoidCheck) {
        if (answer != "да" && answer != "нет") {
            QMessageBox::warning(this, "Неверный ввод",
                                 "Для задания 3 разрешены только ответы: да или нет.");
            return;
        }
    }

    emit checkRequested(answer);
}

void TaskWidget::on_btnBack_clicked()
{
    emit backToMenuRequested();
}
