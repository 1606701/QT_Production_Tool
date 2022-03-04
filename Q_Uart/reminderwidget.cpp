#include "reminderwidget.h"
#include "ui_reminderwidget.h"
#include <QTimer>
ReminderWidget::ReminderWidget(QString text,int time_count,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReminderWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    //text为要显示的信息
    ui->label->setText(text);
    //设置定时器，到时自我销毁
    QTimer* timer = new QTimer(this);
    timer->start(time_count);//时间1.5秒
    timer->setSingleShot(true);//仅触发一次
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeupDestroy()));
}

ReminderWidget::~ReminderWidget()
{
    delete ui;
}
