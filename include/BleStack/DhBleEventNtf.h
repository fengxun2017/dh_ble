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

typedef enum
{
    BLE_EVENT_CONNECTED = 0x0001,
    BLE_EVENT_DISCONNECTED,
    BLE_EVENT_CONN_UPDATE,
    BLE_EVENT_RECV_WRITE,
    BlE_EVENT_RECV_HVC,                 // handle value confirm 对indication的响应 
}EnBleEvtType;



typedef struct
{
	EnBleEvtType   m_u2EvtType;			// 事件类型
	union
	{
        BlkBleRecvWriteEvent    m_blkWriteInfo;
        BlkBleConnectedEvent    m_blkConnInfo;
        BlkBleDisconnectedEvent m_blkDisconnInfo;
        BlkBleConnUpdateEvent   m_blkConnUpdateInfo;
	}m_event;
}BlkBleEvent;

typedef void (*BleEventHandler)(BlkBleEvent event);

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern u4 BleEventPush(BlkBleEvent event);
extern void DhBleEventNtfInit(void);
extern void DhBleEventHandlerReg(BleEventHandler handler);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DHBLEEVENTNTF_H__ */
