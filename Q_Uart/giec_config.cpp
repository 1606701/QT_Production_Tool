#include "giec_config.h"
#include <QtDebug>
#include <QCoreApplication>

Giec_Config::Giec_Config(QString qstrfilename)
{
    if (qstrfilename.isEmpty())
    {
        m_qstrFileName = QCoreApplication::applicationDirPath() + "/Config.ini";
    }
    else
    {
        m_qstrFileName = qstrfilename;
    }

    m_psetting = new QSettings(m_qstrFileName, QSettings::IniFormat);
    qDebug() <<"config file path:"<< m_qstrFileName;
}

Giec_Config::~Giec_Config()
{
    delete m_psetting;
    m_psetting = 0;
}

void Giec_Config::Set(QString qstrnodename,QString qstrkeyname,QVariant qvarvalue)
{
    m_psetting->setValue(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname), qvarvalue);
}

QVariant Giec_Config::Get(QString qstrnodename,QString qstrkeyname)
{
    QVariant qvar = m_psetting->value(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname));
    return qvar;
}
