#include "../../include/DhGlobalHead.h"

u4  NrfUartBaudGet(EnUartBaudrate baud)
{
    u4 baud;
    
    switch(baud)
    {
        case UART_BAUDRATE_BAUDRATE_Baud115200:
            baud = 0x01D7E000;
        break; 
        case UART_BAUDRATE_BAUDRATE_Baud1200:
            baud = 0x0004F000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud14400:
            baud = 0x003B0000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud19200:
            baud = 0x004EA000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud1M:
            baud = 0x10000000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud230400:
            baud = 0x03AFB000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud2400:
            baud = 0x0009D000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud250000:
            baud = 0x04000000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud28800:
            baud = 0x0075F000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud38400:
            baud = 0x009D5000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud460800:
            baud = 0x075F7000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud4800:
            baud = 0x0013B000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud57600:
            baud = 0x00EBF000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud76800:
            baud = 0x013A9000;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud921600:
            baud = 0x0EBEDFA4;
        break;
        case UART_BAUDRATE_BAUDRATE_Baud9600:
            baud = 0x00275000;
        break;
        default:
            baud = 0x00275000;
        break;   
    }

    return baud;
}

void NrfUartSimpleInit(u1 rxPin, u1 txpin, u4 baud)
{
    NRF_UART0->PSELTXD = txpin;
    NRF_UART0->PSELRXD = rxPin;
    NRF_UART0->PSELCTS = 0xFFFFFFFF;
    NRF_UART0->PSELRTS = 0xFFFFFFFF;
    NRF_UART0->CONFIG = 0x00;

    NRF_UART0->BAUDRATE = baud;
    NRF_UART0->ENABLE = 0x04;
}

void NrfUartSimpleTxByte(u1 byte)
{
    u2 count = 20000;
    NRF_UART0->EVENTS_TXDRDY = 0;
    NRF_UART0->TXD = byte;
    NRF_UART0->TASKS_STARTTX = 1;
    while( 0==NRF_UART0->EVENTS_TXRDY && (count--) )
    {
    }
}
