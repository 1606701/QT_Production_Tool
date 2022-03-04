#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include "JQchecksum.h"
namespace Ui {
class MainWindow;
}
typedef struct test_item
{
    QString SN;
    QString CHIP_ID;
}GIEC_TEST_IETM;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    GIEC_TEST_IETM giec_test;
    bool check_sn_flag = true;
private slots:
    void handler_sn_is_used();

    void upload_data(GIEC_TEST_IETM test_item);

    void on_pushButton_open2close_clicked();

    void on_pushButton_send_clicked();

    void on_pushButton_clearRec_clicked();

    void on_pushButton_clearSend_clicked();

    void on_pushButton_clear_log_clicked();

    void on_comboBox_baud_currentIndexChanged(const QString &arg1);

    void on_comboBox_dataBits_currentIndexChanged(int index);

    void on_comboBox_stopBits_currentIndexChanged(int index);

    void on_comboBox_parity_currentIndexChanged(int index);

    void on_comboBox_serialName_currentIndexChanged(const QString &arg1);

    void on_checkBox_timer_send_stateChanged(int arg1);

    void on_pushButton_detect_serial_clicked();

    void on_comboBox_baud_activated(const QString &arg1);

    void on_comboBox_dataBits_activated(const QString &arg1);

    void on_comboBox_stopBits_activated(const QString &arg1);

    void on_comboBox_parity_activated(const QString &arg1);

    void on_comboBox_serialName_activated(const QString &arg1);
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

public:
    void serialPortReceive();
private:
    Ui::MainWindow *ui;
    QSerialPort* serialPort;
    QTimer *timer;

    bool detectFlag=0;

    qint64 sendNumber=0;
    qint64 receiveNumber=0;

    void serialPortOpen2Close();
    void serialPortDetect();
    void serialPortSend();


    void setSerialPortBaud();
    void setSerialDataBits();
    void setSerialParity();
    void setSerialStopBits();

    void setSerialName();

    void delayms(int num);

    void handler_recv_data(QString data);
    bool check_cmd_CRC32(QString data);
protected:
    bool eventFilter(QObject *target, QEvent *event);//事件过滤器

};

#endif // MAINWINDOW_H
