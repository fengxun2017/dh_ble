/**
 * @file				BleLinkAdvertising.h
 * @brief			BleLinkAdvertising.c µÄÍ·ÎÄ¼þ
 * @author			fengxun
 * @date			2017å¹´11æœˆ23æ—¥
*/
#ifndef __BLELINKADVERTISING_H__
#define __BLELINKADVERTISING_H__
#include "../../DhGlobalHead.h"

typedef struct 
{
	u1	m_ChannelOn_37:1;
	u1	m_ChannelOn_38:1;
	u1	m_ChannelOn_39:1;
}BlkAdvChannelOn;


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void LinkAdvAddrInfoCfg(BlkBleAddrInfo addr);
extern BlkBleAddrInfo LinkAdvAddrInfoGet(void);

extern u4 LinkAdvDataCfg(u1 *pu1Data, u2 u2Offset, u2	len);
extern void LinkAdvParamsCfg(BlkAdvChannelOn channels, u2 intervalMs);
extern u4 LinkAdvStart(void);
extern void LinkAdvStateInit(void);
extern u4	LinkScanRspCfg(u1 *pu1Data, u2 u2Offset, u2	len);
extern u2 LinkScanRspLenGet(void);
extern u2 LinkAdvDataLenGet(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLELINKADVERTISING_H__ */
