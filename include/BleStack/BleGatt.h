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

#ifndef __BLEGATT_H__
#define __BLEGATT_H__
#include "../DhGlobalHead.h"


#define BLE_PRIMARY_SERVICE_UUID				0x2800	
#define BLE_INCLUDE_UUID						0x2802
#define BLE_CHARACTERISTIC_DECLARATION_UUID		0x2803
#define BLE_CCCD_UUID							0x2902
#define BLE_GENERIC_ACCESS_SERVICE_UUID			0x1800
#define BLE_GENERIC_ATTRIBUTE_SERVICE_UUID		0x1801
#define BLE_DEVICE_NAME_UUID					0x2A00
#define BLE_APPEARANCE_UUID						0x2A01
#define BLE_PERIPHERAL_PRIVACY_FLAG_UUID		0x2A02
#define BLE_PPCP_UUID							0x2A04
#define BLE_RECONNECTION_ADDRESS_UUID			0x2A03
#define BLE_SERVICE_CHANGED_UUID				0x2A05

/* 返回特性句柄相关的信息，包括特性值的句柄，描述符的句柄 */
typedef struct 
{
    u2  m_u2ValueHandle;    /* 特性值句柄 */
    u2  m_u2CccdHandle;     /* Client Characteristic Configuration 句柄 */
}BlkBleAttHandleInfo;

typedef struct
{
    u2  m_u2AttHandle;
    u2  m_u2WriteLen;
    u1  m_pu1AttValue[BLE_ATT_MTU_MAX_SIZE];
}BlkBleRecvWriteEvent;


/* 特征值具有的特性 */
typedef struct 
{
    u1  m_u1BroadcastEnable:1;
    u1  m_u1ReadEnable:1;
    u1  m_u1WriteCmdEnable:1;
    u1  m_u1WriteReqEnable:1;
    u1  m_u1NotifyEnable:1;
    u1  m_u1IndicateEnable:1;
    u1  m_u1SignedWriteEnable:1;
    u1  m_u1ExtendedProps:1;
}BlkCharProperties;

/* 特性配置，当前只支持CCCD描述符 */
typedef struct
{
    BlkAttributeType    m_blkUuid;          // 特性值的UUID
    BlkCharProperties   m_BlkCharProps;     // 是否支持读写等特性
    u2  m_u2ValuePermission;                // 特性值的许可条件
    u2  m_u2CCCDPermission;                 // 客户端特性配置描述符的许可条件
}BlkGattCharCfg;

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern u4	BleGattFindAttByHandle(u2 u2Handle, BlkBleAttribute **ppblkAtt);
extern u4 BleGattFindAttByType(u2 u2StartHandle, u2 u2EndHandle, u1 *pu1UUID, u1 UUIDType, BlkBleAttribute **ppblkAtt);
extern u4	BleGattInfoInit(void);
extern u4 BleGattServiceDeclAdd(u1 *pu1ServiceUuid, u1 uuidType);
extern u4 BleGattCharacteristicAdd( BlkGattCharCfg charaCfg, u1 *pu1CharValueBuff, u2 u2BuffSize, BlkBleAttHandleInfo *pu2ValueHandle );
extern u2 BleDeviceNameAttHandleGet(void);
extern u4 BleGattSendNotify(u2 u2AttHandle, u1 *pu1AttValue, u2 len);
extern u4 BleGattSendIndication(u2 u2AttHandle, u1 *pu1AttValue, u2 len);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLEGATT_H__ */
