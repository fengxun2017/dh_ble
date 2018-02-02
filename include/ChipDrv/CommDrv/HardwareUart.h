/**
 * @file				HarwareUart.h
 * @brief			HarwareUart.c 头文件
 * @author			fengxun
 * @date			2018年2月1日
*/
#ifndef __HARWAREUART_H__
#define __HARWAREUART_H__
#include "../../DhGlobalHead.h"


typedef enum
{
    UART_BAUDRATE_1200 = 0,
    UART_BAUDRATE_2400,
    UART_BAUDRATE_4800,
    UART_BAUDRATE_9600,
    UART_BAUDRATE_14400,
    UART_BAUDRATE_19200,
    UART_BAUDRATE_28800,
    UART_BAUDRATE_38400,
    UART_BAUDRATE_57600,
    UART_BAUDRATE_76800,
    UART_BAUDRATE_115200,
    UART_BAUDRATE_230400,
    UART_BAUDRATE_250000,
    UART_BAUDRATE_460800,
    UART_BAUDRATE_921600,
    UART_BAUDRATE_1M,
}EnUartBaudrate;

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void HwUartSimpleInit(u1 txPin, u1 rxPin, EnUartBaudrate enBaud);
extern void HwUartSimpleTxData(u1 *pu1Data, u2 len);
extern void DhPrintf(const char *fmt,...);
extern void DhPrintfLineFeed(const char *fmt,...);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HARWAREUART_H__ */
