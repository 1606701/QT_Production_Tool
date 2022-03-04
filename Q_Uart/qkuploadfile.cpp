#include "qkuploadfile.h"
#include "giec_config.h"
#include <QMessageBox>
QKUploadFile *QKUploadFile::d = 0;
QKUploadFile::QKUploadFile(QWidget *parent)
    :QWidget(parent)
{
    _uploadManager = new QNetworkAccessManager(this);
     connect(_uploadManager,SIGNAL(finished(QNetworkReply*)),SLOT(replyFinished(QNetworkReply*)));
     m_buf = NULL;
}

QKUploadFile::~QKUploadFile()
{
    if( m_buf != NULL ) delete[] m_buf;
}

QKUploadFile * QKUploadFile::getInstance()
{
    if (d == 0)
        d = new QKUploadFile;
    return d;
}

bool QKUploadFile::check_sn(QString data)
{
    QString ip = Giec_Config().Get("giec_test","factory_ip").toString();
    QString model = Giec_Config().Get("giec_test","stb_model").toString();
    qDebug()<<ip<<model;
    QString header = "http://"+ip+"/box_cms_v3/get_data_bykey.php?model="+model+"&up_sn="+data+"&workshop=2";
    QUrl url = QString(header);
    QNetworkRequest request( url );
    _reply = _uploadManager->post( request , "" );

    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(upLoadError(QNetworkReply::NetworkError)));
    connect(_reply, SIGNAL(uploadProgress ( qint64 ,qint64 )), this, SLOT( OnUploadProgress(qint64 ,qint64 )));
    return true;
}
void QKUploadFile::uploadFile(QString upload_data)
{
    QString sn_data;
    sn_data = "91123456789005";
    if(check_sn(sn_data) == false)
        return;
    QString header;
    QString ip = Giec_Config().Get("giec_test","factory_ip").toString();
    QString model = Giec_Config().Get("giec_test","stb_model").toString();
    header = "http://"+ip+"/box_cms_v3/collectinfo.php?model="+model;
    QUrl url = QString(header+upload_data);
    QNetworkRequest request( url );

    QByteArray arr = "";
    _reply = _uploadManager->post( request , arr );

    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(upLoadError(QNetworkReply::NetworkError)));
    connect(_reply, SIGNAL(uploadProgress ( qint64 ,qint64 )), this, SLOT( OnUploadProgress(qint64 ,qint64 )));
}


void QKUploadFile::replyFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        qDebug()<<"no error.....";
        QByteArray bytes = reply->readAll();  //获取字节
        QString result(bytes);  //转化为字符串
        qDebug()<<result;
        if(result.contains("已测试通过"))
        {
            emit sn_is_used();//关闭串口
        }
        if(result.contains("upload_ok"))
        {
            QMessageBox *m_box = new QMessageBox(QMessageBox::Information,QString("info"),QString("upload OK"));
            QTimer::singleShot(1000,m_box,SLOT(accept()));
            m_box->exec();
        }
    }
    else{
        qDebug() << "replyFinished:  " << reply->error() << " " <<reply->errorString();
    }

    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug()<<status_code;
}

void QKUploadFile::upLoadError(QNetworkReply::NetworkError errorCode)
{
    qDebug() << "upLoadError  errorCode: " << (int)errorCode;
}

void QKUploadFile::OnUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    qDebug() << "bytesSent: " << bytesSent << "  bytesTotal: "<< bytesTotal;
}
