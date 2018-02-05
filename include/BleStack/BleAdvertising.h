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

#ifndef __BLEADVERTISING_H__
#define __BLEADVERTISING_H__
#include "../DhGlobalHead.h"

typedef struct
{
	u1		m_u1AddrType;				/* 0-public  1-random */
	u1		m_pu1Addr[BLE_ADDR_LEN];	/* 设备地址,LSB     */
}BlkBleAddrInfo;

typedef struct 
{
	u1	m_ChannelOn_37:1;
	u1	m_ChannelOn_38:1;
	u1	m_ChannelOn_39:1;
}BlkAdvChannelOn;


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern u4 BleAdvDataCfg(u1 *pu1Data, u2 len);
extern u4 BleAdvStart(BlkAdvChannelOn channels, u2 IntervalMs);
extern u4 BleScanRspDataCfg(u1 *pu1Data, u2 len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLEADVERTISING_H__ */
