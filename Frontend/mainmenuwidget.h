#ifndef MAINMENUWIDGET_H
#define MAINMENUWIDGET_H

#include <QWidget>

namespace Ui {
class mainmenuwidget;
}

class MainMenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuWidget(QWidget *parent = nullptr);
    ~MainMenuWidget();

signals:
    void task1Clicked();
    void task2Clicked();
    void task3Clicked();
    void task4Clicked();
    void statsClicked();

private slots:
    void on_btnTask1_clicked();
    void on_btnTask2_clicked();
    void on_btnTask3_clicked();
    void on_btnTask4_clicked();
    void on_btnStats_clicked();

private:
    Ui::mainmenuwidget *ui;
};

#endif // MAINMENUWIDGET_H
