#ifndef QKUPLOADFILE_H
#define QKUPLOADFILE_H

#include <QObject>
#include <QWidget>
#include <QtNetwork>

class QKUploadFile:public QWidget
{
    Q_OBJECT
public:
    explicit QKUploadFile(QWidget *parent = 0);
    ~QKUploadFile();
    void uploadFile(QString uploadFilename);
    bool check_sn(QString data);
    static QKUploadFile * getInstance();

signals :
    void sn_is_used();
private:
    QNetworkAccessManager *_uploadManager;
    QNetworkReply *_reply;
    char *m_buf;
    QString filename;
    static QKUploadFile *d;
    bool Is_tested_Sn = true;

private slots:
    void replyFinished(QNetworkReply *reply);
    void upLoadError(QNetworkReply::NetworkError errorCode);
    void OnUploadProgress(qint64 bytesSent, qint64 bytesTotal);




};

#endif // QKUPLOADFILE_H
