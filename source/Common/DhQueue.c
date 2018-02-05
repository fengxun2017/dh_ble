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
/**
 *@brief: 		DhQueueInit
 *@details:		队列初始化
 *@param[in]	queue
 *@param[in]	size		队列大小
 *@param[in]	elemBuff	实际存放队列元素的buff

 *@retval:		void
 */
u4 QueueInit( BlkDhQueue * queue, u2 size, void *elemBuff )
{
	if( size!=0 && (size&(size-1))==0 )
	{
		return ERR_DH_QUEUE_SIZE_INVALID;
	}
    queue->m_pValue = elemBuff;
    queue->m_u2IRead = 0;
    queue->m_u2IWrite = 0;
    queue->m_u2QueueSize = size;
    queue->m_u2ElemCount = 0;

    return DH_SUCCESS;
}


/**
 *@brief: 		IsQueueEmpty
 *@details:		队列是否为空
 *@param[in]	queue  
 *@retval:		1-为空	0-非空
 */
u1	IsQueueEmpty( BlkDhQueue *queue )
{
	if ( 0 == queue->m_u2ElemCount )
	{
		return	1;
	}

	return	0;
}

u1 IsQueueFull( BlkDhQueue *queue )
{
	if ( queue->m_u2ElemCount == queue->m_u2QueueSize )
	{
		return	1;
	}

	return 0;
}

/**
 *@brief: 		QueueEmptyElemGet
 *@details:		获取队列中的一个无效内容元素，队里现有元素加1,赋值在外部进行。
 				等于将入队操作的赋值操作提取到外部进行，先入队实际的队列元素赋值在外部
 *@param[in]	queue  
 *@param[in]	u4ElemSize  队列中的元素大小

 *@retval:		pvalue		内容为空的元素指针
 */
void *QueueEmptyElemGet( BlkDhQueue *queue ,u4 u4ElemSize)
{
	void *pValue;
	if( IsQueueFull(queue) )
	{
		/* 队列已满，没有空元素了 */
		return NULL;
	}

	/* 
		判断队列满不放在临界区里了，就算放到临界区buff真放满了，再有新数据也是丢弃或者覆盖队头数据 
		这里放在临界区外面出现冲突的时候也就是覆盖掉队列头的数据而已。
	*/
	CRITICAL_REGION_ENTER();
	
	pValue = (void *)((u4)(queue->m_pValue)+u4ElemSize*(queue->m_u2IWrite));
	queue->m_u2IWrite = (queue->m_u2IWrite+1)&(queue->m_u2QueueSize-1);	// 先做入队操作了
	queue->m_u2ElemCount++;
	
	CRITICAL_REGION_EXIT();

	return pValue;
}

/**
 *@brief: 		QueueValidElemGet
 *@details:		获取队列头元素，没有出队.该函数之后需要调用QueuePop执行出队操作。
 				将队列元素的提取和出队操作分离是因为这里提取获取到的只是元素的指针，外部还需要获取元素的实际内容
 				如果这里就直接出队，如果这个时候有很多入队操作，会导致覆盖点当前刚出队的这个元素的内容
 				提取数据和出队操纵分开，可能出现多个地方出队操纵提取的是同一个数据。
 				不过现在队列是协议栈内部使用的。对同一个队列的出队操纵都是在同一个地方，不会出现多线程的问题。
 *@param[in]	queue  
 *@param[in]	u4ElemSize  队列中的元素大小
 
 *@retval:		队列头元素
 */
void *QueueValidElemGet(BlkDhQueue *queue, u4 u4ElemSize)
{
	void *pValue;
	
	if( IsQueueEmpty(queue) )
	{
		return NULL;
	}
	pValue = (void *)((u4)(queue->m_pValue)+u4ElemSize*(queue->m_u2IRead));
	
	return pValue;
}

/**
 *@brief: 		QueuePop
 *@details:		出队操作，剔除队列头元素
 *@param[in]	queue  
 
 *@retval:		void
 */
void QueuePop(BlkDhQueue *queue)
{
	CRITICAL_REGION_ENTER();
	queue->m_u2IRead = (queue->m_u2IRead+1)&(queue->m_u2QueueSize-1);		// 出队操作
	queue->m_u2ElemCount--;
	CRITICAL_REGION_EXIT();

}

