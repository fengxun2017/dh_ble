/**
 * @file				BleAtt.h
 * @brief			BleAtt.c 头文件
 * @author			fengxun
 * @date			2018年1月2日
*/
#ifndef __BLEATT_H__
#define __BLEATT_H__
#include "../DhGlobalHead.h"
typedef struct
{
	u1	uuidType;
	u1	*pu1Uuid;
}BlkAttributeType;

typedef struct
{
	u2	u2AttSize;				/* 属性的大小*/
	u2	u2CurrentLen;			/* 属性值的当前长度*/
	u1	*pu1AttValue;			/* 具体属性值*/
}BlkAttributeValue;

typedef struct 
{
	BlkAttributeType	attType;
	BlkAttributeValue	attValue;
	u2					attPermission;		/* 许可条件，上层自定义实现*/
}BlkBleAttribute;

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



#define ATT_PERMISSION_READ						0x01	/* 允许读 */
#define ATT_PERMISSION_WRITE					0x02	/* 允许写 */
#define ATT_PERMISIION_READ_AUTHENTICATION		0x04	/* 读需要链路认证过 */
#define ATT_PERMISIION_WRITE_AUTHENTICATION		0x08	/* 写需要链路认证过 */
#define ATT_PERMISSION_READ_AUTHORIZATION		0x10	/* 读需要需要授权 */
#define ATT_PERMISSION_WRITE_AUTHORIZATION		0x20	/* 写需要需要授权 */
#define ATT_PERMISSION_READ_ENCRYPTION			0x40	/* 读需要链路加密 */
#define ATT_PERMISSION_WRITE_ENCRYPTION			0x80	/* 写需要链路加密 */

#define ATT_PROPERTIES_BROADCAST				0x01
#define ATT_PROPERTIES_READ						0x02
#define ATT_PROPERTIES_WRITE_WITHOUT_RSP		0x04
#define ATT_PROPERTIES_WRITE					0x08
#define ATT_PROPERTIES_NOTIFY					0x10
#define ATT_PROPERTIES_INDICATE					0x20
#define ATT_PROPERTIES_AUTH_SIGN_WRITE			0x40
#define ATT_PROPERTIES_EXTEND_PEOPERTIES		0x80

#define UUID_TYPE_16BIT				2
#define UUID_TYPE_128BIT			16

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern u4 BleAttHandle(u1 *pu1Data, u2 len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLEATT_H__ */
