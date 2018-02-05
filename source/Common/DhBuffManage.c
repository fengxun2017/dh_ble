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

