#include "../../include/DhGlobalHead.h"

#define SHORT_READ_START			(1<<0)
#define SHORT_END_DISABLE			(1<<1)

#define INTEN_DISABLED				(1<<4)

#define RADIO_MODE_1MBLE			3


#define PCNF0_LFLEN_8BIT			(8)			// 4.0负载数据不会超过37字节，6bit就够了
#define PCNF0_S0LEN_1BYTE			(1<<8)
#define PCNF0_S1LEN_0BIT			(0<<16)

#define PCNF1_MAXLEN_VALUE			(BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH)	//51822的radio PDU中的前两字节不算在payload长度内
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
	NRF_RADIO->SHORTS = SHORTS_COMM;
	NRF_RADIO->INTENCLR = 0xFFFFFFFF;
	NRF_RADIO->INTENSET = INTEN_DISABLED;
	NRF_RADIO->MODE = RADIO_MODE_1MBLE;
	NRF_RADIO->PCNF0 = PCNF0_LFLEN_8BIT |PCNF0_S0LEN_1BYTE |PCNF0_S1LEN_0BIT;
	NRF_RADIO->PCNF1 = PCNF1_BALEN_3 | PCNF1_MAXLEN_VALUE | PCNF1_STATLEN_0 | PCNF1_ENDIA_LITTLE | PCNF1_WHITEEN_ENABLE;
	NRF_RADIO->CRCCNF = CRCCNF_LEN_3 | CRCCNF_SKIPADDR;
	NRF_RADIO->CRCPOLY = CRCPOLY_BLE_POLY;
	NRF_RADIO->TIFS = TIFS_VALUE;

	if( (NRF_FICR->OVERRIDEEN&(1<<3)) == 0 )
	{
		NRF_RADIO->OVERRIDE4 = 0x80000000;
		NRF_RADIO->OVERRIDE0 = NRF_FICR->BLE_1MBIT[0];
		NRF_RADIO->OVERRIDE1 = NRF_FICR->BLE_1MBIT[1];
		NRF_RADIO->OVERRIDE2 = NRF_FICR->BLE_1MBIT[2];
		NRF_RADIO->OVERRIDE3 = NRF_FICR->BLE_1MBIT[3];
		NRF_RADIO->OVERRIDE4 |= (NRF_FICR->BLE_1MBIT[4]&0xFFFFFFF);
	}

	NRF_RADIO->BASE0 = 0x00;
	NRF_RADIO->BASE1 = 0x00;
	NRF_RADIO->PREFIX0 = 0x00;
	NRF_RADIO->PREFIX1 = 0x00;
	
	NVIC_SetPriority(RADIO_IRQn, DH_IRQ_PRIORITY_0);	
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
