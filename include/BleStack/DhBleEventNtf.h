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
    
    BLE_EVENT_SM_DISP_KEY,
    BLE_EVENT_SM_INPUT_KEY,
    BLE_EVENT_SM_PAIRING_REQ,
    BLE_EVENT_SM_ENC_COMPLETE,          // 链路加密过程完成。
    BLE_EVENT_SM_BONDING_COMPLETE,
    BLE_EVENT_SM_LTK_REQ,
    BLE_EVENT_SM_FAILED,
}EnBleEvtType;




typedef struct
{
	EnBleEvtType   m_u2EvtType;			// 事件类型
	union
	{
        BlkBleRecvWriteEvent            m_blkWriteInfo;
        BlkBleConnectedEvent            m_blkConnInfo;
        BlkBleDisconnectedEvent         m_blkDisconnInfo;
        BlkBleConnUpdateEvent           m_blkConnUpdateInfo;
        
        BlkBleSmKeyDispEvent            m_blkSmKeyDisp;
        BlkBleSmKeyInputEvent           m_blkSmKeyInput;
        BlkBleSmPairingReqEvent         m_blkSmPairingReq;
        BlkBleSmFailedEvent             m_blkSmFailed;
        BlkBleSmEncCompleteEvent        m_blkSmEncComplete;
        BlKBleSmBondingCompleteEvent    m_blkSmBondingComplete;
        BlkBleSmLtkReqEvent             m_blkSmLtkReq;
	}m_event;
}BlkBleEvent;

typedef void (*BleEventHandler)(BlkBleEvent *event);

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
