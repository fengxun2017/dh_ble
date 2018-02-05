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

#ifndef __BLELINK_H__
#define __BLELINK_H__
#include "../../DhGlobalHead.h"

#define BLE_SCA_20_PPM				(7)
#define BLE_SCA_30_PPM				(6)
#define BLE_SCA_50_PPM				(5)
#define BLE_SCA_75_PPM				(4)
#define BLE_SCA_100_PPM				(3)
#define BLE_SCA_150_PPM				(2)
#define BLE_SCA_250_PPM				(1)
#define BLE_SCA_500_PPM				(0)
#define BLE_SCA_GRADE_NUMBER		(8)

#define LLID_START					(0x02)
#define LLID_CONTINUATION			(0x01)
#define LLID_EMPTY_PACKET			(0x01)			// 空包的LLID需要设置成0x01
#define LLID_CONTROL				(0x03)

#define CONTROL_PACKET              (0x01)
#define DATA_PACKET                 (0x00)
typedef struct
{
	u1	m_u1Header1;
	u1	m_u1Header2;
}BlkBlePduHeader;	/* PDU header有2字节*/

/*
        
******link层以上都当做host，部分link control命令可以延迟处理则也当做host层数据********
*/


/* 链路中断只会处理紧急事件，其他事情通过将数据延后到下半部处理，部分链路控制命令也是当做host层数据推送到下半部*/
typedef struct 
{
	u1	m_pu1LinkData[BLE_PDU_LENGTH];
}BlkLinkToHostData;

/* host层数据推送到一个数据队列中，链路层进入连接事件后再取出数据，部分链路控制返回也当做host层数据通过这个数据结构放到数据队列中 */
typedef struct
{
	u1 m_u1Length;					// buff中数据长度
	u1 m_u1PacketFlag;
	u1 m_pu1HostData[BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH];
}BlkHostToLinkData;



typedef enum
{
	BLE_LINK_ADVERTISING = 0,		/* 广播中 */
	BLE_LINK_CONNECTED,
	BLE_LINK_STANDBY,				/* 空闲待命状态*/
	
	BLE_LINK_STATE_END,
}EnBleLinkState;	/* 链路状态*/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void BleLinkInit(void);
extern void BleLinkReset(void);

extern void BleLinkStateHandlerReg(EnBleLinkState state, BleRadioEvtHandler evtHandler);
extern void BleLinkStateSwitch(EnBleLinkState state);
extern u4 BleHostDataToLinkPop(BlkHostToLinkData *pblkData);
extern u4 BleHostDataToLinkPush(BlkHostToLinkData blkData);
extern u4 BleLinkDataToHostPush(BlkLinkToHostData blkData);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLELINK_H__ */
