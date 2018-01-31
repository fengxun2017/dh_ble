/**
 * @file				DhQueue.h
 * @brief			DhQueue.c 的头文件
 * @author			fengxun
 * @date			2017年15月5日
*/
#ifndef __DHQUEUE_H__
#define __DHQUEUE_H__
#include "../DhGlobalHead.h"
typedef struct 
{
	void	*m_pValue;			// 指向实际数据
	u2		m_u2QueueSize;		// 队列大小,需要为2^n
	u2		m_u2ElemCount;		// 当年队列元素个数
	u2		m_u2IRead;			// 读指针
	u2		m_u2IWrite;			// 写指针
}BlkDhQueue;

/* 
        队列创建
    !!!!!!!!!!!!!   队列大小需要时2^n         !!!!!!!!!!!!!!!!!!
*/
#define CREATE_QUEUE_INSTANCE(QUEUE_NAME, QUEUE_ELEM_TYPE, QUEUE_SIZE)			\
	static QUEUE_ELEM_TYPE QUEUE_NAME##_BUFF[QUEUE_SIZE];							\
	static BlkDhQueue QUEUE_NAME##_queue = {QUEUE_NAME##_BUFF,QUEUE_SIZE,0,0,0};	\
	static BlkDhQueue *QUEUE_NAME = &QUEUE_NAME##_queue




#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void *QueueEmptyElemGet( BlkDhQueue *queue ,u4 u4ElemSize);
extern void *QueueValidElemGet(BlkDhQueue *queue, u4 u4ElemSize);
extern u1	IsQueueEmpty( BlkDhQueue *queue );
extern u1 IsQueueFull( BlkDhQueue *queue );
extern u4 QueueInit( BlkDhQueue * queue, u2 size, void *elemBuff );
extern void QueuePop(BlkDhQueue *queue);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DHQUEUE_H__ */
