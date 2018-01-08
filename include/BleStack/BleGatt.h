/**
 * @file				BleGatt.h
 * @brief			BleGatt.c µÄÍ·ÎÄ¼þ
 * @author			fengxun
 * @date			2018å¹´1æœˆ3æ—¥
*/
#ifndef __BLEGATT_H__
#define __BLEGATT_H__

#define BLE_PRIMARY_SERVICE_UUID				0x2800	
#define BLE_INCLUDE_UUID						0x2802
#define BLE_CHARACTERISTIC_DECLARATION_UUID		0x2803
#define BLE_CCID_UUID							0x2902
#define BLE_GENERIC_ACCESS_SERVICE_UUID			0x1800
#define BLE_GENERIC_ATTRIBUTE_SERVICE_UUID		0x1801
#define BLE_DEVICE_NAME_UUID					0x2A00
#define BLE_APPEARANCE_UUID						0x2A01
#define BLE_PERIPHERAL_PRIVACY_FLAG_UUID		0x2A02
#define BLE_PPCP_UUID							0x2A04
#define BLE_RECONNECTION_ADDRESS_UUID			0x2A03
#define BLE_SERVICE_CHANGED_UUID				0x2A05

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
extern u4	BleGattFindAttByHandle(u2 u2Handle, BlkBleAttribute **ppblkAtt);
extern u4 BleGattFindAttByType(u2 u2StartHandle, u2 u2EndHandle, u1 *pu1UUID, u1 UUIDType, BlkBleAttribute **ppblkAtt);
extern u4	BleGattInfoInit(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLEGATT_H__ */
