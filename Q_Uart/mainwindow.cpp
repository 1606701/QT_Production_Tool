/*
 * Author：cm
 * Data  : 2022.2.17
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "JQchecksum.h"
#include "qkuploadfile.h"
#include "reminderwidget.h"
#include <QKeyEvent>
#include "giec_uart_cmd.h"
#include "giec_config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->timer=new QTimer(this);

    QObject::connect(this->timer,&QTimer::timeout,this,&MainWindow::serialPortSend);

    this->serialPortDetect();

    this->serialPort=nullptr;

    this->setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);

    //ui->checkBox_send_newline->setCheckState(Checked);
    ui->textEdit_send->setFocus();
    ui->textEdit_send->installEventFilter(this);

    //this->setFixedSize(this->width(),this->height());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::serialPortOpen2Close()
{
    if(this->serialPort==nullptr)
    {
        this->serialPort = new QSerialPort(this);

        this->serialPort->setPortName(ui->comboBox_serialName->currentText());

        this->setSerialPortBaud();
        this->setSerialDataBits();
        this->setSerialParity();
        this->setSerialStopBits();

        if(!serialPort->open( QIODevice::ReadWrite ))
        {
            delete serialPort;
            this->serialPort=nullptr;
            ui->pushButton_open2close->setText("Open");
            QMessageBox::information(this,"error","open failed");
        }
        else
        {
            ui->pushButton_open2close->setText("Close");
            QObject::connect(this->serialPort,&QSerialPort::readyRead,this,&MainWindow::serialPortReceive);
        }
    }
    else
    {
        serialPort->close();
        delete serialPort;
        serialPort=nullptr;
        ui->pushButton_open2close->setText("Open");
    }

}

void MainWindow::serialPortDetect()
{
    this->detectFlag=1;
    QList<QSerialPortInfo> serialPortList(QSerialPortInfo::availablePorts());
    QSerialPort tempSerial;
    ui->comboBox_serialName->clear();
    for(int i=0;i<serialPortList.size();i++)
    {
        tempSerial.setPort(serialPortList.at(i));
        ui->comboBox_serialName->addItem(serialPortList.at(i).portName());
        tempSerial.close();

    }
    this->detectFlag=0;
}

void MainWindow::serialPortSend()
{
    //qDebug()<<ui->textEdit_send->toPlainText().toLatin1();
    if(ui->checkBox_hex_send->checkState()==Qt::Unchecked)
    {
        this->serialPort->write(ui->textEdit_send->toPlainText().toLocal8Bit());

        this->sendNumber+=ui->textEdit_send->toPlainText().toLocal8Bit().size();
    }
    else
    {
        this->sendNumber +=this->serialPort->write(QByteArray::fromHex( ui->textEdit_send->toPlainText().toLocal8Bit() ));
    }
    if(ui->checkBox_send_newline->isChecked())
    {
        this->sendNumber += this->serialPort->write("\n", 1);
    }
    ui->label_send_number->setText(QString::number(this->sendNumber));
}

void MainWindow::delayms(int num)
{
    QTime time;
            time.start();
            while(time.elapsed() < num)             //等待时间流逝num ms
                QCoreApplication::processEvents();   //处理事件
}

bool MainWindow::check_cmd_CRC32(QString data)
{
    if(data.isEmpty())
        return false;
    QString crcFromSTB = data.mid(data.length() - 8, 8);
    QString crc = QString().sprintf("%08x",JQChecksum::giec_crc32(data.toLatin1(),0,data.length() - 8));//myTools.Calc_Crc32(dataStr.substring(0, dataStr.length() - 8));
    qDebug()<<"crcFromSTB=" + crcFromSTB + "\ncrc=" + crc;
    if (crcFromSTB == crc) {
        return true;
    } else {
        return false;
    }
}

void MainWindow::upload_data(GIEC_TEST_IETM test_item)
{
    QKUploadFile *my_upload;
    my_upload = new QKUploadFile(nullptr);
    QString data;
    data = "&up_sn="+test_item.SN+"&up_led=OK&up_av=OK&up_hdmi=OK&up_StartTime=20220225\
            &up_FasNo=OK&up_BoardSN=666777888&up_ChipID="+test_item.CHIP_ID+"&up_SID=123&up_APPV=OK\
            &up_LDRV=OK&up_IRE=OK&up_IR=OK&up_AMT=OK&up_ANT=OK&up_Panel=OK&up_CHLOCK=OK\
            &up_FCT=OK&up_USB=OK&up_SIDT=OK&up_APPVT=OK&up_LDRVT=OK&up_SNT=OK&up_STEDBM=OK\
            &up_FACFLAG=OK&up_EndTime=20220226";
    my_upload->uploadFile(data);
}
void MainWindow::handler_recv_data(QString data)
{
    if(data == nullptr)
    {

    }
    if(data.contains("startERRORCMDend"))
    {

    }
    if(data.contains("startGIEC"))
    {
        QString msg = data.mid(data.indexOf("startGIEC")+5);
        if(msg.contains("end"))
        {
         msg = msg.mid(0,msg.indexOf("end"));
        }
        else
        {
            qDebug()<<"msg not contains end";
        }
        qDebug()<<"msg = "<<msg;
        //check data crc
        if(check_cmd_CRC32(msg) == false)
            return;
        else
            qDebug()<<"check crc OK";
        //switch data type

        switch(msg.at(5).toLatin1() - '0')
        {
            case GIEC_ITEM_SN_CMD:
            {
                if(msg.at(16) == 'N' && msg.at(17) == 'G')
                {
                    ReminderWidget *my_reminder = new ReminderWidget("test write SerialNUmber NG",2000,nullptr);
                    my_reminder->show();
                }
                else
                {
                    ui->textEdit_receive->append("write serialnumber ok");

                    Giec_Uart_Cmd *my_cmd;
                    my_cmd = new Giec_Uart_Cmd();
                    if(this->serialPort != nullptr)
                    {
                        QByteArray temp = my_cmd->send_cmd(GIEC_ITEM_CHIP_ID_CMD,"").toLatin1();
                        qDebug()<<temp<<temp.length();
                        this->serialPort->write(temp+"\n",temp.length()+1);
                        //this->serialPort->write("\n",1);
                    }
                }
            }
            break;
            case GIEC_ITEM_CHIP_ID_CMD:
            {
                if(msg.at(16) == 'N' && msg.at(17) == 'G')
                {
                    ReminderWidget *my_reminder = new ReminderWidget("test chip id NG",2000,nullptr);
                    my_reminder->show();
                }
                else
                {
                    ui->textEdit_receive->append("chip id get");
                    QString chipid = msg.mid(16,msg.length() - 8 - 15);
                    qDebug()<<chipid;
                    ui->textEdit_receive->append("chip id = "+chipid);

                    giec_test.CHIP_ID = chipid;//get chip id
                    this->upload_data(giec_test);
                }
            }
            break;
            default:
            qDebug()<<"unkown cmd";
            break;
        }
    }
}

QString recv_buf;
void MainWindow::serialPortReceive()
{
    QByteArray temp = this->serialPort->readAll();//1024);

    recv_buf.append(temp);
    if(recv_buf.contains("end"))
    {
        qDebug()<<"recv ok";
        qDebug()<<recv_buf;
        ui->textEdit_receive->append(recv_buf);
        handler_recv_data(recv_buf);
        recv_buf.clear();
    }
    /*
    QString tempStr;
    if(ui->checkBox_hex_show->checkState()==Qt::Unchecked)
    {
        tempStr=QString::fromLocal8Bit(temp);
    }
    else
    {
        tempStr=temp.toHex();
    }

    if(!temp.isEmpty())
    {
        //ui->textEdit_receive->moveCursor(QTextCursor::End);
        //ui->textEdit_receive->insertPlainText(tempStr);
        ui->textEdit_receive->append(recv_buf);
    }
    this->receiveNumber+=temp.size();
    ui->label_receive_number->setText(QString::number(this->receiveNumber));
    temp.clear();
    tempStr.clear();*/

}

void MainWindow::setSerialPortBaud(){
    if(this->serialPort != nullptr){
        this->serialPort->setBaudRate(ui->comboBox_baud->currentText().toInt());
    }
}

void MainWindow::setSerialDataBits(){
    if(this->serialPort != nullptr){
        switch(ui->comboBox_dataBits->currentText().toInt())
        {
        case 8:
            this->serialPort->setDataBits(QSerialPort::Data8);break;
        case 7:
            this->serialPort->setDataBits(QSerialPort::Data7);break;
        case 6:
            this->serialPort->setDataBits(QSerialPort::Data6);break;
        case 5:
            this->serialPort->setDataBits(QSerialPort::Data5);break;
        default:
            break;
        }
    }
}

void MainWindow::setSerialParity(){
    if(this->serialPort != nullptr){
        switch(ui->comboBox_parity->currentIndex())
        {
        case 0:
            this->serialPort->setParity(QSerialPort::NoParity);
            break;
        case 1:
            this->serialPort->setParity(QSerialPort::OddParity);
            break;
        case 2:
            this->serialPort->setParity(QSerialPort::EvenParity);
            break;
        default:
            break;
        }
    }
}

void MainWindow::setSerialStopBits(){
    if(this->serialPort != nullptr){
        switch (ui->comboBox_stopBits->currentIndex())
        {
        case 0:
            this->serialPort->setStopBits(QSerialPort::OneStop);
            break;
        case 1:
            this->serialPort->setStopBits(QSerialPort::OneAndHalfStop);
            break;
        case 2:
            this->serialPort->setStopBits(QSerialPort::TwoStop);
            break;
        default:
            break;
        }
    }
}


void MainWindow::on_pushButton_open2close_clicked()
{
    this->serialPortOpen2Close();
}

void MainWindow::on_pushButton_send_clicked()
{
    if(this->serialPort!=nullptr)
    {
        this->serialPortSend();
    }
}

void MainWindow::on_pushButton_clearRec_clicked()
{
    ui->textEdit_receive->clear();
}

void MainWindow::on_pushButton_clearSend_clicked()
{
    ui->textEdit_send->clear();
}

void MainWindow::on_comboBox_dataBits_currentIndexChanged(int index)
{
    if(this->serialPort)
    {
        switch(index)
        {
        case 0:
            this->serialPort->setDataBits(QSerialPort::Data8);break;
        case 1:
            this->serialPort->setDataBits(QSerialPort::Data7);break;
        case 2:
            this->serialPort->setDataBits(QSerialPort::Data6);break;
        case 3:
            this->serialPort->setDataBits(QSerialPort::Data5);break;
        default:
            break;
        }
    }

}

void MainWindow::on_comboBox_baud_currentIndexChanged(const QString &arg1)
{
    if(this->serialPort)
    {
        this->serialPort->setBaudRate(arg1.toInt());
    }
}

void MainWindow::on_comboBox_stopBits_currentIndexChanged(int index)
{
    if(this->serialPort)
    {
        switch (index)
        {
        case 0:
            this->serialPort->setStopBits(QSerialPort::OneStop);
            break;
        case 1:
            this->serialPort->setStopBits(QSerialPort::OneAndHalfStop);
            break;
        case 2:
            this->serialPort->setStopBits(QSerialPort::TwoStop);
            break;
        default:
            break;
        }
    }
}

void MainWindow::on_comboBox_parity_currentIndexChanged(int index)
{
    if(this->serialPort)
    {
        switch(index)
        {
        case 0:
            this->serialPort->setParity(QSerialPort::NoParity);
            break;
        case 1:
            this->serialPort->setParity(QSerialPort::OddParity);
            break;
        case 2:
            this->serialPort->setParity(QSerialPort::EvenParity);
            break;
        default:
            break;
        }
    }
}

void MainWindow::on_comboBox_serialName_currentIndexChanged(const QString &)
{
    if(this->serialPort&&!this->detectFlag)
    {
        this->serialPort->close();
        delete serialPort;
        this->serialPort=nullptr;
        this->serialPortOpen2Close();
    }
}

void MainWindow::on_pushButton_clear_log_clicked()
{
    this->sendNumber=0;
    this->receiveNumber=0;
    ui->label_receive_number->setText(QString::number(0));
    ui->label_send_number->setText(QString::number(0));
}

void MainWindow::on_checkBox_timer_send_stateChanged(int arg1)
{
    if(this->serialPort)
    {
        if(arg1==Qt::Checked)
        {
            this->timer->start(ui->lineEdit_send_time->text().toInt());
        }
        if(arg1==Qt::Unchecked)
        {
            this->timer->stop();
        }
    }

}

void MainWindow::on_pushButton_detect_serial_clicked()
{
    this->serialPortDetect();
}

void MainWindow::on_comboBox_baud_activated(const QString &)
{
    this->setSerialPortBaud();
}

void MainWindow::on_comboBox_dataBits_activated(const QString &)
{
    this->setSerialDataBits();
}

void MainWindow::on_comboBox_stopBits_activated(const QString &)
{
    this->setSerialStopBits();
}

void MainWindow::on_comboBox_parity_activated(const QString &)
{
    this->setSerialParity();
}

void MainWindow::on_comboBox_serialName_activated(const QString &)
{
    //this->serialPortDetect();

    if(this->serialPort != nullptr){
        this->serialPortOpen2Close();
        this->serialPortOpen2Close();
    }
}
void MainWindow::handler_sn_is_used()
{
    //this->serialPortOpen2Close();
    QMessageBox::information(this,"test waring","sn already be used");
    check_sn_flag = false;
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->textEdit_send)
    {
        if(event->type() == QEvent::KeyPress)//回车键
        {
             QKeyEvent *k = static_cast<QKeyEvent *>(event);
             if(k->key() == Qt::Key_Return)
             {
                 check_sn_flag = true;
                 ui->textEdit_receive->clear();
                 QKUploadFile *my_up;
                 my_up = new QKUploadFile(nullptr);
                 connect(my_up,SIGNAL(sn_is_used()),this,SLOT(handler_sn_is_used()));
                 my_up->check_sn(ui->textEdit_send->toPlainText());
                 if(check_sn_flag == false)
                     return false;

                 giec_test.SN = ui->textEdit_send->toPlainText();//get sn
                 if(this->serialPort!=nullptr)
                 {
                     qDebug()<<"send data";
                     //this->serialPortSend();
                     Giec_Uart_Cmd *my_cmd;
                     my_cmd = new Giec_Uart_Cmd();

                     QByteArray temp = my_cmd->send_cmd(GIEC_ITEM_SN_CMD,ui->textEdit_send->toPlainText()).toLatin1();
                     qDebug()<<temp<<temp.length();
                     this->serialPort->write(temp+"\n",temp.length()+1);
                 }
                 else
                 {
                     QMessageBox::information(this,"test waring","com not open");
                 }
                 return true;
             }
        }
    }
    return QWidget::eventFilter(target,event);
}
void MainWindow::on_pushButton_clicked()
{
    QKUploadFile *my_upload;
    my_upload = new QKUploadFile(nullptr);
    connect(my_upload,SIGNAL(sn_is_used()),this,SLOT(handler_sn_is_used()));
    QString data;
    data = "&up_sn=91123456789004&up_led=OK&up_av=OK&up_hdmi=OK&up_StartTime=20220225\
            &up_FasNo=OK&up_BoardSN=666777888&up_ChipID=buf9172912g9&up_SID=123&up_APPV=OK\
            &up_LDRV=OK&up_IRE=OK&up_IR=OK&up_AMT=OK&up_ANT=OK&up_Panel=OK&up_CHLOCK=OK\
            &up_FCT=OK&up_USB=OK&up_SIDT=OK&up_APPVT=OK&up_LDRVT=OK&up_SNT=OK&up_STEDBM=OK\
            &up_FACFLAG=OK&up_EndTime=20220226";
    my_upload->uploadFile(data);
}

void MainWindow::on_pushButton_2_clicked()
{
    qDebug()<<Giec_Config().Get("giec_test","factory_ip").toString();
}
