#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>

namespace Ui {
class taskwidget;
}

class TaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskWidget(QWidget *parent = nullptr);
    ~TaskWidget();

    void setTask(const QString &funcName, double a, double b, int n);

signals:
    void checkRequested(const QString &answer);
    void backToMenuRequested();

private slots:
    void on_btnSend_clicked();
    void on_btnBack_clicked();

private:
    Ui::taskwidget *ui;
};

#endif // TASKWIDGET_H
