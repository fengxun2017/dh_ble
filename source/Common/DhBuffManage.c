#include "../../include/DhGlobalHead.h"

/*
    实现简单的内存管理，只能静态申请，只能使用在申请后就不会释放的场景中使用，协议栈中需要该模块管理内存
*/

u1 *DhMemoryAlloc(BlkDhMemoryManage *pblkMemory, u4 size)
{
    u1  *pu1Buff = NULL;
    
    if( NULL == pblkMemory )
    {
        return pu1Buff;
    }
    if( (pblkMemory->m_u4BuffSize-pblkMemory->m_u4CurrentIndex) < size )
    {
        /* 内存不够申请的了 */
        return pu1Buff;
    }

    pu1Buff = &pblkMemory->m_pu1Buff[pblkMemory->m_u4CurrentIndex];
    pblkMemory->m_u4CurrentIndex += size;

    return pu1Buff;
}

