
#ifndef __DHBUFFMANAGE_H__
#define __DHBUFFMANAGE_H__

typedef struct
{
    u4  m_u4CurrentIndex;   // 当前可以使用的缓存的起始地址
    u4  m_u4BuffSize;       // 可供申请缓存总量
    u1  *m_pu1Buff;
}BlkDhMemoryManage;

#define CREATE_MEMORY_INSTANCE(BUFF_NAME, BUFF_SIZE)			\
	static u1 BUFF_NAME##_BUFF[BUFF_SIZE];							\
	static BlkDhMemoryManage  BUFF_NAME##INSTANCE = {0, BUFF_SIZE, BUFF_NAME##BUFF}\
	static BlkDhMemoryManage *BUFF_NAME = &BUFF_NAME##INSTANCE


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern u1 *DhMemoryAlloc(BlkDhMemoryManage *pblkMemory, u4 size);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DHBUFFMANAGE_H__ */
