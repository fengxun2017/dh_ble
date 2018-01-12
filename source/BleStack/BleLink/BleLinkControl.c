/**
 * @file				BleLinkControl.c
 * @brief			ble链路控制相关过程
 * @author			fengxun
 * @date			2017年12月29日
*/
#include "../../include/DhGlobalHead.h"

#define LL_CONNECTION_UPDATE_REQ		0x00
#define LL_CHANNEL_MAP_REQ				0x01
#define LL_TERMINATE_IND				0x02
#define LL_ENC_REQ						0x03
#define LL_ENC_RSP						0x04
#define LL_START_ENC_REQ				0x05
#define LL_START_ENC_RSP				0x06
#define LL_UNKNOWN_RSP					0x07

#define LL_FEATURE_REQ					0x08
#define LL_FEATURE_RSP					0x09
	#define LL_FEATURE_SET_SIZE					0x08
	
#define LL_PAUSE_ENC_REQ				0x0A
#define LL_PAUSE_ENC_RSP				0x0B
#define LL_VERSION_IND					0x0C
	#define LL_VERSION_SIZE						0x05
#define LL_REJECT_IND					0x0D

#define FEATURE_SUPPORT_LE_ENCY         (1<<0x00)

#define LL_VERSION_COMPID				(0xFFFF)
#define LL_VERSION_VERSNR				(BLE_VERSION_NUMBER)
#define LL_VERSION_SUBVERSNR			(0x0001)

#define TIMEOUT_PRT						(40000)		// 40s procedure response timeout

static u4 LinkFeatureReqHandle(u1	*pu1FearureSet, u2 len)
{
	BlkHostToLinkData rspData;
	
	if( NULL==pu1FearureSet || LL_FEATURE_SET_SIZE!=len )
	{
		return BLE_LINK_INVALID_PARAM;
	}

	// 目前只支持4.0的加密而已,不用管对方的feature了
	memset(rspData.m_pu1HostData, 0x00, BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH);
	rspData.m_pu1HostData[0] = LL_FEATURE_RSP;
	rspData.m_pu1HostData[1] |=  FEATURE_SUPPORT_LE_ENCY;
	rspData.m_u1Length = LL_FEATURE_SET_SIZE + 1;	//opcode
	// 链路控制目前也用这个接口好了
	BleHostDataToLinkPush(rspData);
}

static u4 LinkVersionIndHandle(u1 *peerVersion, u2 len)
{
	BlkHostToLinkData rspData;
	u2	rspLen = 0;
	
	if( NULL==peerVersion || LL_VERSION_SIZE!=len )
	{
		return BLE_LINK_INVALID_PARAM;
	}

	// 目前只支持4.0,不用看对方version了
	memset(rspData.m_pu1HostData, 0x00, BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH);
	rspData.m_pu1HostData[rspLen++] = LL_VERSION_IND;
	rspData.m_pu1HostData[rspLen++] = LL_VERSION_VERSNR;
	rspData.m_pu1HostData[rspLen++] = LL_VERSION_COMPID&0xff;
	rspData.m_pu1HostData[rspLen++] = LL_VERSION_COMPID>>8;
	rspData.m_pu1HostData[rspLen++] = LL_VERSION_SUBVERSNR&0xff;
	rspData.m_pu1HostData[rspLen++] = LL_VERSION_SUBVERSNR>>8;
	
	rspData.m_u1Length = rspLen;	//opcode
	// 链路控制目前也用这个接口好了
	BleHostDataToLinkPush(rspData);
}

void LinkRspUnknown()
{
	BlkHostToLinkData rspData;
	u2	rspLen = 0;
	
	memset(rspData.m_pu1HostData, 0x00, BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH);
	rspData.m_pu1HostData[rspLen++] = LL_UNKNOWN_RSP;
	
	rspData.m_u1Length = rspLen;	//opcode
	// 链路控制目前也用这个接口好了
	BleHostDataToLinkPush(rspData);
}


u4 CheckLinkChannelMapUpdate(u1 *pu1PDU, u1 *pu1NewChannelMap, u2 *u2Instant)
{
    u1  llid;
    u1  opcode;
    u2  len;
    u1  index = 0;

    if( NULL==pu1PDU || NULL==pu1NewChannelMap )
    {
        return ERR_LINK_INVALID_PARAMS;
    }
    
    llid = pu1PDU[index++]&0x03;
    len = pu1PDU[index++]&0x1f;
    if ( LLID_CONTROL==llid && len>0 )
    {
        opcode = pu1PDU[index++];
        if( LL_CHANNEL_MAP_REQ == opcode )
        {
            memcpy(pu1NewChannelMap, pu1PDU+index, BLE_CHANNEL_MAP_LEN)；
            index += BLE_CHANNEL_MAP_LEN;
            *u2Instant = pu1PDU[index++];
            *u2Instant += (pu1PDU[index++]<<8)&0xff00;

            return DH_SUCCESS;
        }
    }

    return ERR_LINK_NOT_CHANNEL_MAP_REQ;
}
u4 BleLinkControlHandle(u1 *pu1Data, u2 len)
{
	u1	opcode;

	opcode = pu1Data[0x00];

	switch(opcode)
	{
		case LL_FEATURE_REQ:
			LinkFeatureReqHandle(pu1Data+0x01, len-1);
		break;
		
		case LL_CHANNEL_MAP_REQ:
		break;

		case LL_TERMINATE_IND:
		break;
		
		case LL_ENC_REQ:
		break;
		
		case LL_START_ENC_REQ:
		break;
		
		case LL_START_ENC_RSP:
		break;
		
		case LL_PAUSE_ENC_REQ:
		break;
		
		case LL_VERSION_IND:
			LinkVersionIndHandle(pu1Data+0x01, len-1);
		break;
		
		case LL_REJECT_IND:
		break;

		default:
			LinkRspUnknown();
		break;
	}

	return DH_SUCCESS;
}
