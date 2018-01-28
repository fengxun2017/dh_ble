

#ifndef __BLEADVERTISING_H__
#define __BLEADVERTISING_H__
#include "../DhGlobalHead.h"

typedef struct
{
	u1		m_u1AddrType;				/* 0-public  1-random */
	u1		m_pu1Addr[BLE_ADDR_LEN];	/* 设备地址,LSB     */
}BlkBleAddrInfo;



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


#endif /* __BLEADVERTISING_H__ */
