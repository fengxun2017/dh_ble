/**
 * @file				BleLinkControl.c
 * @brief			ble链路控制相关过程
 * @author			fengxun
 * @date			2017年12月29日
*/
#include "../../include/DhGlobalHead.h"

#define BLE_LINK_CONTROL_DEBUG

#if !defined(BLE_LINK_CONTROL_DEBUG)
#undef DEBUG_INFO
#define DEBUG_INFO(...)
#undef DEBUG_DATA
#define DEBUG_DATA(...)
#endif

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
	#define LL_FEATURE_SET_SIZE			0x08
	
#define LL_PAUSE_ENC_REQ				0x0A
#define LL_PAUSE_ENC_RSP				0x0B
#define LL_VERSION_IND					0x0C
	#define LL_VERSION_SIZE				0x05
#define LL_REJECT_IND					0x0D

#define FEATURE_SUPPORT_LE_ENCY         (1<<0x00)

#define LL_VERSION_COMPID				(0xFFFF)
#define LL_VERSION_VERSNR				(BLE_VERSION_NUMBER)
#define LL_VERSION_SUBVERSNR			(0x0022)

#define TIMEOUT_PRT						(40000)		// 40s procedure response timeout

static u4 LinkFeatureReqHandle(u1	*pu1FearureSet, u2 len)
{
	BlkHostToLinkData rspData;
	
	if( NULL==pu1FearureSet || LL_FEATURE_SET_SIZE!=len )
	{
		return ERR_LINK_INVALID_PARAMS;
	}

	// 目前只支持4.0的加密而已,不用管对方的feature了
	memset(rspData.m_pu1HostData, 0x00, BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH);
	rspData.m_pu1HostData[0] = LL_FEATURE_RSP;
	rspData.m_pu1HostData[1] |=  FEATURE_SUPPORT_LE_ENCY;
	rspData.m_u1Length = LL_FEATURE_SET_SIZE + 1;	//opcode
	// 链路控制目前也用这个接口好了

	rspData.m_u1PacketFlag = CONTROL_PACKET;
	BleHostDataToLinkPush(rspData);
	
	return DH_SUCCESS;
}

static u4 LinkVersionIndHandle(u1 *peerVersion, u2 len)
{
	BlkHostToLinkData rspData;
	u2	rspLen = 0;
	
	if( NULL==peerVersion || LL_VERSION_SIZE!=len )
	{
		return ERR_LINK_INVALID_PARAMS;
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
	rspData.m_u1PacketFlag = CONTROL_PACKET;
	// 链路控制目前也用这个接口好了
	BleHostDataToLinkPush(rspData);
	return DH_SUCCESS;
}

void LinkRspUnknown(u1 opcode)
{
	BlkHostToLinkData rspData;
	u2	rspLen = 0;
	
	memset(rspData.m_pu1HostData, 0x00, BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH);
	rspData.m_pu1HostData[rspLen++] = LL_UNKNOWN_RSP;
	rspData.m_pu1HostData[rspLen++] = opcode;
	
	rspData.m_u1Length = rspLen;	//opcode
	rspData.m_u1PacketFlag = CONTROL_PACKET;
	// 链路控制目前也用这个接口好了
	BleHostDataToLinkPush(rspData);
}

/**
 *@brief: 		CheckLinkChannelMapUpdateReq
 *@details:		检查链路控制请求是否是channel map update
 *@param[in]	pu1PDU              链路PDU
 *@param[out]	pu1NewChannelMap    返回新的通道映射
 *@param[out]	u2Instant           返回通道改变的时间点

 *@note 通道更新请求需要立刻处理，虽然规范要求了对方必须需要给的缓冲时间需要多于6个间隔，
        但是链路数据都是通过下半部处理的，延迟不确定。所以不放在 BleLinkControlHandle 函数中处理
        直接在链路数据接收中断里处理
 *@retval:		DH_SUCCESS
 */
u4 CheckLinkChannelMapUpdateReq(u1 *pu1PDU, u1 *pu1NewChannelMap, u2 *pu2Instant)
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
            memcpy(pu1NewChannelMap, pu1PDU+index, BLE_CHANNEL_MAP_LEN);
            index += BLE_CHANNEL_MAP_LEN;
            *pu2Instant = pu1PDU[index++];
            *pu2Instant += ((pu1PDU[index]<<8)&0xFF00);
            DEBUG_INFO("u2 instant:%04x",*pu2Instant);
            return DH_SUCCESS;
        }
    }

    return ERR_LINK_NOT_CHANNEL_MAP_REQ;    // 不是channel map控制请求
}

/**
 *@brief: 		CheckLinkConnUpdateReq
 *@details:		链路连接参数更新，虽然规范要求了对方必须需要给的缓冲时间需要多于6个间隔，
                但是链路数据都是通过下半部处理的，延迟不确定。所以不放在 BleLinkControlHandle 函数中处理
                直接在链路数据接收中断里处理，直接检查是否是链路更新控制，如果是就直接处理
 *@param[in]	pu1PDU          链路PDU        
 *@param[in]	u1WinSize       新窗口大小    
 *@param[in]	u2WinOffset     新窗口偏移
 *@param[in]	u2Interval      新连接间隔
 *@param[in]	u2Latency       新从机延迟
 *@param[in]	u2Timeout       新超时
 *@param[in]	u2Instant       连接参数更新的时间点

 *@retval:		DH_SUCCESS
 */

u4 CheckLinkConnUpdateReq(u1 *pu1PDU, u1 *u1WinSize, u2 *u2WinOffset, u2 *u2Interval, u2 *u2Latency, u2 *u2Timeout, u2 *u2Instant)
{
    u1  llid;
    u1  opcode;
    u2  len;
    u1  index = 0;

    if( NULL==pu1PDU || NULL==u1WinSize || NULL==u2WinOffset || NULL==u2Interval || NULL==u2Latency || NULL==u2Timeout || NULL==u2Instant )
    {
        return ERR_LINK_INVALID_PARAMS;
    }
    
    llid = pu1PDU[index++]&0x03;
    len = pu1PDU[index++]&0x1f;
    if ( LLID_CONTROL==llid && len>0 )
    {
        opcode = pu1PDU[index++];
        if( LL_CONNECTION_UPDATE_REQ == opcode )
        {
            *u1WinSize = pu1PDU[index++];
            *u2WinOffset = pu1PDU[index++]; *u2WinOffset += (((u2)pu1PDU[index++]<<8)&0xFF00);
            *u2Interval = pu1PDU[index++]; *u2Interval += (((u2)pu1PDU[index++]<<8)&0xFF00);
            *u2Latency = pu1PDU[index++]; *u2Latency += (((u2)pu1PDU[index++]<<8)&0xFF00);
            *u2Timeout = pu1PDU[index++]; *u2Timeout += (((u2)pu1PDU[index++]<<8)&0xFF00);
            *u2Instant = pu1PDU[index++]; *u2Instant += (((u2)pu1PDU[index++]<<8)&0xFF00);
            return DH_SUCCESS;
        }
    }

    return ERR_LINK_NOT_CONN_UPDATE_REQ;    // 不是conn update控制请求
}

u4 BleLinkControlHandle(u1 *pu1Data, u2 len)
{
	u1	opcode;

	opcode = pu1Data[0x00];

	switch(opcode)
	{
        case LL_CONNECTION_UPDATE_REQ:
        break;
	
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
			LinkRspUnknown(opcode);
		break;
	}

	return DH_SUCCESS;
}
