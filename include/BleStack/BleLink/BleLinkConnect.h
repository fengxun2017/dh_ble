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

#ifndef __BLELINKCONNECT_H__
#define __BLELINKCONNECT_H__
#include "../../DhGlobalHead.h"

typedef struct
{
    u1  m_u1PeerBleAddrType;
    u1  m_pu1PeerBleAddr[BLE_ADDR_LEN];     /* 对端设备地址，LSB */
    u2  m_u2ConnInterval;                   /* 1.25ms为单位 */
    u2  m_u2SlaveLatency;
    u2  m_u2ConnTimeout;                    /* 10ms为单位 */
}BlkBleConnectedEvent;

typedef struct
{
    u2  m_u2ConnInterval;
    u2  m_u2SlaveLatency;
    u2  m_u2ConnTimeout;
}BlkBleConnUpdateEvent;

typedef struct
{
    u1  m_u1ErrCode;
}BlkBleDisconnectedEvent;



#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void LinkConnReqHandle(u1 addrType, u1 *pu1Addr, u1* pu1LLData);
extern void LinkConnStateInit(u1 sca);
void LinkConnStateReset(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLELINKCONNECT_H__ */
