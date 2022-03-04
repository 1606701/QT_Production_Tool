#ifndef GIEC_CONFIG_H
#define GIEC_CONFIG_H

#include <QObject>
#include <QWidget>

#include <QVariant>
#include <QSettings>

class Giec_Config
{
public:
    Giec_Config(QString qstrfilename = "");
    virtual ~Giec_Config(void);
    void Set(QString,QString,QVariant);
    QVariant Get(QString,QString);
private:
    QString m_qstrFileName;
    QSettings *m_psetting;
};

#endif // GIEC_CONFIG_H
