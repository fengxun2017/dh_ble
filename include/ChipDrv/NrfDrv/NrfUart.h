
#ifndef __NRFUART_H__
#define __NRFUART_H__
#include "../../DhGlobalHead.h"




#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern u4  NrfUartBaudGet(EnUartBaudrate baud);
extern void NrfUartSimpleInit(u1 rxPin, u1 txpin, u4 baud);
extern void NrfUartSimpleTxByte(u1 byte);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __NRFUART_H__ */
