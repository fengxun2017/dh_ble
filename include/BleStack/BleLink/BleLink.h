/**
 * @file				BleLink.h
 * @brief			BleLink.c 头文件
 * @author			fengxun
 * @date			2017年11月21日
*/
#ifndef __BLELINK_H__
#define __BLELINK_H__
#include "../../DhGlobalHead.h"

typedef enum
{
	BLE_LINK_ADVERTISING = 0,		/* 广播中 */
	BLE_LINK_CONNECTING,			/* 连接中 */
	BLE_LINK_CONNECTED,				/* 已连接 */
	BLE_LINK_STANDBY,				/* 空闲待命状态*/
	
	BLE_LINK_STATE_END,
}EnBleLinkState;


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void BleLinkInit(void);
extern void BleLinkStateSwitch(EnBleLinkState state);
extern void BleLinkStateHandlerReg(EnBleLinkState state, RadioEvtHandler evtHandler);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLELINK_H__ */
