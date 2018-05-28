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

#define	BLE_EVENT_NTF_IRQ					SWI0_IRQn
#define BLE_EVENT_NTF_IRQ_HANDLER			SWI0_IRQHandler
static BleEventHandler s_bleEventHandler = NULL;

#define BLE_EVENT_QUEUE_SIZE		(16)    /* 必须是2的n次幂*/

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

	/* 触发中断,执行后通知上层动作   */
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
