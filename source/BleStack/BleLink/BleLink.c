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


/**
 * @file				BleLink.c
 * @brief			ble链路层状态机，Ble链路层分为广播态，连接中状态，连接上状态，空闲待命状态。
 					每个状态下又细分字状态。如广播态分：发送结束，接收结束，发送扫描响应结束，广播空闲。
 					协议栈的底层处理分为2部分处理，时序相关的在中断中，剩下的为下半部放到软中断中处理
 					
*/
#include "../../../include/DhGlobalHead.h"

#define BLE_LINK_DEBUG

#if !defined(BLE_LINK_DEBUG)
#undef DEBUG_INFO
#define DEBUG_INFO(...)
#undef DEBUG_DATA
#define DEBUG_DATA(...)
#else
char *LINK_STATE_STRING[4] ={"ble link advertising", "ble link connected", "ble link standby"}; 
#endif

#define BLE_LINK_STATE_COUNT			(BLE_LINK_STATE_END)	/* 链路层状态个数*/
#define BLE_STACK_SOFTIRQ				SWI1_IRQn				/* 协议栈下半部处理用软中断实现 */
#define BLE_STACK_SOFTIRQ_HANDLER		SWI1_IRQHandler

typedef struct
{
	EnBleLinkState			m_enState;									/* 链路状态 */
	BleRadioEvtHandler		m_bleRadioEvtHandler[BLE_LINK_STATE_COUNT];	/* 每个链路状态的ble radio事件处理函数*/
}BlkBleLinkInfo;


BlkBleLinkInfo s_blkLinkInfo;
u1  s_u1HostPushFlag = 0;

/* 
	分离芯片实现的ble协议栈可以分为control 和 host 2部分，既由2颗芯片实现完整协议栈，通过hci通信
	control实现link和control-hci
	host实现来l2cap,att,sm, host-hci
	这里是在一个芯片上实现完整协议栈所以没有明确的control和host区分，也没有hci
*/
/* 创建host->link 和link->host2个方向的队列 */
#define BLE_DATA_QUEUQ_SIZE				(16)        /* 必须是2的n次幂 */

/* 链路收到的数据除了空包，和一些需要特殊处理的数据包都丢到LINK_TO_HOST队列去，再后续软中断再处理，一些link control命令也是当做host数据丢进去*/
CREATE_QUEUE_INSTANCE(BLE_LINK_TO_HOST_DATA_QUEUE, BlkLinkToHostData, BLE_DATA_QUEUQ_SIZE);

/* 上层应用的发送是异步的，数据都是丢到这个队列中，和每个ble同步事件中会从队列中取出来再实际发送 */
CREATE_QUEUE_INSTANCE(BLE_HOST_TO_LINK_DATA_QUEUE, BlkHostToLinkData, BLE_DATA_QUEUQ_SIZE);


static void LinkRadioEvtHandler(EnBleRadioEvt evt)
{
	EnBleLinkState state = s_blkLinkInfo.m_enState;				// 获取当前链路状态
	
	switch(evt)
	{
		case BLE_RADIO_EVT_TRANS_DONE:
		if( NULL!=s_blkLinkInfo.m_bleRadioEvtHandler[state] && BLE_LINK_STANDBY!=state )
		{
			s_blkLinkInfo.m_bleRadioEvtHandler[state](evt);			// 调用当前链路状态的ble radio事件处理函数
		}
	}
}

static void PreConningStateHandle(EnBleRadioEvt evt)
{
    BleLinkStateSwitch(BLE_LINK_CONNECTED);	    // 链路状态切换到连接态。
	LinkConnSubStateSwitch(CONN_CONNING_RX);	// 切换连接连接子状态到连接中状态。
}

static u1 IsHostPushing(void)
{
    return s_u1HostPushFlag;
}

static void HostPushingFlagCfg(void)
{
    s_u1HostPushFlag = 1;
}

static void HostPushingFlagClr(void)
{
    s_u1HostPushFlag = 0;
}

/**
 *@brief: 		BleLinkInit
 *@details:		Ble链路初始化
 				初始化ble radio资源。注册ble radio事件处理函数
 *@retval:		void
 */
void BleLinkInit(void)
{
	u1 i;
	BleRadioInit();
	BleRadioEvtHandlerReg(LinkRadioEvtHandler);				// 注册ble radio事件处理函数
	memset(&s_blkLinkInfo, 0x00, sizeof(s_blkLinkInfo));

	BleLPowerTimerInit(LP_CLK_SRC_RC_250_PPM, 1000);						// 初始化低功耗定时器，使用内部RC为时钟源,1s校准一次
	BleHAccuracyTimerInit();
	for( i = 0; i < BLE_LINK_STATE_END; i++ )
	{
		s_blkLinkInfo.m_bleRadioEvtHandler[i] = NULL;
	}
	s_blkLinkInfo.m_enState = BLE_LINK_STANDBY;

	LinkAdvStateInit();	//链路广播状态相关初始化
	LinkConnStateInit(BLE_SCA_250_PPM);

    BleLinkStateHandlerReg(BLE_LINK_PRE_CONNING, PreConningStateHandle);
	
	/* 使能协议栈下半部处理的软中断 */
	NVIC_ClearPendingIRQ(BLE_STACK_SOFTIRQ);
	NVIC_SetPriority(BLE_STACK_SOFTIRQ, DH_IRQ_PRIORITY_3);
	NVIC_EnableIRQ(BLE_STACK_SOFTIRQ);
	
}

u4 NotifyHostDisconn(u1 reason)
{
    BlkBleEvent bleEvent;
    
    bleEvent.m_u2EvtType = BLE_EVENT_DISCONNECTED;
    bleEvent.m_event.m_blkDisconnInfo.m_u1ErrCode = reason;   // 表示超时断开
    return BleEventPush( bleEvent );
}

/**
 *@brief: 		BleLinkReset
 *@details:		复位链路状态

 *@retval:		void
 */
void BleLinkReset(void)
{
    /* 关闭协议栈的定时器 */
    LinkAdvStateReset();
    LinkConnStateReset();
    LinkConnStateInit(BLE_SCA_250_PPM);
    BleLinkStateSwitch(BLE_LINK_STANDBY);
}

void BleDisconnCommHandle(u1 DisconnReason)
{
    BleLinkReset();
    NotifyHostDisconn(DisconnReason);
    BleSecurityManagerInit();
}

/**
 *@brief: 		BleLinkStateHandlerReg
 *@details:		链路层各状态的ble radio处理函数
 *@param[in]	state   		链路状态        
 				evtHandler  	radio事件处理函数

 *@retval:		void
 */
void BleLinkStateHandlerReg(EnBleLinkState state, BleRadioEvtHandler evtHandler)
{
	s_blkLinkInfo.m_bleRadioEvtHandler[state] = evtHandler;
}


/**
 *@brief: 		BleLinkStateSwitch
 *@details:		链路状态切换
 *@param[in]	state		切换的状态

 *@retval:		void
 */
void BleLinkStateSwitch(EnBleLinkState state)
{
	s_blkLinkInfo.m_enState = state;
}




/**
 *@brief: 		BleHostDataToLinkPush
 *@details:		应用层的数据放到队列中，合适的时候链路层再发出去
                ！！当前链路控制的一些指令处理也是通过该接口发送。
 *@param[in]	blkSendData  

 *@retval:		DH_SUCCESS
 */
u4 BleHostDataToLinkPush(BlkHostToLinkData blkSendData)
{
	BlkHostToLinkData *pblkData;
    u1  llid;
	u1  pu1SK[BLE_ENC_KEY_SIZE];
	u2  u2EncLen;
	
	if( BLE_LINK_CONNECTED != s_blkLinkInfo.m_enState )
	{
        return ERR_LINK_STATE_INVALID;
	}
	/* 这里只是从队列中取出一个元素，但实际赋值操作在后面，所以有一个时间差，可能刚取出元素，还没赋值,ble事件来了判断队列有数据
	    于是将没赋值的队列元素数据发送出去了*/
	HostPushingFlagCfg();   
	pblkData = (BlkHostToLinkData *)QueueEmptyElemGet(BLE_HOST_TO_LINK_DATA_QUEUE, sizeof(BlkHostToLinkData));
	if( NULL == pblkData )
	{
		return ERR_DH_QUEUE_FULL;
	}
	llid = blkSendData.m_u1PacketFlag;
	if ( LINK_ENCRYPTED==LinkEncKeyGet(pu1SK) ) /* 链路加密 */
	{
	    DEBUG_INFO("send packet count:%d",GetSendPacketCounter());
	    u2EncLen = BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH;    //加密后的最长数据
	    LinkDataEnc(llid, 0, pu1SK, blkSendData.m_pu1HostData, blkSendData.m_u2Length, pblkData->m_pu1HostData, &u2EncLen);
	    pblkData->m_u2Length = u2EncLen;
	    UpdateSendPacketCounter();
	}
	else
	{  
	    /*判断从什么时候开始需要加密--临时处理方式，后面看看有没有别的更好的方式*/
	    if(blkSendData.m_u1PacketFlag==CONTROL_PACKET)
    	{  
            if(blkSendData.m_pu1HostData[0] == 0x05)//LL_START_ENC_REQ)
            {
                DEBUG_INFO("start encrypt");
                LinkEncStartFlagCfg(LINK_DATA_ENC_FLAG);
                BleSecurityStatusValid(SECURITY_STATUS_VALID);
            }
    	}
    	memcpy(pblkData->m_pu1HostData, blkSendData.m_pu1HostData, BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH);
    	pblkData->m_u2Length = blkSendData.m_u2Length;
    }
    pblkData->m_u1PacketFlag = blkSendData.m_u1PacketFlag;
    HostPushingFlagClr();
	return DH_SUCCESS;
}

/**
 *@brief: 		BleHostDataToLinkPop
 *@details:		提取应用层待发送的数据，通过链路层发送出去
 *@param[in]	&pblkData  
 
 *@retval:		DH_SUCCESS
 */
u4 BleHostDataToLinkPop(BlkHostToLinkData *pblkData)
{
	BlkHostToLinkData *tmp;

	if( NULL == pblkData )
	{
		return ERR_LINK_INVALID_PARAMS;
	}
	tmp = (BlkHostToLinkData*)QueueValidElemGet(BLE_HOST_TO_LINK_DATA_QUEUE, sizeof(BlkHostToLinkData));
	if( (NULL==tmp) || IsHostPushing() )
	{
		return ERR_DH_QEUEUE_EMPTY;
	}
	memcpy(pblkData->m_pu1HostData, tmp->m_pu1HostData, BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH);
	pblkData->m_u2Length = tmp->m_u2Length;
	pblkData->m_u1PacketFlag = tmp->m_u1PacketFlag;
	QueuePop(BLE_HOST_TO_LINK_DATA_QUEUE);

	return DH_SUCCESS;
}

/**
 *@brief: 		BleLinkDataToHostPush
 *@details:		链路层收到的数据放入队列中，通过软中断通知到host层
 *@param[in]	blkSendData  

 *@retval:		DH_SUCCESS
 */
u4 BleLinkDataToHostPush(BlkLinkToHostData blkSendData)
{
	BlkLinkToHostData *pblkData;
	
	pblkData = (BlkLinkToHostData *)QueueEmptyElemGet(BLE_LINK_TO_HOST_DATA_QUEUE, sizeof(BlkLinkToHostData));
	if( NULL == pblkData)
	{
		return ERR_DH_QUEUE_FULL;
	}
	
	memcpy(pblkData->m_pu1LinkData, blkSendData.m_pu1LinkData, BLE_PDU_LENGTH);

	/* 触发软中断去处理 */
	NVIC_SetPendingIRQ(BLE_STACK_SOFTIRQ);

	return DH_SUCCESS;
}

void BLE_STACK_SOFTIRQ_HANDLER(void)
{
	u1	llid;
	u2	len,u2DecLen;
	u1  pu1SK[BLE_ENC_KEY_SIZE];
	BlkBlePduHeader *pHeader;
	BlkLinkToHostData *pData;
	do{
		pData = (BlkLinkToHostData *)QueueValidElemGet(BLE_LINK_TO_HOST_DATA_QUEUE, sizeof(BlkLinkToHostData));
		if( NULL != pData )
		{
			pHeader = (BlkBlePduHeader *)pData->m_pu1LinkData;
			llid = pHeader->m_u1Header1&0x03;
			len = pHeader->m_u1Header2&0x1F;

			if ( LLID_CONTROL==llid  && len>0 ) /* 链路控制在接收的时候已经解密了 */
			{
			    DEBUG_INFO("link control:%d",pData->m_pu1LinkData[2]);
				BleLinkControlHandle(pData->m_pu1LinkData+BLE_PDU_HEADER_LENGTH, len);
			}
			else if( (LLID_CONTINUATION==llid || LLID_START==llid) && len>0 )
			{
    			if ( LINK_ENCRYPTED==LinkEncKeyGet(pu1SK) )   /* 链路加密了 */
    			{
    			    u2DecLen = BLE_PDU_LENGTH;
    			    DEBUG_INFO("recv packet count:%d",GetRecvPacketCounter());
    			    if( ERR_LINK_MIC_FAILED==LinkDataDec(llid, 1, pu1SK, pData->m_pu1LinkData+BLE_PDU_HEADER_LENGTH, len, pData->m_pu1LinkData+BLE_PDU_HEADER_LENGTH, &u2DecLen) )
    			    {
                        /* MIC错误就断开 */
                        BleDisconnCommHandle(LINK_DISCONN_REASON_MIC_FAILED);
                        return ;
                    }
                    else
                    {
                        UpdateRecvPacketCounter();  /* 每解密一包数据增加计数 */
                    }
                    len = u2DecLen;
    			}
				BleL2capHandle(pData->m_pu1LinkData+BLE_PDU_HEADER_LENGTH, len);
			}
			/* 数据处理完要出队 */
			QueuePop(BLE_LINK_TO_HOST_DATA_QUEUE);
		}
	}while( NULL != pData );
}

void BleLinkPeerAddrInfoGet(BlkBleAddrInfo *addr)
{
    u1  tmp[BLE_ADDR_LEN+1];
    LinkPeerAddrInfo(tmp, BLE_ADDR_LEN+1);

    addr->m_u1AddrType = tmp[0];
    memcpy(addr->m_pu1Addr,tmp+1, BLE_ADDR_LEN);
}

