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
#define ATT_INVALID_HANDLE						0x01	
#define ATT_READ_NOT_PERMITTED					0x02
#define ATT_WRITE_NOT_PERMITTED					0x03
#define ATT_INVALID_PDU							0x04
#define ATT_INSUFFICIENT_AUTHENTICATION			0x05
#define ATT_REQUEST_NOT_SUPPORTED				0x06
#define ATT_INVALID_OFFSET						0x07
#define ATT_INSUFFICIENT_AUTHORIZATION			0x08
#define ATT_PREPARE_QUEUE_FULL					0x09
#define ATT_ATTRIBUTE_NOT_FOUND					0x0A
#define ATT_ATTRIBUTE_NOT_LONG					0x0B
#define ATT_INSUFFICIENT_ENCRYPTION_KEYSIZE		0x0C
#define ATT_INVALID_ATTRIBUTE_VALUE_LEN			0x0D
#define ATT_UNLIKELY_ERROR						0x0E
#define ATT_INSUFFICIENT_ENCRYPTION				0x0F
#define ATT_UNSUPPORTED_GROUP_TYPE				0x10
#define ATT_INSUFFICIENT_RESOURCES				0x11

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

#define ATT_ERR_RSP_LEN							0x05
#define ATT_EXCHANGE_MTU_RSP_LEN				0x03

#define ATT_FIND_INFO_REQ_LEN					0x04		/* 去除opcode后的长度 */


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

__INLINE static u4 ReadByGroupTypeRsp(u1 *pu1AttList, u1 len)
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

__INLINE static u4 ReadByTypeRsp(u1 *pu1AttList, u1 len)
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

__INLINE static u4 FindInfoRsp(u1 format, u1 *pu1InfoRsp, u1 len)
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
	BlkAttributeValue *pblkAtt;
	u4	ret;
	if ( ATT_FIND_INFO_REQ_LEN != len )
	{
		return ERR_ATT_INVALID_PDU;
	}
	pblkAtt = BleGattFindAttByHandle()
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
