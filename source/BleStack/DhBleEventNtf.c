/**
 * @file			DhBleEventNtf.c
 * @brief			实现协议栈事件往上层的通知，使用软中断实现
 * @author			fengxun
 * @date			2017年12月26日
*/
#include "../../include/DhGlobalHead.h"

#define	BLE_EVENT_NTF_IRQ					SWI0_IRQn
#define BLE_EVENT_NTF_IRQ_HANDLER			SWI0_IRQHandler
static BleEventHandler s_bleEventHandler = NULL;

#define BLE_EVENT_QUEUE_SIZE		(8)

/* 创建事件通知队列 */
CREATE_QUEUE_INSTANCE(BLE_EVENT_NTF_QUEUE, BlkBleEvent, BLE_EVENT_QUEUE_SIZE);

void DhBleEventNtfInit(void)
{

	// 事件通知使用最低优先级
	NVIC_ClearPendingIRQ(BLE_EVENT_NTF_IRQ);
	NVIC_SetPriority(BLE_EVENT_NTF_IRQ, DH_IRQ_PRIORITY_3);
	NVIC_EnableIRQ(BLE_EVENT_NTF_IRQ);
}

void DhBleEventHandlerReg(BleEventHandler handler)
{
	s_bleEventHandler = handler;
}


/**
 *@brief: 		BleEventPush
 *@details:		BLE事件入队，通知上层应用
 *@param[in]	BlkBleEvent event  
 *@retval:		
 */
u4 BleEventPush(BlkBleEvent event)
{
	BlkBleEvent *pBlkEvent;
	pBlkEvent = (BlkBleEvent *)QueueEmptyElemGet(BLE_EVENT_NTF_QUEUE, sizeof(BlkBleEvent));
	if( NULL == pBlkEvent)
	{
		return ERR_DH_QUEUE_FULL;
	}
	*pBlkEvent = event;

	/* 触发中断,执行后续通知上层动作   */
	NVIC_SetPendingIRQ(BLE_EVENT_NTF_IRQ);

	return DH_SUCCESS;
}

void BLE_EVENT_NTF_IRQ_HANDLER(void)
{
	static BlkBleEvent	bleEvent;
	BlkBleEvent	*pBleEvent;

	do{
		/* 依次提取队列中的事件调用上层回调 
			对 BLE_EVENT_NTF_QUEUE队列的出队操作只在这里
			没有临界区问题
		*/
		pBleEvent = (BlkBleEvent *)QueueValidElemGet(BLE_EVENT_NTF_QUEUE, sizeof(BlkBleEvent));
		if( NULL != pBleEvent)
		{
			bleEvent = *pBleEvent;
			QueuePop(BLE_EVENT_NTF_QUEUE);
			if( NULL != s_bleEventHandler )
			{
			    s_bleEventHandler(&bleEvent);
			}
		}
	}while(NULL != pBleEvent );
}
