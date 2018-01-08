/**
 * @file				BleL2cap.h
 * @brief			BleL2cap.c ��ͷ�ļ�
 * @author			fengxun
 * @date			2017年12月28日
*/
#ifndef __BLEL2CAP_H__
#define __BLEL2CAP_H__
#include "../DhGlobalHead.h"

/* BLE只支持3个通道 */
#define BLE_L2CAP_ATT_CHANNEL_ID			0x0004
#define BLE_L2CAP_SM_CHANNEL_ID				0x0006
#define BLE_L2CAP_SIGNAL_CHANNEL_ID			0x0005

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern u4 BleL2capHandle(u1 *pu1Data, u2 u2length);
extern u4 BleL2capDataSend(u2 u2ChannelId,  u1 *pu1Data, u2 len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLEL2CAP_H__ */
