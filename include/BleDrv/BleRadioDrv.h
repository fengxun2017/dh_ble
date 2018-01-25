/**
 * @file			BleRadioDrv.h
 * @brief			BleRadioDrv.c头文件
 * @author			fengxun
 * @date			2017年11月20日
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
