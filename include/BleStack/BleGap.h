/**
 * @file				BleGap.h
 * @brief			BleGap.c µÄÍ·ÎÄ¼þ
 * @author			fengxun
 * @date			2018å¹´1æœˆ30æ—¥
*/
#ifndef __BLEGAP_H__
#define __BLEGAP_H__
#include "../DhGlobalHead.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern u4 BleGapAddrCfg(BlkBleAddrInfo addr);
extern u4 BleGapDeviceNameCfg(char *pu1Name, u1 len);
extern u4 BleGapDeviceNameGet(char *pu1Name, u1 *pu1BuffSize);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLEGAP_H__ */
