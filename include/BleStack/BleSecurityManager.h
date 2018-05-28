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
#ifndef __BLESECURITYMANAGER_H__
#define __BLESECURITYMANAGER_H__

#include "../DhGlobalHead.h"

#define BLE_SM_PASSKEY_SIZE             (6)
#define BLE_SM_ID_EDIV_SIZE             (2)
#define BLE_SM_ID_RAND_SIZE             (8)

#define BLE_SECURITY_KEY_LEN            16

#define BLE_PAIRING_BONDING              (0x01)
#define BLE_PAIRING_NO_BONDING           (0x00)

#define BLE_PAIRING_MITM_NEED            (0x01)
#define BLE_PAIRING_MITM_NO_NEED         (0x00)

#define BLE_PAIRING_OOB_NOT_PRESENT      (0x00)
#define BLE_PAIRING_OOB_PRESENT          (0x01)

#define BLE_PAIRING_FAILED_MASTER        (1)    // master端发送的配对失败
#define BLE_PAIRING_FAILED_SLAVE         (0)

#define SM_KEY_MAX_SIZE                 (16)
#define SM_KEY_MIN_SIZE                 (7)

#define SM_FAILED_PASSKEY_ENTRY         0x01
#define SM_FAILED_OOB_NOT_AVAILABLE     0x02
#define SM_FAILED_AUTH_REQUIREMENTS     0x03        // 从机发送安全请求后，收到主机的配对请求后应该检查是否符合安全请求。
#define SM_FAILED_CONFIRM_VALUE         0x04
#define SM_FAILED_PAIRING_NOT_SUPPORT   0x05
#define SM_FAILED_ENC_KEY_SIZE          0x06
#define SM_FAILED_COMMAND_NOT_SUPPORT   0x07
#define SM_FAILED_UNSPECIFIED_REASON    0x08
#define SM_FAILED_REPEATED_ATTEMPT      0x09
#define SM_FAILED_INVALID_PARAMS        0x0A


#define DEV_DISPLAY_ONLY            0x00
#define DEV_DISPLAY_YES_NO          0x01
#define DEV_KEYBOARD_ONLY           0x02
#define DEV_NO_INPUT_NO_OUTPUT      0x03
#define DEV_KEYBOARD_DISPLAY        0x04
    

/* 当前已经满足的安全状态，PS：选这几个值是为了和ATT中的权限对应方便判断 */
#define SECURITY_STATUS_AUTHENTICATED           (0x04|0x08)
#define SECURITY_STATUS_ENCRYPTED               (0x40|0x80)
#define SECURITY_STATUS_VALID                   (1<<15)         /* 只有在底层链路加密完成后，链路上的各种安全状态才是实际有效的 */

typedef struct
{
    u1  m_bEncKey:1;
    u1  m_bIdKey:1;
    u1  m_bSignKey:1;
}BlkKeyDistribution;

/* Long Term Key */
typedef struct
{
    u1  m_pu1Ltk[BLE_SECURITY_KEY_LEN];
    u1  m_pu1Ediv[BLE_SM_ID_EDIV_SIZE];
    u1  m_pu1Rand[BLE_SM_ID_RAND_SIZE];
}BlkSmLtkInfo;

/* Identity Resolving Key */
typedef struct
{
    u1  m_pu1Irk[BLE_SECURITY_KEY_LEN];
    BlkBleAddrInfo m_blkAddr;
}BlkSmIrkInfo;

/* Connection Signature Resolving Key */
typedef struct
{
    u1  m_pu1Csrk[BLE_SECURITY_KEY_LEN];     
}BlkSmCsrkInfo;

typedef struct
{
    BlkSmLtkInfo    m_blkLtkInfo;
    BlkSmIrkInfo    m_blkIrkInfo;
    BlkSmCsrkInfo   m_blkCsrkInfo;
}BlkSmKeysBuff;

typedef struct
{
    BlkSmKeysBuff   *m_pblkMasterKeys;
    BlkSmKeysBuff   *m_pblkSlaveKeys;
}BlkBleSmKeysExchange;

typedef struct
{
    u1                  m_u1IoCapacity;
    u1                  m_u1OobFlag;
    u1                  m_u1BondFlag;
    u1                  m_u1MimtFlag;               
    u1                  m_u1MaxEncKeySize;      /* 强制为16字节 */
    BlkKeyDistribution  m_blkInitiatorKeyMap;
    BlkKeyDistribution  m_blkResponderKeyMap;
}BlkBleSmParsms;


typedef struct
{
    u1  m_pu1DispKey[BLE_SM_PASSKEY_SIZE];    /* MSB,6字节ASCII码,如值 十进制值364578则数组值为'3','6','4','5','7','8' */
}BlkBleSmKeyDispEvent;

typedef struct
{
    u1 m_u1Reserve;
}BlkBleSmKeyInputEvent;

typedef struct
{
    u1  m_u1Reserve;
}BlkBleSmPairingReqEvent;

typedef struct
{
    u1  m_u1Reserve;
}BlkBleSmEncCompleteEvent;

typedef struct
{
    u1  m_u1Reserve;
}BlKBleSmBondingCompleteEvent;

typedef struct 
{
    u1  m_pu1Ediv[BLE_SM_ID_EDIV_SIZE];     /* LSB */
    u1  m_pu1Rand[BLE_SM_ID_RAND_SIZE];     /* LSB */
}BlkBleSmLtkReqEvent;


typedef struct
{
    u1  m_u1Dir;                  // 哪端发送的配对失败
    u1  m_u1FailedReason;
}BlkBleSmFailedEvent;

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern u4 BleSecurityManagerInit(void);
extern u4 BleSlaveSecurityReq(u1 bongFlag, u1 MITMFlag);
extern u4 BleSmPairingParamsRsp(BlkBleSmParsms params, BlkBleSmKeysExchange keys);
extern u4 BleSmPasskeySet(u1 *pu1Passkey, u1 u1Len);
extern u4 BleSmGetLtk(u1 *pu1Rand, u1 *pu1EDIV, u1 *pu1Ltk, u1 *flag);
extern u4 BleSmLtkSet(u1 *pu1Ltk, u1 size);
extern u4 BleSecurityReqHandle(u1 *pu1Req, u2 u2Len);
extern u2 BleSecurityStatusGet(void);
extern void BleSecurityStatusValid(u2 validFlag);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLESECURITYMANAGER_H__ */
