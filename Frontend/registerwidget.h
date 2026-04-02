#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>

namespace Ui {
class registerwidget;
}

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = nullptr);
    ~RegisterWidget();

signals:
    void registerRequested(const QString &login,
                           const QString &email,
                           const QString &password1,
                           const QString &password2);
    void switchToAuth();

private slots:
    void on_btnRegister_clicked();
    void on_btnToAuth_clicked();

private:
    Ui::registerwidget *ui;
};

#endif // REGISTERWIDGET_H
