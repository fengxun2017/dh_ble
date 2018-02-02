/**
 * @file				BleLink.c
 * @brief			ble链路层状态机，Ble链路层分为广播态，连接中状态，连接上状态，空闲待命状态。
 					每个状态下又细分字状态。如广播态分：发送结束，接收结束，发送扫描响应结束，广播空闲。
 					协议栈的底层处理分为2部分处理，时序相关的在中断中，剩下的为下半部放到软中断中处理
 					
 * @author			fengxun
 * @date			2017年11月21日
*/
#include "../../include/DhGlobalHead.h"

#define nBLE_LINK_DEBUG

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


/* 
	分离芯片实现的ble协议栈可以分为control 和 host 2部分，既由2颗芯片实现完整协议栈，通过hci通信
	control实现link和control-hci
	host实现来l2cap,att,sm, host-hci
	这里是在一个芯片上实现完整协议栈所以没有明确的control和host区分，也没有hci
*/
/* 创建host->link 和link->host2个方向的队列 */
#define BLE_DATA_QUEUQ_SIZE				(8)
CREATE_QUEUE_INSTANCE(BLE_LINK_TO_HOST_DATA_QUEUE, BlkLinkToHostData, BLE_DATA_QUEUQ_SIZE);
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

	
	/* 使能协议栈下半部处理的软中断 */
	NVIC_ClearPendingIRQ(BLE_STACK_SOFTIRQ);
	NVIC_SetPriority(BLE_STACK_SOFTIRQ, DH_IRQ_PRIORITY_3);
	NVIC_EnableIRQ(BLE_STACK_SOFTIRQ);
	
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
    BleLinkStateSwitch(BLE_LINK_STANDBY);
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
 *@param[in]	blkData  

 *@retval:		DH_SUCCESS
 */
u4 BleHostDataToLinkPush(BlkHostToLinkData blkData)
{
	BlkHostToLinkData *pblkData;

	if( BLE_LINK_CONNECTED != s_blkLinkInfo.m_enState )
	{
        return ERR_LINK_STATE_INVALID;
	}
	pblkData = (BlkHostToLinkData *)QueueEmptyElemGet(BLE_HOST_TO_LINK_DATA_QUEUE, sizeof(BlkHostToLinkData));
	if( NULL == pblkData )
	{
		return ERR_DH_QUEUE_FULL;
	}
	memcpy(pblkData->m_pu1HostData, blkData.m_pu1HostData, BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH);
	pblkData->m_u1Length = blkData.m_u1Length;
    pblkData->m_u1PacketFlag = blkData.m_u1PacketFlag;
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
	if( NULL == tmp)
	{
		return ERR_DH_QEUEUE_EMPTY;
	}
	memcpy(pblkData->m_pu1HostData, tmp->m_pu1HostData, BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH);
	pblkData->m_u1Length = tmp->m_u1Length;
	pblkData->m_u1PacketFlag = tmp->m_u1PacketFlag;
	QueuePop(BLE_HOST_TO_LINK_DATA_QUEUE);

	return DH_SUCCESS;
}

/**
 *@brief: 		BleLinkDataToHostPush
 *@details:		链路层收到的数据放入队列中，通过软中断通知到host层
 *@param[in]	blkData  

 *@retval:		DH_SUCCESS
 */
u4 BleLinkDataToHostPush(BlkLinkToHostData blkData)
{
	BlkLinkToHostData *pblkData;
	
	pblkData = (BlkLinkToHostData *)QueueEmptyElemGet(BLE_LINK_TO_HOST_DATA_QUEUE, sizeof(BlkLinkToHostData));
	if( NULL == pblkData)
	{
		return ERR_DH_QUEUE_FULL;
	}
	
	memcpy(pblkData->m_pu1LinkData, blkData.m_pu1LinkData, BLE_PDU_LENGTH);

	/* 触发软中断去处理 */
	NVIC_SetPendingIRQ(BLE_STACK_SOFTIRQ);

	return DH_SUCCESS;
}

void BLE_STACK_SOFTIRQ_HANDLER(void)
{
	u1	llid;
	u2	len;
	BlkBlePduHeader *pHeader;
	BlkLinkToHostData *pData;
	DEBUG_INFO("soft irq!!!");
	do{
		pData = (BlkLinkToHostData *)QueueValidElemGet(BLE_LINK_TO_HOST_DATA_QUEUE, sizeof(BlkLinkToHostData));
		if( NULL != pData )
		{
			pHeader = (BlkBlePduHeader *)pData->m_pu1LinkData;
			llid = pHeader->m_u1Header1&0x03;
			len = pHeader->m_u1Header2&0x1F;
			
			if ( LLID_CONTROL==llid  && len>0 )
			{
			    DEBUG_INFO("link control");
				BleLinkControlHandle(pData->m_pu1LinkData+BLE_PDU_HEADER_LENGTH, len);
			}
			else if( (LLID_CONTINUATION==llid || LLID_START==llid) && len>0 )
			{
			    DEBUG_INFO("l2cap handle");
				BleL2capHandle(pData->m_pu1LinkData+BLE_PDU_HEADER_LENGTH, len);
			}
			/* 数据处理完要出队 */
			QueuePop(BLE_LINK_TO_HOST_DATA_QUEUE);
		}
	}while( NULL != pData );
}
