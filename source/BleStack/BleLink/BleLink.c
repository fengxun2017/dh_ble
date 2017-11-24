/**
 * @file				BleLink.c
 * @brief			ble链路层状态机
 * @author			fengxun
 * @date			2017年11月21日
*/
#include "../../include/DhGlobalHead.h"



#define BLE_LINK_STATE_COUNT	(BLE_LINK_STATE_END)
typedef struct
{
	EnBleLinkState		m_enState;
	RadioEvtHandler		m_linkAdvHandler[BLE_LINK_STATE_COUNT];
}BlkBleLinkInfo;

static void LinkRadioEvtHandler(EnBleRadioEvt evt);
static BlkBleLinkInfo s_blkLinkInfo;

void BleLinkInit(void)
{
	u1 i;
	BleRadioInit();
	BleRadioEvtHandlerReg(LinkRadioEvtHandler);
	memset(s_blkLinkInfo, 0x00, sizeof(s_blkLinkInfo));

	BleLPowerTimerInit();	// 初始化低功耗定时器，链路层各个状态都需要定时器
	for( i = 0; i < BLE_LINK_STATE_END; i++ )
	{
		s_blkLinkInfo.m_linkAdvHandler[i] = NULL;
	}
	s_blkLinkInfo.m_enState = BLE_LINK_STANDBY;
}

void BleLinkStateSwitch(EnBleLinkState state)
{
	s_blkLinkInfo.m_enState = state;
}
void BleLinkStateHandlerReg(EnBleLinkState state, RadioEvtHandler evtHandler)
{
	s_blkLinkInfo.m_linkAdvHandler[state] = evtHandler;
}

static void LinkRadioEvtHandler(EnBleRadioEvt evt)
{
	EnBleLinkState state = s_blkLinkInfo.m_enState;
	
	switch(evt)
	{
		case BLE_RADIO_EVT_TRANS_DONE:
		if( NULL != s_blkLinkInfo.m_linkAdvHandler[state])
		{
			s_blkLinkInfo.m_linkAdvHandler[state](evt);
		}
	}
}
