/**
 * @file				RadioDrv.h
 * @brief			NrfRadioDrv.c 的头文件
 * @author			fengxun
 * @date			2017年10月18日
*/
#ifndef __RADIODRV_H__
#define __RADIODRV_H__
#include "../DhGlobalHead.h"

#define SHORT_READ_START			(1<<0)
#define SHORT_END_DISABLE			(1<<2)
#define SHORT_DISABLED_TXEN			(1<<3)
#define SHORT_ADDRESS_RSSISTART		(1<<4)
#define SHORT_END_START				(1<<5)
#define SHORT_ADDRESS_BCSTART		(1<<6)
#define SHORT_DISABLED_RSSISTOP		(1<<8)

#define INTEN_READY					(1<<0)
#define INTEN_ADDRESS				(1<<1)
#define INTEN_PAYLOAD				(1<<2)
#define INTEN_END					(1<<3)
#define INTEN_DISABLED				(1<<4)
#define INTEN_DEVMATCH				(1<<5)
#define INTEN_DEVMISS				(1<<6)
#define INTEN_RSSIEND				(1<<7)
#define INTEN_BCMATCH				(1<<10)

#define INTCLR_READY					(1<<0)
#define INTCLR_ADDRESS				(1<<1)
#define INTCLR_PAYLOAD				(1<<2)
#define INTCLR_END					(1<<3)
#define INTCLR_DISABLED				(1<<4)
#define INTCLR_DEVMATCH				(1<<5)
#define INTCLR_DEVMISS				(1<<6)
#define INTCLR_RSSIEND				(1<<7)
#define INTCLR_BCMATCH				(1<<10)

#define CRCSTATUS_ERR				0
#define CRCSTATUS_OK				1

#define TXPOWER_POS4				0x04
#define TXPOWER_0					0x00
#define TXPOWER_NEG4				0xFC
#define TXPOWER_NEG8				0xF8
#define TXPOWER_NEG12				0xF4
#define TXPOWER_NEG16				0xF0
#define TXPOWER_NEG20				0xEC
#define TXPOWER_NEG30				0xD8


#define RADIO_MODE_1MBLE			3


#define PCNF0_LFLEN_8BIT			(6)			// 4.0负载数据不会超过37字节，6bit就够了
#define PCNF0_S0LEN_1BYTE			(1<<8)
#define PCNF0_S1LEN_0BIT			(0<<16)

#define PCONF1_MAXLEN_VALUE			BLE_PDU_PAYLOAD
#define PCONF1_STATLEN_0			(0<<8)
#define PCONF1_BALEN_3				(3<<16)
#define PCONF1_ENDIA_LITTLE			(0<<24)
#define PCONF1_WHITEEN_ENABLE		(1<<25)

#define PREFIX0_AP0_POS				(0)
#define PREFIX0_AP1_POS				(8)
#define PREFIX0_AP2_POS				(16)
#define PREFIX0_AP3_POS				(24)

#define PREFIX1_AP4_POS				(0)
#define PREFIX1_AP5_POS				(8)
#define PREFIX1_AP6_POS				(16)
#define PREFIX1_AP7_POS				(24)

#define LOGIC_TX_ADDR_0				(0)
#define LOGIC_TX_ADDR_1				(1)
#define LOGIC_TX_ADDR_2				(2)
#define LOGIC_TX_ADDR_3				(3)
#define LOGIC_TX_ADDR_4				(4)
#define LOGIC_TX_ADDR_5				(5)
#define LOGIC_TX_ADDR_6				(6)
#define LOGIC_TX_ADDR_7				(7)

#define LOGIC_RX0_ENABLE			(1<<0)
#define LOGIC_RX1_ENABLE			(1<<1)
#define LOGIC_RX2_ENABLE			(1<<2)
#define LOGIC_RX3_ENABLE			(1<<3)
#define LOGIC_RX4_ENABLE			(1<<4)
#define LOGIC_RX5_ENABLE			(1<<5)
#define LOGIC_RX6_ENABLE			(1<<6)
#define LOGIC_RX7_ENABLE			(1<<7)

#define CRCCNF_LEN_3				(3<<0)
#define CRCCNF_SKIPADDR				(1<<8)
#define CRCPOLY_BLE_POLY			BLE_CRC_POLY

#define NRF_RADIO_ADDRESS           0x40001000UL







#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __RADIODRV_H__ */
