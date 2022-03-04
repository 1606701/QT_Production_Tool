#ifndef GIEC_UART_CMD_H
#define GIEC_UART_CMD_H

#include <QObject>
#include <QWidget>

typedef enum _GIEC_FACTORY_TEST_ITEM
{
    GIEC_ITEM_SN_CMD = 1,
    GIEC_ITEM_CHIP_ID_CMD
} GIEC_FACTORY_TEST_ITEM;

class Giec_Uart_Cmd
{

public:
    Giec_Uart_Cmd();
    QString send_cmd(int cmd_type,QString data);
};

#endif // GIEC_UART_CMD_H
