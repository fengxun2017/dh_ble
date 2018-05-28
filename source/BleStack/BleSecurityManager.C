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

#define nBLE_SM_DEBUG

#if !defined(BLE_SM_DEBUG)
#undef DEBUG_INFO
#define DEBUG_INFO(...)
#undef DEBUG_DATA
#define DEBUG_DATA(...)
#else

char *PAIRING_FAILED_REASON[] = {"SM_FAILED_PASSKEY_ENTRY","SM_FAILED_OOB_NOT_AVAILABLE","SM_FAILED_AUTH_REQUIREMENTS","SM_FAILED_CONFIRM_VALUE"
                                ,"SM_FAILED_PAIRING_NOT_SUPPORT","SM_FAILED_ENC_KEY_SIZE","SM_FAILED_COMMAND_NOT_SUPPORT","SM_FAILED_UNSPECIFIED_REASON" 
                                ,"SM_FAILED_REPEATED_ATTEMPT","SM_FAILED_INVALID_PARAMS"};

char *PAIRING_METHOD_DESC[5] = {"SM_PAIRING_ALG_NOT_SUPPORT", "SM_JUST_WORK", "SM_OOB", "SM_PASSKEY_DISP", "SM_PASSKEY_INPUT"};

#endif

#define SM_PAIRING_REQ                  0x01
#define SM_PAIRING_RSP                  0x02
#define SM_PAIRING_CONFIRM              0x03
#define SM_PAIRING_RAND                 0x04
#define SM_PAIRING_FAILED               0x05
#define SM_ENCRYPTION_INFO              0x06
#define SM_MASTER_IDENTIFICATION        0x07
#define SM_IDENTITY_INFO                0x08
#define SM_IDENTITY_ADDR_INFO           0x09
#define SM_SIGN_INFO                    0x0A
#define SM_SECURITY_REQ                 0x0B



#define SM_MIN_DATA_LEN                 0x02
#define SM_PAIRING_REQ_LEN              0x07
#define SM_PAIRING_RSP_LEN              0x07
#define SM_PAIRING_CONFIRM_LEN          (16)
#define SM_PAIRING_RAND_LEN             (16)
#define SM_PAIRING_ENC_INFO_LEN         (1+BLE_SECURITY_KEY_LEN)
#define SM_PAIRING_MASTER_ID_LEN        (1+10)
#define SM_PAIRING_IRK_LEN              (1+BLE_SECURITY_KEY_LEN)
#define SM_PAIRING_ID_ADDR_LEN          (1+7)
#define SM_PAIRING_SIGN_LEN             (1+BLE_SECURITY_KEY_LEN)

#define SM_BLOCK_SIZE                   (16)


#define SM_AUTH_FLAG                    0x80

/* 高位为1表示认证的，具有MITM保护 */
typedef enum
{
    SM_PAIRING_ALG_NOT_SUPPORT = 0x00,            // 配对算法不支持
    SM_JUST_WORK,
    SM_OOB,
    SM_PASSKEY_DISP = 0x83,
    SM_PASSKEY_INPUT = 0x84,
}EnPairingMethod;


#define PASSKEY_INPUT_CANCEL_FLAG   0x01
#define PASSKEY_INPUT_FLAG          0x02
#define PASSKEY_CONFIRM_FLAG        0x03

typedef struct
{
    u1  m_u1KeySize;                                          /* 配对协商出来的秘钥大小-选择小的那个 */
    u1  m_u1BondFlag;                                         /* 是有密钥分发阶段 */
    u1  m_pu1SlaveRand[SM_PAIRING_RAND_LEN];                /* LSB */
    u1  m_pu1MasterConfirm[SM_PAIRING_CONFIRM_LEN];           /* LSB */
    u1  m_pu1TempKey[BLE_SECURITY_KEY_LEN];                   /* LSB, 临时密钥 */
    u1  m_pu1ShortTermKey[BLE_SECURITY_KEY_LEN];              /* LSB，短期密钥*/
    EnPairingMethod         m_enPairingAlg;
    BlkBleSmParsms          m_blkPeerSmParams;
    BlkBleSmKeysExchange    m_blkKeysExchange;
    /* 对于设备输入，手机显示，设备输入 的passkey模式下，不能保证对方发过来的confirm和自己输入passkey的先后顺序，需要该变量记录 */
    u1  m_u1PairingFlag;
    /*
        保存一下pairing req和pairing rsp的原始数据，虽然重复保存了，但是方便后续计算confirm值
    */
    u1  m_pu1PairingReq[SM_PAIRING_REQ_LEN];    /* opcode占一字节       LSB */
    u1  m_pu1PairingRsp[SM_PAIRING_RSP_LEN];    /* LSB */

    u4  m_u2SecurityStatus;                       /* 最高位表示安全状态指示是否有效，只有链路层加密后安全状态才有效 */
}BlkBleSmInfo;

static BlkBleSmInfo     s_blkSmInfo;

/* s_ppu1IoMapMethod[init][rsp] */
static const EnPairingMethod s_ppu1IoMapMethod[5][5] = {
    SM_JUST_WORK,       SM_JUST_WORK,       SM_PASSKEY_INPUT,   SM_JUST_WORK, SM_PASSKEY_INPUT,
    SM_JUST_WORK,       SM_JUST_WORK,       SM_PASSKEY_INPUT,   SM_JUST_WORK, SM_PASSKEY_INPUT,
    SM_PASSKEY_DISP,    SM_PASSKEY_DISP,    SM_PASSKEY_INPUT,   SM_JUST_WORK, SM_PASSKEY_DISP,
    SM_JUST_WORK,       SM_JUST_WORK,       SM_JUST_WORK,       SM_JUST_WORK, SM_JUST_WORK,
    SM_PASSKEY_DISP,    SM_PASSKEY_DISP,    SM_PASSKEY_INPUT,   SM_JUST_WORK, SM_PASSKEY_INPUT,
};
 
static BlkKeyDistribution GetKeyMap(u1 key)
{
    BlkKeyDistribution  map;
    memset(&map, 0x00, sizeof(map));
    if(key&0x01)
    {
        map.m_bEncKey = 1;
    }
    if(key&0x02)
    {
        map.m_bIdKey = 1;
    }
    if(key&0x04)
    {
        map.m_bSignKey = 1;   
    }

    return map;
}

static u1 GetKeyValue(BlkKeyDistribution keyMap)
{
    u1 keyValue = 0;
    if( 1 == keyMap.m_bEncKey )
    {
        keyValue = 0x01;
    }
    if( 1==keyMap.m_bIdKey )
    {
        keyValue |= 0x02;
    }
    if( 1==keyMap.m_bSignKey )
    {
        keyValue |= 0x04;
    }

    return keyValue;
}

/**
 *@brief: 		SmAesEnc
 *@details:		加密16字节数据
 *@param[in]	pu1In       数据Buff,LSB输入，内部加密前转换成MSB格式
 *@param[in]    pu1Key      秘钥，LSB输入，内部加密前转换成MSB。
 *@param[out]   pu1Out      输出密文，LSB输出
 
 *@retval:		DH_SUCCESS
 */
static u4 SmAesEnc(u1 *pu1In, u1 *pu1Key, u1 *pu1Out)
{

    DhAesEnc(pu1In, pu1Key, pu1Out);

    return DH_SUCCESS;
}

/**
 *@brief: 		SmGetRandom
 *@details:		获取随机数
 *@param[out]	pu1Data  
 *@param[int]	len      要获取的长度 
 
 *@retval:		
 */
static void SmGetRandom(u1 *pu1Data, u2 len)
{
   DhGetRand(pu1Data, len);
}

/**
 *@brief: 		SmPasskeyDispSet
 *@details:		将3字节passkey转换成可显示的6位ASCII码
 *@param[in]	pu1In       passkey实际值，LSB输入  
 *@param[in]	pu1Out      ASCII码输出，MSB输出
 *@param[out]	?
 *@retval:		static
 */
static void SmPasskeyDispSet(u1 *pu1In, u1 *pu1Out)
{
    u4  value;
    u1  i;
    u1  pu1Tmp[BLE_SM_PASSKEY_SIZE];
    
    if( NULL==pu1In || NULL==pu1Out )
    {
        return ;
    }

    value = pu1In[0] + (pu1In[1]<<8) + (pu1In[2]<<16);

    i = 0;
    while( i < BLE_SM_PASSKEY_SIZE )
    {
        pu1Tmp[i++] = value%10+'0';
        value /= 10;
    }
    for( i = 0; i < BLE_SM_PASSKEY_SIZE; i++ )
    {
        pu1Out[i] = pu1Tmp[BLE_SM_PASSKEY_SIZE-i-1]; 
    }
}


u4 BlePairingFailedHandle(u1 dir, u1 *pu1Data, u2 u2Len)
{
    BlkBleEvent bleEvent;
    
    bleEvent.m_u2EvtType = BLE_EVENT_SM_FAILED;
    bleEvent.m_event.m_blkSmFailed.m_u1Dir = dir;
    bleEvent.m_event.m_blkSmFailed.m_u1FailedReason = pu1Data[1];

    /* 清空安全管理信息 */
    memset(&s_blkSmInfo, 0x00, sizeof(s_blkSmInfo));
    return BleEventPush(bleEvent);
}

/**
 *@brief: 		SmPairingFailed
 *@details:		从机终止配对过程，发送配对失败。
 *@param[in]	u1Reason  

 *@retval:		DH_SUCCESS
 */
static u4 SmPairingFailed(u1 u1Reason)
{
    u1  pu1SmData[2];
    u1  u1Index = 0;

    DEBUG_INFO("pairing failed:%s",PAIRING_FAILED_REASON[u1Reason-1]);
    pu1SmData[u1Index++] = SM_PAIRING_FAILED;
    pu1SmData[u1Index++] = u1Reason;

    /* 配对失败清空信息 */
    memset(&s_blkSmInfo, 0x00, sizeof(s_blkSmInfo));
    BleL2capDataSend(BLE_L2CAP_SM_CHANNEL_ID, pu1SmData, u1Index);

    return BlePairingFailedHandle(BLE_PAIRING_FAILED_SLAVE, pu1SmData, 2);
}

static EnPairingMethod SmPairingMethodSelect(u1 IoCap, u1 MITMFlag)
{
    EnPairingMethod method;

    if (BLE_PAIRING_MITM_NO_NEED==MITMFlag && BLE_PAIRING_MITM_NO_NEED==s_blkSmInfo.m_blkPeerSmParams.m_u1MimtFlag )
    {
        method =  SM_JUST_WORK;
    }
    else
    {
        method = s_ppu1IoMapMethod[s_blkSmInfo.m_blkPeerSmParams.m_u1IoCapacity][IoCap];
    }
    DEBUG_INFO("pairing method:%s", PAIRING_METHOD_DESC[method]);
    
    return method;  
}


/**
 *@brief: 		CalcConfirmValue
 *@details:		计算配对过程中的confirm值
 *@param[in]	pu1Rand         随机数，LSB
 *@param[out]	pu1Out          LSB

 *@retval:		void
 */
static void CalcConfirmValue(u1 *pu1Rand, u1 *pu1Out)
{
    
    u1  pu1P1[16];
    u1  pu1P2[16];    
    u1  u1Index;
	
    BlkBleAddrInfo  peerAddr;
    BlkBleAddrInfo  selfAddr;

    if( NULL==pu1Out || NULL==pu1Rand )
    {
        return ;
    }
    
    BleLinkSelfAddrInfoGet(&selfAddr);
    BleLinkPeerAddrInfoGet(&peerAddr);  
    /*
        Sconfirm = c1(TK, Srand,Pairing Request command, Pairing Response command,
                      initiating device address type, initiating devices address,
                      responding device address type, responding device address)
        p1 = pres || preq || rat’ || iat’
        p2 = padding || ia || ra
        c1 (k, r, preq, pres, iat, rat, ia, ra) = e(k, e(k, r XOR p1) XOR p2)
    */
    memset(pu1P1, 0x00, sizeof(pu1P1));
    memset(pu1P2, 0x00, sizeof(pu1P2));
    u1Index = 0;
    pu1P1[u1Index++] = peerAddr.m_u1AddrType;pu1P1[u1Index++] = selfAddr.m_u1AddrType;
    memcpy(pu1P1+u1Index, s_blkSmInfo.m_pu1PairingReq, SM_PAIRING_REQ_LEN);
    u1Index += SM_PAIRING_REQ_LEN;
    memcpy(pu1P1+u1Index, s_blkSmInfo.m_pu1PairingRsp, SM_PAIRING_RSP_LEN);
    u1Index += SM_PAIRING_RSP_LEN;

    DhMemxor(pu1P1, pu1Rand, SM_BLOCK_SIZE);
    SmAesEnc(pu1P1, s_blkSmInfo.m_pu1TempKey, pu1P1);

    u1Index = 0;
    memcpy(pu1P2+u1Index, selfAddr.m_pu1Addr, BLE_ADDR_LEN);
    u1Index += BLE_ADDR_LEN;
    memcpy(pu1P2+u1Index, peerAddr.m_pu1Addr, BLE_ADDR_LEN);

    DhMemxor(pu1P2, pu1P1, SM_BLOCK_SIZE);
    SmAesEnc(pu1P2, s_blkSmInfo.m_pu1TempKey, pu1Out);

}
static u4 SmConfirmRsp()
{
    u1  pu1ConfirmRsp[SM_PAIRING_CONFIRM_LEN+1];    //opcode
    u4  ret = DH_SUCCESS;
    
    SmGetRandom(s_blkSmInfo.m_pu1SlaveRand, SM_PAIRING_RAND_LEN);
    
    pu1ConfirmRsp[0] = SM_PAIRING_CONFIRM;
    CalcConfirmValue(s_blkSmInfo.m_pu1SlaveRand, pu1ConfirmRsp+1);
    
    ret = BleL2capDataSend(BLE_L2CAP_SM_CHANNEL_ID, pu1ConfirmRsp, SM_PAIRING_CONFIRM_LEN+1);
    DEBUG_INFO("slave confirm:");
    DEBUG_DATA(pu1ConfirmRsp+1, SM_PAIRING_CONFIRM_LEN);

    return ret;
}


u4 SmPairingReqHandle(u1 *pu1Req, u2 u2Len)
{
    u1  u1IoCap;
    u1  u1OOBflag;
    u1  m_u1BondFlag;
    u1  u1MitmFlag;
    u1  u1MaxKeySize;
    u1  u1InitiatorKeys;
    u1  u1RespKeys;
    u1  u1Index = 0;

    BlkBleEvent blkBleEvent;

    DEBUG_INFO("pairing req:");
    DEBUG_DATA(pu1Req+1, u2Len-1);
    
    BleSecurityManagerInit();   //清空状态
    if( u2Len!=SM_PAIRING_REQ_LEN )
    {
        return SmPairingFailed(SM_FAILED_INVALID_PARAMS);
    }
    u1Index = 1;    //第一字节为opcode
    u1IoCap = pu1Req[u1Index++];
    u1OOBflag = pu1Req[u1Index++];
    
    m_u1BondFlag = pu1Req[u1Index]&0x03;
    u1MitmFlag = pu1Req[u1Index++]&0x04;
    
    u1MaxKeySize = pu1Req[u1Index++];
    u1InitiatorKeys = pu1Req[u1Index++];
    u1RespKeys = pu1Req[u1Index++];

    if( u1IoCap>4 || u1OOBflag>1 || m_u1BondFlag>1 || u1MaxKeySize<SM_KEY_MIN_SIZE || u1MaxKeySize>SM_KEY_MAX_SIZE )
    {
        s_blkSmInfo.m_enPairingAlg = SM_PAIRING_ALG_NOT_SUPPORT;    //参数错误的情况要将配对算法设置一下，后面收到配对其他指令要先判断一下配对算法。
        return SmPairingFailed(SM_FAILED_INVALID_PARAMS);
    }

        /* 不支持OOB */
    if ( BLE_PAIRING_OOB_PRESENT == u1OOBflag )
    {
        return SmPairingFailed(SM_FAILED_OOB_NOT_AVAILABLE);
    }
    
    s_blkSmInfo.m_blkPeerSmParams.m_u1OobFlag = u1OOBflag;
    s_blkSmInfo.m_blkPeerSmParams.m_u1MimtFlag = u1MitmFlag;
    s_blkSmInfo.m_blkPeerSmParams.m_u1BondFlag = m_u1BondFlag;
    s_blkSmInfo.m_blkPeerSmParams.m_u1MaxEncKeySize = u1MaxKeySize;
    s_blkSmInfo.m_blkPeerSmParams.m_u1IoCapacity = u1IoCap;
    s_blkSmInfo.m_blkPeerSmParams.m_blkResponderKeyMap = GetKeyMap(u1RespKeys);
    s_blkSmInfo.m_blkPeerSmParams.m_blkInitiatorKeyMap = GetKeyMap(u1InitiatorKeys);

    memcpy(s_blkSmInfo.m_pu1PairingReq, pu1Req, SM_PAIRING_REQ_LEN);
    /* 需要上层回复配对请求 */
    memset(&blkBleEvent, 0x00, sizeof(blkBleEvent));
    blkBleEvent.m_u2EvtType = BLE_EVENT_SM_PAIRING_REQ;
    return BleEventPush(blkBleEvent);
    
}


u4 SmPairingConfirmHandle(u1 *pu1Req, u2 u2Len)
{
    u1  rspFlag;
    
    DEBUG_INFO("pairing confirm:");
    DEBUG_DATA(pu1Req+1, u2Len-1);
    if( u2Len != (SM_PAIRING_CONFIRM_LEN+1) )
    {
        return SmPairingFailed(SM_FAILED_INVALID_PARAMS);
    }

    if( SM_KEY_MAX_SIZE != s_blkSmInfo.m_u1KeySize)
    {
        return SmPairingFailed(SM_FAILED_ENC_KEY_SIZE);
    }
    if( PASSKEY_INPUT_CANCEL_FLAG&s_blkSmInfo.m_u1PairingFlag )
    {
        return SmPairingFailed(SM_FAILED_PASSKEY_ENTRY);
    }
    if( SM_JUST_WORK == s_blkSmInfo.m_enPairingAlg )
    {
        /* justwork模式 TK为0 */
        memset(s_blkSmInfo.m_pu1TempKey, 0x00, BLE_SECURITY_KEY_LEN);
    }

    if( SM_PASSKEY_INPUT == s_blkSmInfo.m_enPairingAlg)
    {
        CRITICAL_REGION_ENTER();
        if( s_blkSmInfo.m_u1PairingFlag & PASSKEY_INPUT_FLAG )
        {
            /* 已经收到输入过passkey了，则这里回复confirm rsp */
            rspFlag = 1;
        }
        else
        {
            /* 等输入passkey后再回confim rsp  */
            s_blkSmInfo.m_u1PairingFlag |= PASSKEY_CONFIRM_FLAG;
        }
        CRITICAL_REGION_EXIT();
    }
    else
    {
        /* 其他模式不需要等待输入过程 */
        rspFlag = 1;
    }

    memcpy(s_blkSmInfo.m_pu1MasterConfirm, pu1Req+1, SM_PAIRING_CONFIRM_LEN);
    if ( rspFlag )
    {
        return SmConfirmRsp();
    }    

    return DH_SUCCESS;
}

u4 SmPairingRandHandle(u1 *pu1Req, u2 u2Len)
{
    u1  pu1Confirm[SM_PAIRING_CONFIRM_LEN];
    u1  pu1RandRsp[SM_PAIRING_RAND_LEN+1];
    u1  R[SM_PAIRING_RAND_LEN];
    u1  *pu1Mrand;

    DEBUG_INFO("pairing rand:");
    DEBUG_DATA(pu1Req+1, u2Len-1);
    if( u2Len != (SM_PAIRING_RAND_LEN+1) )
    {
        return SmPairingFailed(SM_FAILED_INVALID_PARAMS);
    }
    pu1Mrand = pu1Req+1;
    CalcConfirmValue(pu1Mrand, pu1Confirm);
    DEBUG_INFO("confirm:");
    DEBUG_DATA(pu1Confirm, SM_PAIRING_CONFIRM_LEN);
    if( memcmp(pu1Confirm, s_blkSmInfo.m_pu1MasterConfirm, SM_PAIRING_CONFIRM_LEN) !=0 )
    {
        return SmPairingFailed(SM_FAILED_CONFIRM_VALUE);
    }

    pu1RandRsp[0] = SM_PAIRING_RAND;
    memcpy(pu1RandRsp+1, s_blkSmInfo.m_pu1SlaveRand, SM_PAIRING_RAND_LEN);
    BleL2capDataSend(BLE_L2CAP_SM_CHANNEL_ID, pu1RandRsp, SM_PAIRING_RAND_LEN+1);

    DEBUG_INFO("pairing rand rsp:");
    DEBUG_DATA(pu1RandRsp+1, SM_PAIRING_RAND_LEN);
    
    /*  
        STK = s1(TK, Srand, Mrand)        r1 set to Srand, and r2 set to Mrand
        s1(k, r1, r2) = e(k, r’)          r’ = r1’ || r2’
        
    */
    memcpy(R, pu1Mrand, 8);
    memcpy(R+8, s_blkSmInfo.m_pu1SlaveRand, 8);
    SmAesEnc(R, s_blkSmInfo.m_pu1TempKey, s_blkSmInfo.m_pu1ShortTermKey);
    
    DEBUG_INFO("short term key:");
    DEBUG_DATA(s_blkSmInfo.m_pu1ShortTermKey, BLE_SECURITY_KEY_LEN);
    
    return DH_SUCCESS;
}



u4 BleSecurityReqHandle(u1 *pu1Req, u2 u2Len)
{
    u1  u1Opcode;
	u4	u4Ret = DH_SUCCESS;
	
    if( NULL == pu1Req )
    {
        return ERR_SM_INVALID_PARAMS;
    }
    if( u2Len < SM_MIN_DATA_LEN )
    {
        return SmPairingFailed(SM_FAILED_INVALID_PARAMS);
    }
    u1Opcode = pu1Req[0];

    switch(u1Opcode)
    {
        case SM_PAIRING_REQ:
            u4Ret= SmPairingReqHandle(pu1Req, u2Len);
        break;

        case SM_PAIRING_RAND:
            u4Ret = SmPairingRandHandle(pu1Req, u2Len);
        break;

        case SM_PAIRING_CONFIRM:
            u4Ret = SmPairingConfirmHandle(pu1Req, u2Len);
        break;

        case SM_PAIRING_FAILED:
            u4Ret = BlePairingFailedHandle(BLE_PAIRING_FAILED_MASTER, pu1Req, u2Len);
        break;
        
        default:
             u4Ret = SmPairingFailed(SM_FAILED_COMMAND_NOT_SUPPORT);
        break;
    }
       
    return u4Ret;
}

u4 BleSecurityManagerInit(void)
{
    memset(&s_blkSmInfo, 0x00, sizeof(s_blkSmInfo));
    return DH_SUCCESS;
}

/**
 *@brief: 		BleSmPairingParamsRsp
 *@details:		配对响应
 *@param[in]	params      配对相关参数设置
 *@param[in]	keysBuff    如果设置了绑定，需要提供秘钥buff，用来存储分发的秘钥。

 *@retval:		DH_SUCCESS
 */
u4 BleSmPairingParamsRsp(BlkBleSmParsms params, BlkBleSmKeysExchange keysBuff)
{
    u1  m_pu1PairingRsp[SM_PAIRING_RSP_LEN];
    u1  u1Index = 0;

    BlkBleEvent bleEvent;
    
    /* 如果设置了绑定，必须要有keysBuff参数 */
    if( (BLE_PAIRING_BONDING==params.m_u1BondFlag) && (NULL==keysBuff.m_pblkMasterKeys || NULL==keysBuff.m_pblkSlaveKeys))
    {
        return ERR_SM_INVALID_PARAMS;
    }
    s_blkSmInfo.m_blkKeysExchange = keysBuff;
    s_blkSmInfo.m_u1KeySize = DH_MIN(params.m_u1MaxEncKeySize, s_blkSmInfo.m_blkPeerSmParams.m_u1MaxEncKeySize);

    /* 不支持OOB，密钥长度强制设置成16 */
    if( BLE_PAIRING_OOB_PRESENT==params.m_u1OobFlag  || params.m_u1MaxEncKeySize!=SM_KEY_MAX_SIZE)
    {
        return SmPairingFailed(SM_FAILED_UNSPECIFIED_REASON);
    }
    
    s_blkSmInfo.m_enPairingAlg = SmPairingMethodSelect(params.m_u1IoCapacity, params.m_u1MimtFlag);
    if( BLE_PAIRING_BONDING==params.m_u1BondFlag && BLE_PAIRING_BONDING==s_blkSmInfo.m_blkPeerSmParams.m_u1BondFlag )
    {
        s_blkSmInfo.m_u1BondFlag = BLE_PAIRING_BONDING;
    }
    s_blkSmInfo.m_u1KeySize = DH_MIN(params.m_u1MaxEncKeySize, s_blkSmInfo.m_blkPeerSmParams.m_u1MaxEncKeySize);
    m_pu1PairingRsp[u1Index++] = SM_PAIRING_RSP;
    m_pu1PairingRsp[u1Index++] = params.m_u1IoCapacity;
    m_pu1PairingRsp[u1Index++] = params.m_u1OobFlag;
    m_pu1PairingRsp[u1Index++] = (params.m_u1BondFlag | (params.m_u1MimtFlag<<2))&0x07;   // 保留位应该都设置成0
    m_pu1PairingRsp[u1Index++] = params.m_u1MaxEncKeySize;
    /* 规范要求主机设置为0的，从机就不要再设置为1 */
    m_pu1PairingRsp[u1Index++] = GetKeyValue(params.m_blkInitiatorKeyMap)&GetKeyValue(s_blkSmInfo.m_blkPeerSmParams.m_blkInitiatorKeyMap)&0xFF;
    m_pu1PairingRsp[u1Index++] = GetKeyValue(params.m_blkResponderKeyMap)&GetKeyValue(s_blkSmInfo.m_blkPeerSmParams.m_blkResponderKeyMap)&0xFF;

    memcpy(s_blkSmInfo.m_pu1PairingRsp, m_pu1PairingRsp, SM_PAIRING_RSP_LEN);

    /* 回复配对响应 */
    BleL2capDataSend(BLE_L2CAP_SM_CHANNEL_ID, m_pu1PairingRsp, u1Index);

    DEBUG_INFO("pairing rsp:");
    DEBUG_DATA(m_pu1PairingRsp+1, u1Index-1);
    
    /* 先清空，just work模式为0，passkey_disp模式为随机生成3字节，passkey_input为输入的6位ASCII码 */
    memset(s_blkSmInfo.m_pu1TempKey, 0x00, BLE_SECURITY_KEY_LEN);
    
    /* 密钥长度需要才进行后续步骤，不满足则不进行，再后续收到confirm后回复配对失败 */
    if(SM_KEY_MAX_SIZE==s_blkSmInfo.m_u1KeySize && SM_JUST_WORK!=s_blkSmInfo.m_enPairingAlg)
    {
        memset(&bleEvent, 0x00, sizeof(bleEvent));
        if( SM_PASSKEY_DISP == s_blkSmInfo.m_enPairingAlg )
        {
            /* 生成passkey--即临时密钥 6为ASCII码，3字节 */
            SmGetRandom(s_blkSmInfo.m_pu1TempKey, 3);//----需要设置小于等于999999
            bleEvent.m_u2EvtType = BLE_EVENT_SM_DISP_KEY;
            /* 转换成6位ASCII码，LSB */
            SmPasskeyDispSet(s_blkSmInfo.m_pu1TempKey, bleEvent.m_event.m_blkSmKeyDisp.m_pu1DispKey);
        }
        else if( SM_PASSKEY_INPUT == s_blkSmInfo.m_enPairingAlg )
        {
            /* 需要上层提供passkey */
            bleEvent.m_u2EvtType = BLE_EVENT_SM_INPUT_KEY;
        }
        BleEventPush(bleEvent);
    }

    return DH_SUCCESS;
}

/**
 *@brief: 		BleSmPasskeySet
 *@details:		输入6位配对码的ascii码,如果取消输入，则key参数传入NULL或长度字段设置成0
 *@param[in]	pu1Passkey      6位数字ASCII码，MSB
 *@param[in]	u1Len          
 *@retval:		DH_SUCCESS
 */
u4 BleSmPasskeySet(u1 *pu1Passkey, u1 u1Len)
{
    u1  i;
    u4  u4Times;
    u4  u4Value;
    u1  rspFlag = 0;
    if( NULL==pu1Passkey || BLE_SM_PASSKEY_SIZE!=u1Len )
    {
        s_blkSmInfo.m_u1PairingFlag |= PASSKEY_INPUT_CANCEL_FLAG;
        return DH_SUCCESS;
    }

    u4Value = 0;
    u4Times = 1;
    for( i = 0; i < 6; i++ )
    {
        u4Value = u4Value*u4Times + (pu1Passkey[i]-'0');
        u4Times *= 10;
    }
    
    memset(s_blkSmInfo.m_pu1TempKey, 0x00, BLE_SECURITY_KEY_LEN);
    /* 0-999999 3字节表示 */
    for( i = 0; i < 3; i++ )
    {
        s_blkSmInfo.m_pu1TempKey[i] = u4Value&0xFF;
        u4Value >>= 8;
    }

    CRITICAL_REGION_ENTER();
    if( s_blkSmInfo.m_u1PairingFlag & PASSKEY_CONFIRM_FLAG )
    {
        /* 已经收到confirm了，则这里回复confirm rsp */
        rspFlag = 1;
    }
    else    
    {
        /* 等收到confirm再回 */
        s_blkSmInfo.m_u1PairingFlag |= PASSKEY_INPUT_FLAG;
    }
    CRITICAL_REGION_EXIT();
    
    /* 已经收到过confirm了，则这里直接回confirm */
    if ( rspFlag )
    {
        return SmConfirmRsp();
    }
    
    return DH_SUCCESS;
}

/**
 *@brief: 		BleSmGetLtk
 *@details:		由EDIV和random来过去LTK
 *@param[in]	pu1Rand,pu1EDIV :均为LSB格式，用来标示LTK，如果rand，ediv都为0，则返回的是本次配对协商的stk
 				  
 *@param[out]   putLtk  返回用来生成会话密钥的LTK，LSB
 *@param[in]    flag    如果立刻获取到了LTK，则返回1，如果需要上层通过接口返回LTK，则置为0.
 
 *@retval:		
 */
u4 BleSmGetLtk(u1 *pu1Rand, u1 *pu1EDIV, u1 *pu1Ltk, u1 *flag)
{
    u1  pu1Zero[BLE_SM_ID_RAND_SIZE];
    BlkBleEvent bleEvent;
    
    memset(pu1Zero, 0x00, sizeof(pu1Zero));
    if( memcmp(pu1Rand, pu1Zero, BLE_SM_ID_RAND_SIZE)==0 && memcmp(pu1EDIV, pu1Zero, BLE_SM_ID_EDIV_SIZE)==0 )
    {
        memcpy(pu1Ltk, s_blkSmInfo.m_pu1ShortTermKey, SM_KEY_MAX_SIZE);
        *flag = 1;
        return DH_SUCCESS;
    }

    *flag = 0;
    /* 从上层获取以前保存的LTK */
    bleEvent.m_u2EvtType = BLE_EVENT_SM_LTK_REQ;
    memcpy(bleEvent.m_event.m_blkSmLtkReq.m_pu1Ediv, pu1EDIV, BLE_SM_ID_EDIV_SIZE);
    memcpy(bleEvent.m_event.m_blkSmLtkReq.m_pu1Rand, pu1Rand, BLE_SM_ID_RAND_SIZE);

    return BleEventPush(bleEvent);
}

/**
 *@brief: 		BleSmLtkSet
 *@details:		输入LTK
 *@param[in]	pu1Ltk  : LTK, LSB格式
 				size    : 密钥长度16字节
 *@retval:		
 */
u4 BleSmLtkSet(u1 *pu1Ltk, u1 size)
{
    u1 pu1SKD[BLE_ENC_SKD_SIZE];
    u1 pu1SK[BLE_ENC_KEY_SIZE];
    if( NULL==pu1Ltk || BLE_ENC_KEY_SIZE!=size )
    {
        return ERR_SM_INVALID_PARAMS;
    }

    LinkEncSkdGet(pu1SKD);
    DhAesEnc(pu1SKD, pu1Ltk, pu1SK);
    LinkEncInfoCfg(pu1SK, NULL, NULL);

    return DH_SUCCESS;
}

/**
 *@brief: 		BleSlaveSecurityReq
 *@details:		从机安全请求
 *@param[in]	bongFlag    是(1)否(0)需要绑定
 *@param[in]	MITMFlag    是(1)否(0)需要防中间人攻击保护
 *@param[out]	
 *@retval:		
 */
u4 BleSlaveSecurityReq(u1 bongFlag, u1 MITMFlag)
{
    u1  pu1SecReq[2];

    pu1SecReq[0] = SM_SECURITY_REQ;
    pu1SecReq[1] = (bongFlag&0x03) | (MITMFlag?0x04:0x00);
    
    BleL2capDataSend(BLE_L2CAP_SM_CHANNEL_ID, pu1SecReq, 2);
    return DH_SUCCESS;
}

void BleSecurityStatusValid(u2 validFlag)
{
    if(validFlag)
    {
        s_blkSmInfo.m_u2SecurityStatus |= (SECURITY_STATUS_VALID|SECURITY_STATUS_ENCRYPTED);
    }
    else
    {
        s_blkSmInfo.m_u2SecurityStatus &= ~(SECURITY_STATUS_VALID|SECURITY_STATUS_ENCRYPTED);
    }
}

u2 BleSecurityStatusGet(void)
{
    return s_blkSmInfo.m_u2SecurityStatus;
}
