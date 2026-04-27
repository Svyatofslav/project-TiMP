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

    enum class Method {
        MiddleRectangles,
        LeftRectangles,
        Task3TrapezoidCheck
    };

    void setTask(const QString &funcName, double a, double b, int n, Method method);

signals:
    void checkRequested(const QString &answer);
    void backToMenuRequested();

private slots:
    void on_btnSend_clicked();
    void on_btnBack_clicked();

private:
    Method m_currentMethod = Method::MiddleRectangles;
    Ui::taskwidget *ui;
};

#endif // TASKWIDGET_H
