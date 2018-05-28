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

#ifndef __BLEATT_H__
#define __BLEATT_H__
#include "../DhGlobalHead.h"
typedef struct
{
	u1	m_u1UuidType;       /* 当前只支持16/128bit长度的UUID */
	u1	*m_pu1Uuid;		/* 指向实际存放uuid的buff，UUID 按LSB格式存放 */
}BlkAttributeType;

typedef struct
{
	u2	m_u2MaxSize;				/* 属性的大小*/
	u2	m_u2CurrentLen;			/* 属性值的当前长度*/
	u1	*m_pu1AttValue;			/* 具体属性值*/
}BlkAttributeValue;

typedef struct 
{
	BlkAttributeType	m_blkAttType;
	BlkAttributeValue	m_blkAttValue;
	u2					m_u2AttPermission;		/* 许可条件，上层自定义实现*/
	u2                  m_u2CCCDHandle;
}BlkBleAttribute;



#define ATT_OPERATE_READ                        0x01
#define ATT_OPERATE_WRITE                       0x02

#define ATT_PERMISSION_READ						0x01	/* 允许读 */
#define ATT_PERMISSION_WRITE					0x02	/* 允许写 */
#define ATT_PERMISSION_READ_AUTHENTICATION		0x04	/* 读需要链路认证加密过 */
#define ATT_PERMISSION_WRITE_AUTHENTICATION		0x08	/* 写需要链路认证加密过 */
//#define ATT_PERMISSION_READ_AUTHORIZATION		0x10	/* 读需要需要授权,暂不实现 */
//#define ATT_PERMISSION_WRITE_AUTHORIZATION	0x20	/* 写需要需要授权,暂不实现 */
#define ATT_PERMISSION_READ_ENCRYPTION			0x40	/* 读需要链路加密--仅加密就可以 */
#define ATT_PERMISSION_WRITE_ENCRYPTION			0x80	/* 写需要链路加密--仅加密就可以 */

#define ATT_PROPERTIES_BROADCAST				0x01
#define ATT_PROPERTIES_READ						0x02
#define ATT_PROPERTIES_WRITE_WITHOUT_RSP		0x04
#define ATT_PROPERTIES_WRITE					0x08
#define ATT_PROPERTIES_NOTIFY					0x10
#define ATT_PROPERTIES_INDICATE					0x20
#define ATT_PROPERTIES_AUTH_SIGN_WRITE			0x40
#define ATT_PROPERTIES_EXTEND_PEOPERTIES		0x80

#define UUID_TYPE_16BIT				            2
#define UUID_TYPE_128BIT			            16

#define BLE_ATT_INVALID_HANDLE					0x0000

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


extern u4 BleAttReqHandle( u1 *pu1Data, u2 len );
extern u4 BleAttSendIndication(u2 u2AttHandle, u1 *pu1AttValue, u2 len);
extern u4 BleAttSendNotify(u2 u2AttHandle, u1 *pu1AttValue, u2 len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLEATT_H__ */
