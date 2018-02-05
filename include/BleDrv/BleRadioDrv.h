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

#ifndef __BLERADIODRV_H__
#define __BLERADIODRV_H__
#include "../DhGlobalHead.h"


typedef enum
{
	BLE_RADIO_EVT_TRANS_DONE = 0,		/* 发送或接收完成， 接收完成不一定是接收到数据了，也可能是接收超时主动关闭了接收*/
}EnBleRadioEvt;


typedef void (*BleRadioEvtHandler)(EnBleRadioEvt);

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

void BleRadioEvtHandlerReg( BleRadioEvtHandler intHandler );

extern void BleRadioCrcInit(u4 u4CrcInit);
extern void BleRadioDisable(void);
extern void BleRadioInit(void);
extern s1 BleRadioRssiGet(void);
extern void BleRadioRssiMeasuStart(void);
extern void BleRadioRssiMeasuStop(void);
extern void BleRadioRxData(u1 u1Channel, u1 *pu1Data);
extern void BleRadioTxData(u1 u1channel, u1 *pu1Data, u2 len);
extern void BleRadioSimpleTx(u1 *pu1Data, u2 len);
extern void BleRadioSimpleRx(u1 *pu1Data);

extern void BleRadioTxRxAddrCfg( u1 logicAddr, u4 u4AccAddr );
extern void BleRadioWhiteIvCfg(u1 iv);
extern u1 IsBleRadioCrcOk(void);
extern void BleAutoToRxEnable(void);
extern void BleAutoToRxDisable(void);

extern void BleAutoToTxEnable(void);

extern void BleAutoToTxDisable(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLERADIODRV_H__ */
