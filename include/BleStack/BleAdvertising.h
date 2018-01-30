

#ifndef __BLEADVERTISING_H__
#define __BLEADVERTISING_H__
#include "../DhGlobalHead.h"

typedef struct
{
	u1		m_u1AddrType;				/* 0-public  1-random */
	u1		m_pu1Addr[BLE_ADDR_LEN];	/* 设备地址,LSB     */
}BlkBleAddrInfo;

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

extern u4 BleAdvDataCfg(u1 *pu1Data, u2 len);
extern u4 BleAdvStart(BlkAdvChannelOn channels, u2 IntervalMs);
extern u4 BleScanRspDataCfg(u1 *pu1Data, u2 len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLEADVERTISING_H__ */
