#ifndef STATSWIDGET_H
#define STATSWIDGET_H

#include <QWidget>

namespace Ui {
class statswidget;
}

class StatsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatsWidget(QWidget *parent = nullptr);
    ~StatsWidget();

    void setStats(const QString &login,
                  int t1, int t2, int t3, int t4);

signals:
    void backToMenuRequested();

private slots:
    void on_btnBack_clicked();

private:
    Ui::statswidget *ui;
};

#endif // STATSWIDGET_H
