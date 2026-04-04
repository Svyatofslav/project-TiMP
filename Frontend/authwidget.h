#ifndef AUTHWIDGET_H
#define AUTHWIDGET_H

#include <QWidget>

namespace Ui {
class authwidget;
}

class AuthWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AuthWidget(QWidget *parent = nullptr);
    ~AuthWidget();

    void clearFields();

signals:
    void loginRequested(const QString &login, const QString &password);
    void switchToRegister();

private slots:
    void on_btnLogin_clicked();
    void on_btnToRegister_clicked();

private:
    Ui::authwidget *ui;
};

#endif // AUTHWIDGET_H
