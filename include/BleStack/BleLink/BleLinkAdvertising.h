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

#ifndef __BLELINKADVERTISING_H__
#define __BLELINKADVERTISING_H__
#include "../../DhGlobalHead.h"

/* 广播通道下的帧类型 */
#define PDU_TYPE_ADV					(0x00)		/* 普通广播 */
#define PDU_TYPE_ADV_DIRECT				(0x01)		/* 直接广播 */
#define PDU_TYPE_ADV_NONCONN			(0x02)		/* 不可连接不可扫描广播 */
#define PDU_TYPE_SCAN_REQ				(0x03)		/* 扫描请求 */
	#define SCAN_REQ_SCANA_SIZE			(6)
	#define SCAN_REQ_ADVA_SIZE			(6)
#define PDU_TYPE_SCAN_RSP				(0x04)		/* 扫描响应 */
#define PDU_TYPE_CONNECT_REQ			(0x05)		/* 连接请求 */
#define PDU_TYPE_ADV_SCAN				(0x06)		/* 不可连接可扫描广播 */

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void LinkAdvAddrInfoCfg(BlkBleAddrInfo addr);
extern BlkBleAddrInfo LinkAdvAddrInfoGet(void);

extern u4   LinkAdvDataCfg(u1 *pu1Data, u2 u2Offset, u2	len);
extern void LinkAdvParamsCfg(BlkAdvChannelOn channels, u2 intervalMs);
extern u4   LinkAdvStart(void);
extern void LinkAdvStateInit(void);
extern void LinkAdvStateReset(void);

extern u4   LinkScanRspCfg(u1 *pu1Data, u2 u2Offset, u2	len);
extern u2   LinkScanRspLenGet(void);
extern u2   LinkAdvDataLenGet(void);

extern void BleLinkSelfAddrInfoGet(BlkBleAddrInfo *addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLELINKADVERTISING_H__ */
