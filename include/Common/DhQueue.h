/**
 * @file				DhQueue.h
 * @brief			DhQueue.c ��ͷ�ļ�
 * @author			fengxun
 * @date			2017年12月25日
*/
#ifndef __DHQUEUE_H__
#define __DHQUEUE_H__

typedef struct 
{
	void	*m_pValue;			// 指向实际队列
	u2		m_u2QueueSize;		// 队列大小
	u2		m_u2ElemCount;		// 队列中当前存在的元素个数
	u2		m_u2IRead;			// 读下标
	u2		m_u2IWrite;			// 写下标
}BlkDhQueue;

/* 
	大小为2^n
*/
#define CREATE_QUEUE_INSTANCE(QUEUE_NAME, QUEUE_ELEM_TYPE, QUEUE_SIZE)			\
	static QUEUE_EMEM_TYPE QUEUE_ELEM_BUFF[QUEUE_SIZE];							\
	static BlkDhQueue QUEUE_NAME##_queue = {QUEUE_ELEM_BUFF,QUEUE_SIZE,0,0,0};	\
	static BlkDhQueue *QUEUE_NAME = &QUEUE_NAME##_queue




#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void *QueueEmptyElemGet( BlkDhQueue *queue );
extern void *QueueValidElemGet(BlkDhQueue *queue);
extern u1	IsQueueEmpty( BlkDhQueue *queue );
extern u1 IsQueueFull( BlkDhQueue queue );
extern u4 QueueInit( BlkDhQueue * queue, u2 size, void *elemBuff );
extern void QueuePop(BlkDhQueue queue);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DHQUEUE_H__ */
