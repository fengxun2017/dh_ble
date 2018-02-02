/**
 * @file				BleLinkConnect.h
 * @brief			BleLinkConnect.c ��ͷ�ļ�
 * @author			fengxun
 * @date			2017年12月26日
*/
#ifndef __BLELINKCONNECT_H__
#define __BLELINKCONNECT_H__
#include "../../DhGlobalHead.h"

typedef struct
{
    u1  m_u1PeerBleAddrType;
    u1  m_pu1PeerBleAddr[BLE_ADDR_LEN];     /* 对端设备地址，LSB */
    u2  m_u2ConnInterval;                   /* 1.25ms为单位 */
    u2  m_u2SlaveLatency;
    u2  m_u2ConnTimeout;                    /* 10ms为单位 */
}BlkBleConnectedEvent;

typedef struct
{
    u2  m_u2ConnInterval;
    u2  m_u2SlaveLatency;
    u2  m_u2ConnTimeout;
}BlkBleConnUpdateEvent;

typedef struct
{
    u1  m_u1ErrCode;
}BlkBleDisconnectedEvent;



#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void LinkConnReqHandle(u1 addrType, u1 *pu1Addr, u1* pu1LLData);
extern void LinkConnStateInit(u1 sca);
void LinkConnStateReset(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLELINKCONNECT_H__ */
