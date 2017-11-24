#include "../../include/DhGlobalHead.h"

#define INVAILD_DATA	(0xFFFF)
#define CHANNEL_ENABLE	(1)
#define ADV_OVER		(0xFE)
typedef enum
{
	ADV_IDLE = 0,	/* 一轮广播结束，等待下次广播周期到期*/
	ADV_TX,			/* 发送广播数据*/
	ADV_RX,			/* 等待接收状态，可能有连接请求或扫描请求*/
	ADV_TX_SCANRSP,
}EnSubState;

typedef struct 
{
	EnSubState			m_enAdvState;
	BlkAdvChannelOn		m_blkChannels;
	u2 					m_u2AdvInterval;	
	u1					m_u1CurrentChannel;
}BlkAdvStateInfo;

/* 存放广播数据信息 */
static u1	s_pu1AdvTxData[BLE_PDU_PAYLOAD] = {0};
static u1	s_pu1AdvRxData[BLE_PDU_PAYLOAD] = {0};
static BlkAdvStateInfo	s_blkAdvStateInfo = {0};


static u1 GetNextAdvChannel(void)
{
	u1	currentChannel = s_blkAdvStateInfo.m_u1CurrentChannel;
	u2	nextChannel = ADV_OVER;
	if( BLE_ADV_CHANNEL_37 == currentChannel )
	{
		if( CHANNEL_ENABLE == s_blkAdvStateInfo.m_blkChannels.m_ChannelOn_38)
		{
			nextChannel = BLE_ADV_CHANNEL_38;
		}
		else if(CHANNEL_ENABLE == s_blkAdvStateInfo.m_blkChannels.m_ChannelOn_39)
		{
			nextChannel = BLE_ADV_CHANNEL_39;
		}
		else
		{
			nextChannel = ADV_OVER;
		}
	}
	else if(BLE_ADV_CHANNEL_38 == currentChannel )
	{
		if(CHANNEL_ENABLE == s_blkAdvStateInfo.m_blkChannels.m_ChannelOn_39)
		{
			nextChannel = BLE_ADV_CHANNEL_39;
		}
		else
		{
			nextChannel = ADV_OVER;
		}
	}
	else if(BLE_ADV_CHANNEL_39 == currentChannel )
	{
		nextChannel = ADV_OVER;
	}
	
}
__INLINE static void HandleAdvTxDone(void)
{
	u1 channel;
	// 发送完成后在当前通道上开始接收
	channel = s_blkAdvStateInfo.m_u1CurrentChannel;
	BleRadioRxData(channel, s_pu1AdvRxData);
	BleLPowerTimerStart(BLE_LP_TIMER0, )
}

__INLINE static void HandleAdvRxDone(void)
{
	
}

static void LinkAdvRadioEvtHandler(EnBleRadioEvt evt)
{
	if( BLE_RADIO_EVT_TRANS_DONE == evt )	/* 发送完成或 接收/接收超时 完成*/
	{
		if( ADV_TX == s_blkAdvStateInfo.m_enAdvState ||  ADV_TX_SCANRSP == s_blkAdvStateInfo.m_enAdvState)
		{
			//发送完成
			HandleAdvTxDone();
		}
		else if( ADV_RX == s_blkAdvStateInfo.m_enAdvState)
		{
			//接收完成或超时
			HandleAdvRxDone();
		}
	}
}

/**
 *@brief: 		LinkFirstAdvChannelGet
 *@details:		获取第一个广播通道

 *@retval:		广播通道
 */
u1 LinkFirstAdvChannelGet(void)
{
	u1 advChannel = BLE_INVALID_CHANNEL;

	/* 获取第一个广播通道 */
	if( CHANNEL_ENABLE == s_blkAdvStateInfo.m_blkChannels.m_ChannelOn_37 )
	{
		advChannel = BLE_ADV_CHANNEL_37;
	}
	else if( CHANNEL_ENABLE == s_blkAdvStateInfo.m_blkChannels.m_ChannelOn_38 )
	{
		advChannel = BLE_ADV_CHANNEL_38;
	}
	else if( CHANNEL_ENABLE == s_blkAdvStateInfo.m_blkChannels.m_ChannelOn_39 )
	{
		advChannel = BLE_ADV_CHANNEL_39;
	}
	return  advChannel;
}


/**
 *@brief: 		LinkAdvStateInit
 *@details:		链路层广播状态初始化

 *@retval:		void
 */
void LinkAdvStateInit(void)
{
	memset(&s_blkAdvStateInfo, 0x00, sizeof(s_blkAdvStateInfo));
	s_blkAdvStateInfo.m_u2AdvInterval = INVAILD_DATA;
	BleLinkStateHandlerReg(BLE_LINK_ADVERTISING, LinkAdvRadioEvtHandler);
}


/**
 *@brief: 		LinkAdvParamsCfg
 *@details:		配置广播参数
 *@param[in]	channel  	广播通道配置
 				interval	广播间隔配置              
 *@retval:		void
 */
void LinkAdvParamsCfg(BlkAdvChannelOn channels, u2 interval)
{
	s_blkAdvStateInfo.m_blkChannels = channel;
	s_blkAdvStateInfo.m_u2AdvInterval = interval
}


/**
 *@brief: 		LinkAdvStart
 *@details:		启动广播
 *@param[in]	channel		广播通道

 *@retval:		void
 */
void LinkAdvStart(u1 channel)
{
	u1 whiteIv;

	BleRadioCrcInit(BLE_ADV_CHANNEL_CRC_INIT);
	/*
    	先只考虑一个连接的情况吧，都用逻辑地址0
    */
    BleRadioTxRxAddrCfg(0, BLE_ADV_ACCESS_ADDR);
	whiteIv = GetChannelWhiteIv(channel);
    BleRadioWhiteIvCfg(whiteIv);
    s_blkAdvStateInfo.m_u1CurrentChannel = channel;
    BleRadioTxData(channel, s_pu1AdvData, BLE_PDU_PAYLOAD);	// 长度字段实际没有作用
}
