/**
 * @file				BleLinkControl.h
 * @brief			BleLinkControl.c µÄÍ·ÎÄ¼þ
 * @author			fengxun
 * @date			2017å¹´12æœˆ29æ—¥
*/
#ifndef __BLELINKCONTROL_H__
#define __BLELINKCONTROL_H__
#include "../../DhGlobalHead.h"



typedef struct
{
    u1  m_u1ErrCode;
}BlkBleDisconnectReason;

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
extern u4 BleLinkControlHandle(u1 *pu1Data, u2 len);
extern u4 CheckLinkChannelMapUpdateReq(u1 *pu1Data, u1 *pu1NewChannelMap, u2 *u2Instant);
extern u4 CheckLinkConnUpdateReq(u1 *pu1PDU, u1 *u1WinSize, u2 *u2WinOffset, u2 *u2Interval, u2 *u2Latency, u2 *u2Timeout, u2 *u2Instant);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLELINKCONTROL_H__ */
