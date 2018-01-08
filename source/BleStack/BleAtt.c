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

#define ATT_ERR_RSP_SIZE						0x05
#define ATT_EXCHANGE_MTU_RSP					0x03
static u4 BleAttErrRsp(u1 reqOpcode, u2 attHandle, u1 errCode)
{
	u1 pu1Rsp[ATT_ERR_RSP_SIZE];
	u1 index;

	pu1Rsp[index++] = ATT_OPCODE_ERROR;
	pu1Rsp[index++] = reqOpcode;
	pu1Rsp[index++] = attHandle&0xff;
	pu1Rsp[index++] = attHandle>>8;
	pu1Rsp[index++] = errCode;

	if( BleL2capDataSend(BLE_L2CAP_ATT_CHANNEL_ID, pu1Rsp, index) != DH_SUCCESS )
	{
		DEBUG_INFO("send att err rsp err!!");
		return ERR_ATT_SEND_RSP_FAILED;
	}

	return DH_SUCCESS;
}
static u4 BleAttMtuRsp(u2 MTU)
{
	u1 pu1Rsp[ATT_EXCHANGE_MTU_RSP];
	u1 index;

	pu1Rsp[index++] = ATT_OPCODE_MTU_EXCHANGE_RSP;
	pu1Rsp[index++] = MTU&0xff;
	pu1Rsp[index++] = MTU>>8;

	if( BleL2capDataSend(BLE_L2CAP_ATT_CHANNEL_ID, pu1Rsp, index) != DH_SUCCESS )
	{
		DEBUG_INFO("send mtu rsp err!!");
		return ERR_ATT_SEND_RSP_FAILED;
	}

	return DH_SUCCESS;
}
u4 BleAttHandle(u1 *pu1Data, u2 len)
{
	
}
