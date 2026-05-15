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

void TaskWidget::setTask4(const QString &funcName,
                          double a,
                          double b,
                          int n,
                          double trapResult,
                          double simpsonResult,
                          const QString &option1,
                          const QString &option2,
                          const QString &option3,
                          const QString &option4)
{
    m_currentMethod = Method::Task4SimpsonChoice;

    QString text = QString(
                       "Дано: функция и высокая точность вычисления.\n"
                       "Функция: %1\n"
                       "Отрезок: [%2, %3]\n"
                       "Число разбиений: n = %4\n"
                       "Метод трапеций дал результат: %5\n"
                       "Метод Симпсона дал результат: %6\n\n"
                       "Найти: почему метод Симпсона предпочтительнее метода трапеций?\n\n"
                       "1) %7\n"
                       "2) %8\n"
                       "3) %9\n"
                       "4) %10\n\n"
                       "Введите номер правильного ответа: 1, 2, 3 или 4.")
                       .arg(funcName)
                       .arg(a, 0, 'f', 2)
                       .arg(b, 0, 'f', 2)
                       .arg(n)
                       .arg(trapResult, 0, 'f', 6)
                       .arg(simpsonResult, 0, 'f', 6)
                       .arg(option1)
                       .arg(option2)
                       .arg(option3)
                       .arg(option4);

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

    if (m_currentMethod == Method::Task4SimpsonChoice) {
        if (answer != "1" && answer != "2" && answer != "3" && answer != "4") {
            QMessageBox::warning(this, "Неверный ввод",
                                 "Для задания 4 нужно ввести номер ответа: 1, 2, 3 или 4.");
            return;
        }
    }

    emit checkRequested(answer);
}

void TaskWidget::on_btnBack_clicked()
{
    emit backToMenuRequested();
}
