#include "mainmenuwidget.h"
#include "ui_mainmenuwidget.h"

MainMenuWidget::MainMenuWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainmenuwidget)      // ← имя из ui_mainmenuwidget.h
{
    ui->setupUi(this);
    setWindowTitle("Главное меню");
}

MainMenuWidget::~MainMenuWidget()
{
    delete ui;
}

void MainMenuWidget::on_btnTask1_clicked()
{
    emit task1Clicked();
}

void MainMenuWidget::on_btnTask2_clicked()
{
    emit task2Clicked();
}

void MainMenuWidget::on_btnTask3_clicked()
{
    emit task3Clicked();
}

void MainMenuWidget::on_btnTask4_clicked()
{
    emit task4Clicked();
}

void MainMenuWidget::on_btnStats_clicked()
{
    emit statsClicked();
}
