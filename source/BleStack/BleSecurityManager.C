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


#define SM_PAIRING_REQ                  0x01
#define SM_PAIRING_RSP                  0x02
#define SM_PAIRING_CONFIRM              0x03
#define SM_PAIRING_RANDOM               0x04
#define SM_PAIRING_FAILED               0x05
#define SM_ENCRYPTION_INFO              0x06
#define SM_MASTER_IDENTIFICATION        0x07
#define SM_IDENTITY_INFO                0x08
#define SM_IDENTITY_ADDR_INFO           0x09
#define SM_SIGN_INFO                    0x0A
#define SM_SECURITY_REQ                 0x0B

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


#define BLE_SECURITY_KEY_LEN            16
#define BLE_SECURITY_RAND_LEN           16
#define BLE_SECURITY_CONFIRM_LEN        16

#define SM_MIN_DATA_LEN                 0x02
#define SM_PAIRING_REQ_LEN              0x06
#define SM_PAIRING_CONFIRM_LEN          (BLE_SECURITY_CONFIRM_LEN)
#define SM_PAIRING_RANDOM_LEN           (BLE_SECURITY_RAND_LEN)
#define SM_PAIRING_ENC_INFO_LEN         (BLE_SECURITY_KEY_LEN)
#define SM_PAIRING_MASTER_ID_LEN        (10)
#define SM_PAIRING_IRK_LEN              (BLE_SECURITY_KEY_LEN)
#define SM_PAIRING_ID_ADDR_LEN          (7)
#define SM_PAIRING_SIGN_LEN             (BLE_SECURITY_KEY_LEN)


#define SM_PAIRING_OOB_NOT_PRESENT      (0x00)
#define SM_PAIRING_OOB_PRESENT          (0x01)

#define SM_PAIRING_BONDING              (0x01)
#define SM_PAIRING_NO_BONDING           (0x00)

#define SM_PAIRING_MITM_NEED            (0x01)
#define SM_PAIRING_MITM_NO_NEED         (0x00)

#define SM_KEY_MAX_SIZE                 (16)
#define SM_KEY_MIN_SIZE                 (7)


#define SM_AUTH_FLAG                    0x80
/* 高位为1表示认证的，具有MITM保护 */
typedef enum
{
    SM_JUST_WORK = 0x00,
    SM_OOB,
    SM_PAIRING_ALG_NOT_SUPPORT,            // 配对算法不支持
    SM_PASSKEY_DISP = 0x81,
    SM_PASSKEY_INPUT = 0x82,
}EnPairingMethod;

typedef struct
{
    u1  u1KeySize;                                          /* 配对协商出来的秘钥大小-选择小的那个 */
    u1  pu1SlaveRand[BLE_SECURITY_RAND_LEN];
    u1  pu1MasterConfirm[BLE_SECURITY_CONFIRM_LEN];         /* LSB */
    u1  pu1TempKey[BLE_SECURITY_KEY_LEN];                   /* MSB */
    u1  pu1ShortTermKey[BLE_SECURITY_KEY_LEN];              /* MSB */
    EnPairingMethod m_enPairingAlg;
}BlkBleSmInfo;

static BlkBleSmInfo     s_blkSmInfo;
static BlkBleSmParsms   s_blkSmParams;

/* s_ppu1IoMapMethod[init][rsp] */
static const u1 s_ppu1IoMapMethod[5][5] = {
    SM_JUST_WORK,       SM_JUST_WORK,       SM_PASSKEY_INPUT,   SM_JUST_WORK, SM_PASSKEY_INPUT,
    SM_JUST_WORK,       SM_JUST_WORK,       SM_PASSKEY_INPUT,   SM_JUST_WORK, SM_PASSKEY_INPUT,
    SM_PASSKEY_DISP,    SM_PASSKEY_DISP,    SM_PASSKEY_INPUT,   SM_JUST_WORK, SM_PASSKEY_DISP,
    SM_JUST_WORK,       SM_JUST_WORK,       SM_JUST_WORK,       SM_JUST_WORK, SM_JUST_WORK,
    SM_PASSKEY_DISP,    SM_PASSKEY_DISP,    SM_PASSKEY_INPUT,   SM_JUST_WORK, SM_PASSKEY_INPUT,
};
 



static u4 SmPairingFail(u1 u1Reason)
{
    u1  pu1SmData[2];
    u1  u1Index = 0;
    BlkHostToLinkData   blkData;

    pu1SmData[u1Index++] = SM_PAIRING_FAILED;
    pu1SmData[u1Index++] = u1Reason;
    
    return BleL2capDataSend(BLE_L2CAP_SM_CHANNEL_ID, pu1SmData, u1Index);
}

static EnPairingMethod SmPairingMethodSelect(u1 OOBFlag, u1 peerIoCap, u1 MITMFlag)
{
    EnPairingMethod method;

    /* 不支持OOB */
    if ( SM_PAIRING_OOB_PRESENT==OOBFlag )
    {
        SmPairingFail(SM_FAILED_OOB_NOT_AVAILABLE);
        method = SM_PAIRING_ALG_NOT_SUPPORT;
    }
    else if (SM_PAIRING_MITM_NO_NEED==MITMFlag && SM_PAIRING_MITM_NO_NEED==s_blkSmParams.m_u1MimtFlag )
    {
        method =  SM_JUST_WORK;
    }
    else
    {
        method = s_ppu1IoMapMethod[peerIoCap][s_blkSmParams.m_enIoCapacity];
    }
    
    return method;  
}
static u4 SmPairingReqHandle(u1 *pu1Req, u2 u2Len)
{
    u1  u1IoCap;
    u1  u1OOBflag;
    u1  u1BondFlag;
    u1  u1MitmFlag;
    u1  u1MaxKeySize;
    u1  u1InitiatorKeys;
    u1  u1RespKeys;
    u1  u1Index = 0;
    
    if( NULL==pu1Req || u2Len<SM_PAIRING_REQ_LEN )
    {
        return ERR_SM_INVALID_PARAMS;
    }
    u1IoCap = pu1Req[u1Index++];
    u1OOBflag = pu1Req[u1Index++];
    
    u1BondFlag = pu1Req[u1Index]&0x03;
    u1MitmFlag = pu1Req[u1Index++]&0x04;
    
    u1MaxKeySize = pu1Req[u1Index++];
    u1InitiatorKeys = pu1Req[u1Index++];
    u1RespKeys = pu1Req[u1Index++];

    if( u1IoCap>4 || u1OOBflag>1 || u1BondFlag>1 || u1MaxKeySize<SM_KEY_MIN_SIZE || u1MaxKeySize>SM_KEY_MAX_SIZE )
    {
        s_blkSmInfo.m_enPairingAlg = SM_PAIRING_ALG_NOT_SUPPORT;    //参数错误的情况要将配对算法设置一下，后面收到配对其他指令要先判断一下配对算法。
        return SmPairingFail(SM_FAILED_INVALID_PARAMS);
    }
    
    s_blkSmInfo.m_enPairingAlg = SmPairingMethodSelect(u1OOBflag, u1IoCap, u1MitmFlag);

    /* 需要上层回复配对请求 */
    
}
u4 BleSecurityReqHandle(u1 *pu1Req, u2 u2Len)
{
    u1  u1Opcode;
    if( NULL == pu1Req )
    {
        return ERR_SM_INVALID_PARAMS;
    }
    if( u2Len < SM_MIN_DATA_LEN )
    {
        return SmPairingFail(SM_FAILED_INVALID_PARAMS);
    }

    u1Opcode = pu1Req[0];

    switch(u1Opcode)
    {
        case SM_PAIRING_REQ:
            SmPairingReqHandle(pu1Req+1, u2Len-1);
        break;

        case SM_PAIRING_RANDOM:

        break;

        case SM_PAIRING_CONFIRM:

        break;
        
        default:
            return SmPairingFail(SM_FAILED_COMMAND_NOT_SUPPORT);
        break;
    }
    
    
    return DH_SUCCESS;
}

u4 BleSecurityManagerInit(void)
{
    memset(&s_blkSmInfo, 0x00, sizeof(s_blkSmInfo));
    s_blkSmInfo.m_enPairingAlg = SM_PAIRING_ALG_NOT_SUPPORT;

    return DH_SUCCESS;
}


/**
 *@brief: 		BleSmPasskeySet
 *@details:		输入6位配对码的ascii码
 *@param[in]	pu1Passkey  

 *@retval:		DH_SUCCESS
 */
u4 BleSmPasskeySet(u1 *pu1Passkey)
{
    
}
