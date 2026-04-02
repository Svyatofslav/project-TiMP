#include "statswidget.h"
#include "ui_statswidget.h"

StatsWidget::StatsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::statswidget)
{
    ui->setupUi(this);
    setWindowTitle("Статистика");
}

StatsWidget::~StatsWidget()
{
    delete ui;
}

void StatsWidget::setStats(const QString &login,
                           int t1, int t2, int t3, int t4)
{
    ui->label_2->setText(QString("Пользователь: %1").arg(login));
    ui->labelT1->setText(QString("Задание 1: %1").arg(t1));
    ui->labelT2->setText(QString("Задание 2: %1").arg(t2));
    ui->labelT3->setText(QString("Задание 3: %1").arg(t3));
    ui->labelT4->setText(QString("Задание 4: %1").arg(t4));
}

void StatsWidget::on_btnBack_clicked()
{
    emit backToMenuRequested();
}
