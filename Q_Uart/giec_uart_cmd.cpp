#include "giec_uart_cmd.h"
#include <QDebug>
#include <JQchecksum.h>

Giec_Uart_Cmd::Giec_Uart_Cmd()
{

}

QString Giec_Uart_Cmd::send_cmd(int cmd_type,QString data)
{
    switch (cmd_type)
    {
        case GIEC_ITEM_SN_CMD:
        {
            QString temp = "GIEC0100";
            qDebug()<<"data length"<<data.length();
            temp+=QString().sprintf("%08x",data.length());
            temp+=data;
            auto crc32 = JQChecksum::giec_crc32( temp.toLatin1(),0,temp.toLatin1().length());
            temp+=QString().sprintf("%08x",crc32);
            //temp+"\n";
            qDebug()<<temp;
            return temp;
        }
        break;

        case GIEC_ITEM_CHIP_ID_CMD:
        {
            QString temp = "GIEC0200";
            temp+=QString().sprintf("%08x",0);
            //qDebug()<<QString().sprintf("%08x",0);
            auto crc32 = JQChecksum::giec_crc32( temp.toLatin1(),0,temp.toLatin1().length());
            //qDebug()<<"crc32"<<crc32;
            temp+=QString().sprintf("%08x",crc32);
            //temp+"\n";
            qDebug()<<temp;
            return temp;
        }
        break;

        default:
        {
            qDebug()<<"unkown cmd";
        }
        break;
    }
}
