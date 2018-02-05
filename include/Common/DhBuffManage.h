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

#ifndef __DHBUFFMANAGE_H__
#define __DHBUFFMANAGE_H__
#include "../DhGlobalHead.h"

typedef struct
{
    u4  m_u4CurrentIndex;   // 当前可以使用的缓存的起始地址
    u4  m_u4BuffSize;       // 可供申请缓存总量
    u1  *m_pu1Buff;
}BlkDhMemoryManage;

#define CREATE_MEMORY_INSTANCE(BUFF_NAME, BUFF_SIZE)									\
	static u1 BUFF_NAME##_BUFF[BUFF_SIZE];												\
	static BlkDhMemoryManage  BUFF_NAME##_INSTANCE = {0, BUFF_SIZE, BUFF_NAME##_BUFF};	\
	static BlkDhMemoryManage *BUFF_NAME = &BUFF_NAME##_INSTANCE


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
