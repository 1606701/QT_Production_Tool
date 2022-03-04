#ifndef REMINDERWIDGET_H
#define REMINDERWIDGET_H

#include <QWidget>

namespace Ui {
class ReminderWidget;
}

class ReminderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReminderWidget(QString text,int time_count,QWidget *parent = nullptr);
    ~ReminderWidget();

private slots:
    void onTimeupDestroy(){
        delete this;
    }

private:
    Ui::ReminderWidget *ui;
};

#endif // REMINDERWIDGET_H
