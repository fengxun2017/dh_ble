/**
 * @file				BleL2cap.c
 * @brief			ble l2cap层
 * @author			fengxun
 * @date			2017年12月28日
*/
#include "../../include/DhGlobalHead.h"

// BLE只支持3个通道

#define BLE_ATT_CHANNEL_ID			0x0004
#define BLE_SM_CHANNEL_ID			0x0006
#define BLE_SIGNAL_CHANNEL_ID		0x0005

#define L2CAP_HEADER_LEN			0x04

u4 BleL2capHandle(u1 *pu1Data, u2 u2length)
{
	u2 u2PayloadLen,u2ChannelId;

	if( NULL==pu1Data )
	{
		return ERR_L2CAP_INVALID_PARAM;
	}
	u2PayloadLen = pu1Data[0]+(pu1Data[1]<<8)&0xFF00;
	if( (u2PayloadLen+0x04) != u2length )
	{
		return ERR_L2CAP_INVALID_LENGTH;
	}
	
	u2ChannelId = pu1Data[2]+ (pu1Data[3]<<8)&0xFF00;
	switch(u2ChannelId)
	{
		case BLE_ATT_CHANNEL_ID:
			BleAttHandle(pu1Data+L2CAP_HEADER_LEN, u2PayloadLen);
		break;

		case BLE_SM_CHANNEL_ID:
		break;

		case BLE_SIGNAL_CHANNEL_ID:
		break;


		default:
		break;
	}
	
	
	return DH_SUCCESS;
}
