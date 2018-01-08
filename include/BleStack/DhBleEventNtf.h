/**
 * @file				DhBleEventNtf.h
 * @brief			DhBleEventNtf.c ��ͷ�ļ�
 * @author			fengxun
 * @date			2017年12月26日
*/
#ifndef __DHBLEEVENTNTF_H__
#define __DHBLEEVENTNTF_H__

#include "../DhGlobalHead.h"

#define DH_BLE_CONNECTED				// 蓝牙连接
#define DH_BLE_DISCONNECTED				// 蓝牙断开




typedef struct
{
	u2	m_u2EvtType;			// 事件类型
}BlkBleEvent;

typedef void (*BleEventHandler)(BlkBleEvent *event);

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DHBLEEVENTNTF_H__ */
