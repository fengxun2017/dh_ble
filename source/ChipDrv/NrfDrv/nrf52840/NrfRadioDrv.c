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

#include "../../../../include/DhGlobalHead.h"

#define SHORT_READ_START			(1<<0)
#define SHORT_END_DISABLE			(1<<1)
#define SHORT_DISABLED_TXEN         (1<<2)
#define SHORT_DISABLED_RXEN         (1<<3)

#define INTEN_DISABLED				(1<<4)

#define RADIO_MODE_1MBLE			3


#define PCNF0_LFLEN_8BIT			(8)		
#define PCNF0_S0LEN_1BYTE			(1<<8)
#define PCNF0_S1LEN_0BIT			(0<<16)

#define PCNF1_MAXLEN_VALUE			(255)	//52840的 radio包结构中的payload最大为 255
#define PCNF1_STATLEN_0				(0<<8)
#define PCNF1_BALEN_3				(3<<16)
#define PCNF1_ENDIA_LITTLE			(0<<24)
#define PCNF1_WHITEEN_ENABLE		(1<<25)

#define PREFIX0_AP0_POS				(0)
#define PREFIX0_AP1_POS				(8)
#define PREFIX0_AP2_POS				(16)
#define PREFIX0_AP3_POS				(24)
#define PREFIX1_AP4_POS				(0)
#define PREFIX1_AP5_POS				(8)
#define PREFIX1_AP6_POS				(16)
#define PREFIX1_AP7_POS				(24)

#define LOGIC_ADDR_0				(0)
#define LOGIC_ADDR_1				(1)
#define LOGIC_ADDR_2				(2)
#define LOGIC_ADDR_3				(3)
#define LOGIC_ADDR_4				(4)
#define LOGIC_ADDR_5				(5)
#define LOGIC_ADDR_6				(6)
#define LOGIC_ADDR_7				(7)



#define CRCCNF_LEN_3				(3<<0)
#define CRCCNF_SKIPADDR				(1<<8)
#define CRCPOLY_BLE_POLY			BLE_CRC_POLY

#define TIFS_VALUE					(BLE_TIFS_VALUE)

#define SHORTS_COMM					(SHORT_READ_START | SHORT_END_DISABLE)


static NrfRadioIntHandler		s_RadioIntHandler = NULL;

/**
 *@brief: 		设置发射功率
 *@param[in]	s1TxPower 发射功率	
 *@retval:		void
 */
void NrfRadioTxPowerSet(s1 s1TxPower)
{
	NRF_RADIO->TXPOWER = s1TxPower;
}

/**
 *@brief: 		NrfRadioInit
 *@details:		nrf radio作为ble模式初始化
 *@param[in]	void  

 *@retval:		void
 */
void NrfRadioInit(void)
{

	// 收到 ready(TxIdle/RxIdle)信号后，自动设置 start
	// 收到 end（Tx/Rx） 信号后，自动设置 disable
	NRF_RADIO->SHORTS = SHORTS_COMM;

	// 只使能 disable 中断，表明发送/接收完成
	NRF_RADIO->INTENCLR = 0xFFFFFFFF;
	NRF_RADIO->INTENSET = INTEN_DISABLED;

	// 设定死只使用 ble 的 1M/s 发送速率
	NRF_RADIO->MODE = RADIO_MODE_1MBLE;

	// 只使用 1M/s 速率，因此 preamble 为 1字节
	// 52840包结构中的 S0，LENGTH，S1 共同构成了BLE 广播包/数据包 PDU 中的 header
	// 目前只考虑广播通道/数据通道PDU中的header为2 字节的情况，
	// 不考虑数据通道header为3字节（使用CTEInfo，与AOA，AOD有关）的情况，所以nordic包结构中的S1长度配置为0
	// 不考虑为长距离通信引入的 LE CODE PHY，所以nordic包结构中的CI，TERM 的长度都配置为0
	NRF_RADIO->PCNF0 = PCNF0_LFLEN_8BIT |PCNF0_S0LEN_1BYTE |PCNF0_S1LEN_0BIT;
	
	// BLE的Access-Address 为4字节，在Nordic 的包结构中，由 BASE 和 PREFIX（1字节） 组成，所以 BASE设置为3字节
	// 52840的包结构中的payload最大长度为255
	// 52840的包结构中的S0, LENGTH,S1和PAYLOAD 共同组成了 BLE广播/数据 PDU，这部分数据的空中序是LSB
	// 使能数据白化
	NRF_RADIO->PCNF1 = PCNF1_BALEN_3 | PCNF1_MAXLEN_VALUE | PCNF1_STATLEN_0 | PCNF1_ENDIA_LITTLE | PCNF1_WHITEEN_ENABLE;
	
	// 3字节crc，BLE 只计算了 广播/数据包的PDU部分，所以这里配置跳过address部分
	NRF_RADIO->CRCCNF = CRCCNF_LEN_3 | CRCCNF_SKIPADDR;
	// BLE CRC的生成多项式为x24 + x10 + x9 + x6 + x4 + x3 + x + 1
	// 换成二进制 1 0000 0000 0000 0110 0101 1011 -> 0x100065B
	NRF_RADIO->CRCPOLY = CRCPOLY_BLE_POLY;

	// 帧间间隔控制（两个连续数据包间隔），52840提供了专用寄存器用来控制帧间间隔。
	// BLE 要求两个连续包，上一个包的最后一个bit，和下一个包的第一个bit 之间间隔为 150us
	// 通过配置这个 TIFS寄存器为150，可以让硬件帮我们自动控制在收到一个数据包后，至少延迟150us再将之前配置好的数据发送出去。
	NRF_RADIO->TIFS = TIFS_VALUE;

	// 清空地址信息
	NRF_RADIO->BASE0 = 0x00;
	NRF_RADIO->BASE1 = 0x00;
	NRF_RADIO->PREFIX0 = 0x00;
	NRF_RADIO->PREFIX1 = 0x00;
	
	// 配置 radio 中断为最高优先级
	NVIC_SetPriority(RADIO_IRQn, 0);	
	NVIC_ClearPendingIRQ(RADIO_IRQn);
	NVIC_EnableIRQ(RADIO_IRQn);	
}

/**
 *@brief: 		NrfLogicAddrCfg
 *@details:		配置nrf radio的地址，即ble协议中的Access Address
 *@param[in]	u1LogAddr	逻辑地址 0-7  
 				u4PhyAddr	物理接入地址  
 *@retval:		void
 */
void NrfLogicAddrCfg(u1 u1LogicAddr, u4 u4PhyAddr)
{
	u4 u4BaseAddr;		// 接入地址的低三字节
	u1 u1Prefix;		// 接入地址的高字节

	// nordic的BALEN设置小于4的话，base寄存器会丢掉低字节
	u4BaseAddr = (u4PhyAddr<<8)&0xFFFFFF00;
	u1Prefix = (u4PhyAddr>>24)&0xff;
	if ( LOGIC_ADDR_0 == u1LogicAddr )
	{
		NRF_RADIO->BASE0 = u4BaseAddr;
		NRF_RADIO->PREFIX0 &= ~0x000000FF;
		NRF_RADIO->PREFIX0 |= (u1Prefix<<PREFIX0_AP0_POS);
	}
	else if( u1LogicAddr <= LOGIC_ADDR_7 )
	{
		NRF_RADIO->BASE1 = u4BaseAddr;
		switch(u1LogicAddr)
		{
			case LOGIC_ADDR_1:
				NRF_RADIO->PREFIX0 &= ~0x0000FF00;
				NRF_RADIO->PREFIX0 |= (u1Prefix<<PREFIX0_AP1_POS);
			break;

			case LOGIC_ADDR_2:
				NRF_RADIO->PREFIX0 &= ~0x00FF0000;
				NRF_RADIO->PREFIX0 |= (u1Prefix<<PREFIX0_AP2_POS);
			break;
			
			case LOGIC_ADDR_3:
				NRF_RADIO->PREFIX0 &= ~0xFF000000;
				NRF_RADIO->PREFIX0 |= (u1Prefix<<PREFIX0_AP3_POS);
			break;
			
			case LOGIC_ADDR_4:
				NRF_RADIO->PREFIX1 &= ~0x000000FF;
				NRF_RADIO->PREFIX1 |= (u1Prefix<<PREFIX1_AP4_POS);
			break;
			
			case LOGIC_ADDR_5:
				NRF_RADIO->PREFIX1 &= ~0x0000FF00;
				NRF_RADIO->PREFIX1 |= (u1Prefix<<PREFIX1_AP5_POS);
			break;
			
			case LOGIC_ADDR_6:
				NRF_RADIO->PREFIX1 &= ~0x00FF0000;
				NRF_RADIO->PREFIX1 |= (u1Prefix<<PREFIX1_AP6_POS);
			break;
			
			case LOGIC_ADDR_7:
				NRF_RADIO->PREFIX1 &= ~0xFF000000;
				NRF_RADIO->PREFIX1 |= (u1Prefix<<PREFIX1_AP7_POS);
			break;
		}
	}
}

void NrfRadioHandlerReg(NrfRadioIntHandler intHandler)
{
	s_RadioIntHandler = intHandler;
}

void RADIO_IRQHandler(void)
{
	if( NRF_RADIO->EVENTS_DISABLED && (NRF_RADIO->INTENSET & INTEN_DISABLED))
	{
		 NRF_RADIO->EVENTS_DISABLED = 0;
		if ( NULL != s_RadioIntHandler )
		{
			s_RadioIntHandler(NRF_RADIO_EVT_DISABLED);
		}
	}
}
