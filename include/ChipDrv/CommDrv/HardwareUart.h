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
    UART_BAUD_1200 = 0,
    UART_BAUD_2400,
    UART_BAUD_4800,
    UART_BAUD_9600,
    UART_BAUD_14400,
    UART_BAUD_19200,
    UART_BAUD_28800,
    UART_BAUD_38400,
    UART_BAUD_57600,
    UART_BAUD_76800,
    UART_BAUD_115200,
    UART_BAUD_230400,
    UART_BAUD_250000,
    UART_BAUD_460800,
    UART_BAUD_921600,
    UART_BAUD_1M,
}EnUartBaudrate;

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void HwUartSimpleInit(u1 txPin, u1 rxPin, EnUartBaudrate enBaud);
extern void HwUartSimpleTxData(u1 *pu1Data, u2 len);
extern void DhPrintf(const char *fmt,...);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HARWAREUART_H__ */
