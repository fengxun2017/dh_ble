/*
* (The MIT License)

* Copyright (c) 2018 Feng Xun

* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* 'Software'), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:

* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include "../../../include/DhGlobalHead.h"

u4  NrfUartBaudGet(EnUartBaudrate enBaud)
{
    u4 baud;
    
    switch(enBaud)
    {
        case UART_BAUDRATE_115200:
            baud = 0x01D7E000;
        break; 
        case UART_BAUDRATE_1200:
            baud = 0x0004F000;
        break;
        case UART_BAUDRATE_14400:
            baud = 0x003B0000;
        break;
        case UART_BAUDRATE_19200:
            baud = 0x004EA000;
        break;
        case UART_BAUDRATE_1M:
            baud = 0x10000000;
        break;
        case UART_BAUDRATE_230400:
            baud = 0x03AFB000;
        break;
        case UART_BAUDRATE_2400:
            baud = 0x0009D000;
        break;
        case UART_BAUDRATE_250000:
            baud = 0x04000000;
        break;
        case UART_BAUDRATE_28800:
            baud = 0x0075F000;
        break;
        case UART_BAUDRATE_38400:
            baud = 0x009D5000;
        break;
        case UART_BAUDRATE_460800:
            baud = 0x075F7000;
        break;
        case UART_BAUDRATE_4800:
            baud = 0x0013B000;
        break;
        case UART_BAUDRATE_57600:
            baud = 0x00EBF000;
        break;
        case UART_BAUDRATE_76800:
            baud = 0x013A9000;
        break;
        case UART_BAUDRATE_921600:
            baud = 0x0EBEDFA4;
        break;
        case UART_BAUDRATE_9600:
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
    while( 0==NRF_UART0->EVENTS_TXDRDY && (count--) )
    {
    }
}
