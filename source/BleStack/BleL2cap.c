/*
* (The MIT License)

* Copyright (c) 2018 Feng Xun

* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* 'Software'), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:

* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include "../../include/DhGlobalHead.h"


/**
 *@brief: 		BleL2capPackage
 *@details:		l2cap数据封包并发送
 *@param[in]	u2ChannelId		l2cap channel id
 *@param[in]	pu1Data			待封包数据
 *@param[in]	len				待封包数据长度

 *@retval:		DH_SUCCESS
 */

u4 BleL2capDataSend(u2 u2ChannelId,  u1 *pu1Data, u2 len)
{
	BlkHostToLinkData hostData;
	u2	index = 0;
	
	if ( NULL == pu1Data )
	{
		return ERR_L2CAP_INVALID_PARAM;		
	}
	if ( (len+BLE_L2CAP_HEADER_LEN) > (BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH) )
	{
		return ERR_L2CAP_INVALID_LENGTH;
	}

	hostData.m_pu1HostData[index++] = len&0xff;
	hostData.m_pu1HostData[index++] = len>>8;
	hostData.m_pu1HostData[index++] = u2ChannelId&0xff;
	hostData.m_pu1HostData[index++] = u2ChannelId>>8;
	memcpy(hostData.m_pu1HostData+index, pu1Data, len);
    hostData.m_u1Length = index+len;
    hostData.m_u1PacketFlag = DATA_PACKET;
    
	if(BleHostDataToLinkPush(hostData) != DH_SUCCESS )
	{
		return ERR_L2CAP_INSUFFICIENT_RESOURCE;
	}
	
	return DH_SUCCESS;
}

/**
 *@brief: 		BleL2capHandle
 *@details:		处理L2cap层的数据包
 *@param[in]	pu1Data		L2cap层数据包
 *@param[in]	u2length	数据长度

 *@retval:		DH_SUCCESS
 */

u4 BleL2capHandle(u1 *pu1Data, u2 u2length)
{
	u2 u2PayloadLen,u2ChannelId;

	if( NULL==pu1Data )
	{
		return ERR_L2CAP_INVALID_PARAM;
	}
	u2PayloadLen = pu1Data[0]+((pu1Data[1]<<8)&0xFF00);
	if( (u2PayloadLen+0x04) != u2length )
	{
		return ERR_L2CAP_INVALID_LENGTH;
	}
	
	u2ChannelId = pu1Data[2]+ ((pu1Data[3]<<8)&0xFF00);
	switch(u2ChannelId)
	{
		case BLE_L2CAP_ATT_CHANNEL_ID:
			BleAttReqHandle(pu1Data+BLE_L2CAP_HEADER_LEN, u2PayloadLen);
		break;

		case BLE_L2CAP_SM_CHANNEL_ID:
		break;

		case BLE_L2CAP_SIGNAL_CHANNEL_ID:
		break;


		default:
		break;
	}
	
	
	return DH_SUCCESS;
}
