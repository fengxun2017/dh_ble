/**
 * @file				BleAtt.c
 * @brief			ble 属性协议
 * @author			fengxun
 * @date			2018年1月2日
*/
#include "../../include/DhGlobalHead.h"

/*
	Requests—PDUs sent to a server by a client, and invoke responses
		If a server receives a request that it does not support, then the server shall
		respond with the Error Response with the Error Code «Request Not Supported», with the Attribute Handle In Error set to 0x0000.

		If the server receives an invalid request–for example, the PDU is the wrong
		length–then the server shall respond with the Error Response with the Error
		Code «Invalid PDU», with the Attribute Handle In Error set to 0x0000.

		If a server does not have sufficient resources to process a request, then the
		server shall respond with the Error Response with the Error Code «Insufficient
		Resources», with the Attribute Handle In Error set to 0x0000.

		If a server cannot process a request because an error was encountered during
		the processing of this request, then the server shall respond with the Error
		Response with the Error Code «Unlikely Error», with the Attribute Handle In
		Error set to 0x0000.
	Commands—PDUs sent to a server by a client
		If a server receives a command that it does not support, indicated by the Command
		Flag of the PDU set to one, then the server shall ignore the Command.
	

*/
#define ATT_ERR_INVALID_HANDLE						0x01	
#define ATT_ERR_READ_NOT_PERMITTED					0x02
#define ATT_ERR_WRITE_NOT_PERMITTED					0x03
#define ATT_ERR_INVALID_PDU							0x04
#define ATT_ERR_INSUFFICIENT_AUTHENTICATION			0x05
#define ATT_ERR_REQUEST_NOT_SUPPORTED				0x06
#define ATT_ERR_INVALID_OFFSET						0x07
#define ATT_ERR_INSUFFICIENT_AUTHORIZATION			0x08
#define ATT_ERR_PREPARE_QUEUE_FULL					0x09
#define ATT_ERR_ATTRIBUTE_NOT_FOUND					0x0A
#define ATT_ERR_ATTRIBUTE_NOT_LONG					0x0B
#define ATT_ERR_INSUFFICIENT_ENCRYPTION_KEYSIZE		0x0C
#define ATT_ERR_INVALID_ATTRIBUTE_VALUE_LEN			0x0D
#define ATT_ERR_UNLIKELY_ERROR						0x0E
#define ATT_ERR_INSUFFICIENT_ENCRYPTION				0x0F
#define ATT_ERR_UNSUPPORTED_GROUP_TYPE				0x10
#define ATT_ERR_INSUFFICIENT_RESOURCES				0x11

#define ATT_OPCODE_ERROR						0x01
#define ATT_OPCODE_MTU_EXCHANGE_REQ				0x02
#define ATT_OPCODE_MTU_EXCHANGE_RSP				0x03
#define ATT_OPCODE_FIND_INFO_REQ				0x04
#define ATT_OPCODE_FIND_INFO_RSP				0x05
#define ATT_OPCODE_FIND_BY_TYPE_VALUE_REQ		0x06
#define ATT_OPCODE_FIND_BY_TYPE_VALUE_RSP		0x07
#define ATT_OPCODE_READ_BY_TYPE_REQ				0x08
#define ATT_OPCODE_READ_BY_TYPE_RSP				0x09
#define ATT_OPCODE_READ_REQ						0x0A
#define ATT_OPCODE_READ_RSP						0x0B
#define ATT_OPCODE_READ_BLOB_REQ				0x0C
#define ATT_OPCODE_READ_BLOB_RSP				0x0D
#define ATT_OPCODE_READ_MULTIPLE_REQ			0x0E
#define ATT_OPCODE_READ_MULTIPLE_RSP			0x0F
#define ATT_OPCODE_READ_BY_GROUP_TYPE_REQ		0x10
#define ATT_OPCODE_READ_BY_GROUP_TYPE_RSP		0x11
#define ATT_OPCODE_WRITE_REQ					0x12
#define ATT_OPCODE_WRITE_RSP					0x13
#define ATT_OPCODE_WRITE_CMD					0x52
#define ATT_OPCODE_SIGNED_WRITE_CMD				0xD2
#define ATT_OPCODE_PREPARE_WRITE_REQ			0x16
#define ATT_OPCODE_PREPARE_WRITE_RSP			0x17
#define ATT_OPCODE_EXECUTE_WRITE_REQ			0x18
#define ATT_OPCODE_EXECUTE_WRITE_RSP			0x19
#define ATT_OPCODE_NOTIFY						0x1B
#define ATT_OPCODE_INDICATION					0x1D
#define ATT_OPCODE_CONFIRMATION					0x1E

#define ATT_GROUP_TYPE_PRIMARY_SERVICE			0x2800
#define ATT_GROUP_TYPE_SECOND_SERVICE			0x2801

#define ATT_INVALID_HANDLE						0x0000
#define UUID_FORMAT_16BIT						0x01
#define UUID_FORMAT_128BIT						0x02

#define ATT_ERR_RSP_LEN							0x05
#define ATT_EXCHANGE_MTU_RSP_LEN				0x03

#define ATT_FIND_INFO_REQ_LEN					(4)		/* 去除opcode后的长度 */
#define ATT_READ_GROUP_TYPE_REQ_LEN1			(6)
#define ATT_READ_GROUP_TYPE_REQ_LEN2			(20)

static u1 CheckGroupType(u1 *pu1Uuid, u1 uuidType)
{
	u1	pu1PrimarySrvUuid[] = {0x00,0x28};
	u1	pu1SecondSSrvUuid[] = {0x01,0x28};

	if( UUID_TYPE_16BIT != uuidType )
	{
		return 0;
	}
	if( memcmp(pu1Uuid, pu1PrimarySrvUuid, uuidType)!=0 && memcmp(pu1Uuid, pu1SecondSSrvUuid, uuidType)!=0 )
	{
		return 0;
	}

	return 1;
}
static u1 GetUuidFormat(u1 uuidType)
{
	if( UUID_TYPE_128BIT == uuidType )
	{
		return UUID_FORMAT_128BIT;
	}
	return UUID_FORMAT_16BIT;
}

/**
 *@brief: 		AttErrRsp
 *@details:		错误响应
 *@param[in]	reqOpcode		导致错误的请求
 *@param[in]	attHandle		导致错误的att句柄 
 *@param[in]	errCode			错误代码
 
 *@retval:		封包后的长度
 */
__INLINE static u4 AttErrRsp(u1 reqOpcode, u2 attHandle, u1 errCode)
{
	u1	pu1Rsp[ATT_ERR_RSP_LEN];
	u1 index = 0;

	pu1Rsp[index++] = ATT_OPCODE_ERROR;
	pu1Rsp[index++] = reqOpcode;
	pu1Rsp[index++] = attHandle&0xff;
	pu1Rsp[index++] = attHandle>>8;
	pu1Rsp[index++] = errCode;

	if( BleL2capDataSend(BLE_L2CAP_ATT_CHANNEL_ID, pu1Rsp, index) != DH_SUCCESS )
	{
		return ERR_ATT_SEND_RSP_FAILED;
	}

	return DH_SUCCESS;
}

__INLINE static u4 AttMtuRsp(u2 MTU)
{
	u1 index = 0;
	u1 pu1Rsp[ATT_EXCHANGE_MTU_RSP_LEN];
	
	pu1Rsp[index++] = ATT_OPCODE_MTU_EXCHANGE_RSP;
	pu1Rsp[index++] = MTU&0xff;
	pu1Rsp[index++] = MTU>>8;
	
	if( BleL2capDataSend(BLE_L2CAP_ATT_CHANNEL_ID, pu1Rsp, index) != DH_SUCCESS )
	{
		return ERR_ATT_SEND_RSP_FAILED;
	}
	return DH_SUCCESS;
}

__INLINE static u4 AttReadByGroupTypeRsp(u1 *pu1AttList, u1 len)
{
	u1	index = 0;
	u1	pu1Rsp[BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH-BLE_L2CAP_HEADER_LEN];
	
	pu1Rsp[index++] = ATT_OPCODE_READ_BY_GROUP_TYPE_RSP;
	pu1Rsp[index++] = len;
	memcpy(pu1Rsp+index, pu1AttList, len);
	index += len;

	if( BleL2capDataSend(BLE_L2CAP_ATT_CHANNEL_ID, pu1Rsp, index) != DH_SUCCESS )
	{
		return ERR_ATT_SEND_RSP_FAILED;
	}
	return DH_SUCCESS;
}

__INLINE static u4 AttReadByTypeRsp(u1 *pu1AttList, u1 len)
{
	u1	index = 0;
	u1	pu1Rsp[BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH-BLE_L2CAP_HEADER_LEN];
	
	pu1Rsp[index++] = ATT_OPCODE_READ_BY_TYPE_RSP;
	pu1Rsp[index++] = len;
	memcpy(pu1Rsp+index, pu1AttList, len);
	index += len;
	
	if( BleL2capDataSend(BLE_L2CAP_ATT_CHANNEL_ID, pu1Rsp, index) != DH_SUCCESS )
	{
		return ERR_ATT_SEND_RSP_FAILED;
	}
	return DH_SUCCESS;
}

__INLINE static u4 AttFindInfoRsp(u1 format, u1 *pu1InfoRsp, u1 len)
{
	u1 index = 0;
	u1	pu1Rsp[BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH-BLE_L2CAP_HEADER_LEN];
	
	pu1Rsp[index++] = ATT_OPCODE_FIND_INFO_RSP;
	pu1Rsp[index++] = format;
	memcpy(pu1Rsp+index, pu1InfoRsp, len);
	index += len;
	
	if( BleL2capDataSend(BLE_L2CAP_ATT_CHANNEL_ID, pu1Rsp, index) != DH_SUCCESS )
	{
		return ERR_ATT_SEND_RSP_FAILED;
	}	return index;
}


static u4 AttFindInfoReqHandle(u1  *pu1Req, u1 len)
{
	BlkBleAttribute *pblkAtt;
	u2	u2StartHandle;
	u2	u2EndHandle;
	u2	u2AttHandle;
	u1	index = 0;
	u4	ret;
	u1	uuidFormat;
	u1	pu1Rsp[BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH-BLE_L2CAP_HEADER_LEN];
	u1	rspLen = 0;
	
	u2StartHandle = pu1Req[index++];
	u2StartHandle += (pu1Req[index++]<<8)&0xff00;
	u2EndHandle = pu1Req[index++];
	u2EndHandle += (pu1Req[index++]<<8)&0xff00;
	if ( ATT_FIND_INFO_REQ_LEN != len )
	{
		ret = AttErrRsp(ATT_OPCODE_FIND_INFO_REQ, u2StartHandle, ATT_ERR_INVALID_PDU);
		return ret;
	}

	if( ATT_INVALID_HANDLE==u2StartHandle || u2StartHandle>u2EndHandle )
	{
		ret = AttErrRsp(ATT_OPCODE_FIND_INFO_REQ, u2StartHandle,ATT_ERR_INVALID_HANDLE);
		return ret;
	}
	
	for( u2AttHandle=u2StartHandle; u2AttHandle <= u2EndHandle; u2AttHandle++ )
	{
		BleGattFindAttByHandle(u2AttHandle, &pblkAtt);
		if ( NULL != pblkAtt )
		{
			if( u2AttHandle == u2StartHandle )
			{
				uuidFormat = GetUuidFormat(pblkAtt->attType.uuidType);
			}
			else
			{	
				/* find information 的响应中只能放同一类型的uuid */
				if( GetUuidFormat(pblkAtt->attType.uuidType) != uuidFormat )
				{
					return AttFindInfoRsp(uuidFormat, pu1Rsp, rspLen);					
				}
			}
			pu1Rsp[rspLen++] = u2AttHandle&0xff;
			pu1Rsp[rspLen++] = (u2AttHandle>>8)&0xff;
			memcpy(pu1Rsp+rspLen, pblkAtt->attType.pu1Uuid; pblkAtt->attType.uuidType);
			/* uuid的类型实际值就是UUID的长度 */
			rspLen += pblkAtt->attType.uuidType;
			
		}
		else	/* pblk==NULL */
		{
			if ( u2AttHandle == u2StartHandle )
			{
				return AttErrRsp(ATT_OPCODE_FIND_INFO_REQ, u2AttHandle, ATT_ERR_ATTRIBUTE_NOT_FOUND);
			}
			return AttFindInfoRsp(uuidFormat, pu1Rsp, rspLen);					
		}
	}

	return DH_SUCCESS;
}

static u4 AttReadGroupTypeReqHandle(u1 *pu1Req, u1 len)
{
	BlkBleAttribute *pblkAtt;
	u1	index = 0;
	u1	rspLen = 0;
	u2	u2StartHandle;
	u2	u2EndHandle;
	u2	u2AttHandle;
	u1	uuidType;
	u1	pu1GroupType[UUID_TYPE_128BIT];

	u2StartHandle = pu1Req[index++];
	u2StartHandle += (pu1Req[index++]<<8)&0xff;
	u2EndHandle = pu1Req[index++];
	u2EndHandle += (pu1Req[index++]<<8)&0xff;

	if( ATT_READ_GROUP_TYPE_REQ_LEN1!=len && ATT_READ_GROUP_TYPE_REQ_LEN2!=len )
	{
		AttErrRsp(ATT_OPCODE_READ_BY_GROUP_TYPE_REQ, );
	}
	if( ATT_READ_GROUP_TYPE_REQ_LEN1 == len )
	{
		uuidType = UUID_TYPE_16BIT;
	}
	else
	{
		uuidType = UUID_TYPE_128BIT;
	}
	memcpy(pu1GroupType, pu1Req+index, uuidType);
	if( CheckGroupType(pu1GroupType, uuidType) != 0 )
	{
		AttErrRsp(ATT_OPCODE_READ_BY_GROUP_TYPE_REQ, u2StartHandle, ATT_ERR_UNSUPPORTED_GROUP_TYPE);
	}
	
	
}
/**
 *@brief: 		BleAttHandle
 *@details:		ATT协议数据处理，目前不支持带认证签名
 *@param[in]	pu1Data		协议数据  
 *@param[in]	len			数据长度
 *@retval:		DH_SUCCESS
 */
u4 BleAttHandle(u1 *pu1Data, u2 len)
{
	u1	opcode;

	opcode = pu1Data[0];
}
