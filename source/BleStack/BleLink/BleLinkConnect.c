
/**
 * @file			BleLinkConnect.c
 * @brief			处理ble链路层连接相关的数据包和交互
 * @author			fengxun
 * @date			2017年12月12日
*/
#include "../../include/DhGlobalHead.h"

#define BLE_LINK_CONN_DEBUG

#if !defined(BLE_LINK_CONN_DEBUG)
#undef DEBUG_INFO
#define DEBUG_INFO(...)
#undef DEBUG_DATA
#define DEBUG_DATA(...)
#endif

/* 下面几个宏都是us */
#define WIN_WIDEN_ADDITIONAL					(31)		/* Window Widening 的基础上再加30us放宽时间，rtc的硬件start stop clear等可能有30us的时钟误差虽然底层做了误差的一些处理，
																	不过为了ble链路的稳定，这里还是在额外监听30us，rtc单位是30.517*/
#define SLEEP_INSTANTANEOUS_DEVIATE_TIME		(16)		/* ble协议规定的睡眠下瞬时延迟不能超过16us */
#define ACTIVE_INSTANTANEOUS_DEVIATE_TIME		(2)			/* ble协议规定的活动下瞬时延迟不能超过2us */
#define FIXED_1_25_MS							(1250)		/* 1.25ms延迟，第一个主机发送的包的时序用到*/
#define PACKET_EXTEND_WINDOW				(BLE_PACKET_LENGTH*8)	/* 规范定义连接创建后主机发送的第一个数据包可以超过传输窗口，即窗口结束前收到了
																			数据包，但是剩下数据会超过窗口。 所以这里直接第一个最大数据包长的情况来做一个监听的延长。
																			PS：可以通过设置 nrf的ADDRESS中断避免错误情况下这个浪费的延迟监听时间。*/

#define BLE_CONN_SUB_STATE_RX_MASK				(0x02)
/*
	连接请求中的LLDATA内容:
	AA			CRCInit		WinSize		WinOffset	Interval	Latency		Timeout		ChM			Hop			SCA
	(4 octets)	(3 octets)	(1 octet)	(2 octets)	(2 octets)	(2 octets)	(2 octets)	(5 octets)	(5 bits)	(3 bits)
*/
#define LLDATA_AA_POS				(0)
#define LLDATA_AA_SIZE				(4)
#define LLDATA_CRCINIT_POS			(LLDATA_AA_POS+LLDATA_AA_SIZE)
#define LLDATA_CRCINIT_SIZE			(3)
#define LLDATA_WINSIZE_POS			(LLDATA_CRCINIT_POS+LLDATA_CRCINIT_SIZE)
#define LLDATA_WINSIZE_SIZE			(1)
#define LLDATA_WINOFFSET_POS		(LLDATA_WINSIZE_POS+LLDATA_WINSIZE_SIZE)
#define LLDATA_WINOFFSET_SIZE		(2)
#define LLDATA_INTERVAL_POS			(LLDATA_WINOFFSET_POS+LLDATA_WINOFFSET_SIZE)
#define LLDATA_INTERVAL_SIZE		(2)
#define LLDATA_LATENCY_POS			(LLDATA_INTERVAL_POS+LLDATA_INTERVAL_SIZE)
#define LLDATA_LATENCY_SIZE			(2)
#define LLDATA_TIMEOUT_POS			(LLDATA_LATENCY_POS+LLDATA_LATENCY_SIZE)
#define LLDATA_TIMEOUT_SIZE			(2)
#define LLDATA_CHM_POS				(LLDATA_TIMEOUT_POS+LLDATA_TIMEOUT_SIZE)
#define LLDATA_CHM_SIZE				(5)
#define LLDATA_HOP_SCA_POS			(LLDATA_CHM_POS+LLDATA_CHM_SIZE)
#define LLDATA_HOP_SCA_SIZE			(1)

#define MORE_DATA_FLAG				(0x01)
#define NO_MORE_DATA				(0x00)

/* 数据通道PDU的header 第一个字节*/
#define DATA_HEAD_LLID_POS			(0)
#define DATA_HEAD_LLID_LENGTH		(2)		// 2bits
#define DATA_HEAD_NESN_POS			(DATA_HEAD_LLID_POS+DATA_HEAD_LLID_LENGTH)
#define DATA_HEAD_NESE_LENGTH		(1)
#define DATA_HEAD_SN_POS			(DATA_HEAD_NESN_POS+DATA_HEAD_NESE_LENGTH)
#define DATA_HEAD_SN_LENGTH			(1)
#define DATA_HEAD_MD_POS			(DATA_HEAD_SN_POS+DATA_HEAD_SN_LENGTH)
#define DATA_HEAD_MD_LENGTH			(1)

#define CHANNEL_MAP_UPDATING        (1) 
#define CHANNEL_MAP_UPDATED         (0)

#define LINK_RX_OLD_DATA            (1)
#define LINK_RX_NEW_DATA            (2)
#define LINK_TX_NEW_DATA            (4)
#define LINK_TX_OLD_DATA            (8)

typedef enum
{
	CONN_IDLE = 0,
	CONN_CONNING_RX = 0x12,			// 连接建立中，收到连接请求后等待主机发过来的第一个数据包的过程
	CONN_CONNECTED_TX = 0x21,		// 连接后的发送数据状态
	CONN_CONNECTED_RX = 0x22,		// 连接后的等待数据状态
	CONN_CONNECTED_RXTIMEOUT=0x30,  // 接收超时
	CONN_CONNING_RXTIMEOUT = 0x40,
}EnConnSubState;	// 连接态的子状态

typedef struct
{
/* 
数据通道选择：
		unmappedChannel:为映射前选择的本次连接事件的数据通道。
		lastUnmappedChannel：前一个连接事件的未映射通道。连接后的第一个连接事件时，该值为0.
							每个连接事件关闭后该值设置为 unmappedChannel

		unmappedChannel = (lastUnmappedChannel + hopIncrement) mod 37
		计算出的unmappedChannel 值表示的通道号 如果在通道映射表ChMap 中为可使用状态，则该值就确定作为本次连接事件的数据通道。

		如果计算出的unmappedChannel 值表示的通道号 如果在通道映射表ChMap 中为不可使用状态，则需要做一次重映射。
		remappingIndex = unmappedChannel mod numUsedChannels	
		remappingIndex:为重映射后的下标
		numUsedChannels：为ChMap中可使用的通道个数。
		重映射表：所有ChMap中可是使用的通道按递增排序，下标从0开始。
		最终本次连接事件使用的数据通道则为 重映射表 中 remappingIndex指示的那个通道。
*/
	u1	m_u1UnmappedChannel;
	u1	m_u1LastUnmappedChannel;
	u1	m_u1CurrentChannel;			/* 当前连接事件使用的数据通道*/
/*
连接事件关闭:
	MD: more data, 通信双方都没有置位这个标记时，主从都应该关闭当前的连接事件。
		任何一方设置了了该标记，主机都可能会继续维持当前的连接事件并发送下一个包，所以从机也应该继续监听下一个可能的包。如果主机发送了包后
		没有收到从机的响应，则主机关闭当前连接事件。如果从机没有接收到主机发来的包，则从机关闭当前连接事件。
		关于等待数据包的时间应该就是 IFS,即连续包应该在IFS后发出，规范限定的误差为 IFS-2 —— IFS+2
		
		对于从机除非收到了2个连续的crc错误包，否则每次收到主机发过来的包都需要回复响应。
*/
	u1	m_u1PeerMD;

	u2	m_u2ConnEventCounter;				/* 连接事件计数，用来同步一些链路控制。从0开始，不管连接事件中有没有收到数据，这个计数都要增加。*/

/* 
SN,NESN: 响应机制。 收到的NESN是告诉自己对方有没有收到前一个包，自己的NESN是告诉对方的。

	连接建立后 TransmitSeqNum 和 NextExpectedSeqNum 都为0
	每个发送的新数据包，SN都要设置成TransmitSeqNum，重发的数据包SN不变
	每收到一个数据包，SN应该和 NextExpectedSeqNum比较，如果不同则为重发包，NextExpectedSeqNum不改变。如果相同则为新包，NextExpectedSeqNum改变
	每个发送的数据包，NESN 要设置为 NextExpectedSeqNum
	如果收到的数据包，NESN和 TransmitSeqNum相同，则之前发送的数据包对方没有收到，应该重发，TransmitSeqNum不变。如果不相同则表明对方收到了之前的数据包 TransmitSeqNum改变

	如果crc错误，nextExpectedSeqNum不改变，即该错误包不被响应。则对方回重发。
	重发的包SN不变
*/ 
	u1	m_u1TransmitSeqNum;						/* 标识链路层数据包*/
	u1	m_u1NextExpectedSeqNum;					/* ack,或者请求重发*/
	
/* 连接请求中的参数，收到连接请求后，第一个主机发过来的包不管CRC对不对，都决定了连接事件的anchor point */
	u1	m_u1TransmitWindowSize;					/* 连接建立过程中的监听窗口 1.25ms为单位,1.25 ms to the lesser of 10 ms and (connInterval - 1.25 ms).窗口开始时间为收到连接请求后的transmitWindowOffset + 1.25ms */
	u2	m_u2TransmitWindowOffset;				/* 监听窗口偏移 1.25ms为单位,0 ms——connInterval*/
	u2	m_u2ConnInterval;						/* 连接间隔 1.25ms为单位         7.5ms-4s*/
	u2  m_u2ConnIntervalNew;
	u2	m_u2ConnSlaveLatency;					/* 从机延迟--从机可以跳过几个连接时间不监听，降低功耗。0——((connSupervisionTimeout/ connInterval) - 1)，不能大于500
													如果延迟后没有收到主机包，则后续的每个连接事件都应该监听，知道收到数据包后才能再应用这个参数。*/
    u2  m_u2ConnSlaveLatencyNew;													
	u2	m_u2ConnSupervisionTimeout;				/* 连接超时--10ms为单位，100 ms —— 32.0 s并且大于(1 + connSlaveLatency) * connInterval。连接建立过程如果超过6个connInterval时间，则认为建立失败 */
    u2  m_u2ConnSupervisionTimeoutNew;             	
	u2	m_u2ConnSupervisionTimeoutCounter;		/* 没有资源对超时单独创建定时器，超时时间不需要精确，这里用一个计数器，以connInterval为单位来计数*/
	u2  m_u2connTimeoutBackup;                  /* 连接监控超时备份 */
	u2  m_u2ConnUpdateInstant;                  /* 连接参数更新的时间点 */
	u1	m_pu1ChannelMap[BLE_CHANNEL_MAP_LEN];	/* 通道映射图，指示可用通道 ,LSB
													The LSB represents data channel index 0 and the bit in position 36 represents data channel index 36.*/
	u1  m_pu1NewChannelMap[BLE_CHANNEL_MAP_LEN];
	u1  m_u1ChannelMapUpdateInstant;            /* 更新channel map的时间点 */
	u1  m_u1ChannelMapNeedUpdateFlag;           /* 收到LL_CHANNEL_MAP_REQ 后需要更新channelMap */
	u1	m_pu1AccessAddress[BLE_ACC_ADDR_LENGTH];/* 接入地址,LSB*/
	u1	m_pu1CrcInitValue[BLE_CRC_LENGTH];		/* CRC初值，LSB*/
	u1	m_u1HopIncrement;						/* 跳频增量 */
	u1	m_u1PeerSCA;							/* 对端设备睡眠时钟精度 */
	u1	m_u1SelfSCA;							/* 自己睡眠时间精度 */

	u1	m_u1PeerAddrType;						/* 对端设备地址类型 */
	u1	m_pu1PeerAddr[BLE_ADDR_LEN];			/* 对端设备地址 ,LSB    */

	u4  m_u4SleepTimer;                         /* 每次等待下次连接事件时的睡眠时间 */
	                                            
	EnConnSubState		m_enConnSubState;		/* 连接态子状态 */
	
}BlkConnStateInfo;

/* 初值设置成空包,用来回复第一包数据 */
static u1	LINK_CONN_STATE_TX_BUFF[BLE_PDU_LENGTH] = {0x05,0x00};   
static u1	LINK_CONN_STATE_RX_BUFF[BLE_PDU_LENGTH];
static u2	SCA_TO_PPM[BLE_SCA_GRADE_NUMBER] = {500, 250, 150, 100, 75, 50, 30, 20};

#define WAIT_PACKET_SLEEP_TIMER			BLE_LP_TIMER0			// 下一个连接事件到来前的睡眠唤醒定时器
#define RECV_PACKET_HA_TIMER			BLE_HA_TIMER0		    // 连接事件到了后等待数据包的定时器

BlkConnStateInfo s_blkConnStateInfo;
static void PacketRecvTimeout(void *pvalue);



__INLINE static void LinkConnSubStateSwitch(EnConnSubState enConnSubState)
{
	s_blkConnStateInfo.m_enConnSubState = enConnSubState;
}

__INLINE static void UpdateChannelMap(void)
{
    if( CHANNEL_MAP_UPDATING == s_blkConnStateInfo.m_u1ChannelMapNeedUpdateFlag )
    {
        memcpy(s_blkConnStateInfo.m_pu1ChannelMap, s_blkConnStateInfo.m_pu1NewChannelMap, BLE_CHANNEL_MAP_LEN);
        s_blkConnStateInfo.m_u1ChannelMapNeedUpdateFlag = CHANNEL_MAP_UPDATED;
    }
}
/**
 *@brief: 		GetTimerDeviation
 *@details:		获取time时间下由于时钟精度导致的时间误差
 *@param[in]	u4TimeUs		要计算误差的时间
 *@param[in]	peerSCA			对端设备 睡眠时钟精度
 *@param[in]	selfSCA			自己的睡眠时钟精度

 *@retval:		u4Time 时间的误差 us
 */
__INLINE static u4 GetTimerDeviation(u4 u4TimeUs, u1 peerSCA, u1 selfSCA)
{
	u2	clockPPM = SCA_TO_PPM[peerSCA] + SCA_TO_PPM[selfSCA];
	u4	deviation;

	deviation = u4TimeUs*clockPPM;
	deviation /= 1000000;

	return deviation;
	
}

/**
 *@brief: 		UpdateSeqNum
 *@details:		更新自己的sn和nesn
 *@param[in]	SN			对方发过来的数据包中的SN 
 *@param[in]	NESN  		对方发过来的数据包中的NESN
 *@param[in]	crcTrue  	接收到的数据包crc是否正确
 *@retval:		void
 */
__INLINE static u4 UpdateSeqNum(u1 SN, u1 NESN, u1 crcTrue )
{
	u1	selfSN = s_blkConnStateInfo.m_u1TransmitSeqNum;
	u1	selfNESN = s_blkConnStateInfo.m_u1NextExpectedSeqNum;
    u4  ret = 0;
    
    /* 如果crc错了不更新nesn */
	if( SN == selfNESN && crcTrue)
	{
		/* 是期望收到的包，则更新NESN */
		s_blkConnStateInfo.m_u1NextExpectedSeqNum = 1-selfNESN;
		ret |= LINK_RX_NEW_DATA;
	}
	else    /* crc错了也当成接收到重发包好了，因为重发包不处理 */
	{
		/* 是对方重发的包，NESN不更新。可能我回         的ack对方没收到，或  者我收到了对方的包crc错了，所以没有更新NESN回空包，这样对方收不到ack就重发*/
		ret |= LINK_RX_OLD_DATA;
	}

	if( NESN == selfSN )
	{
		/* 对方没有对我上一个发的包回ack,所以不更新SN，要重发 */
		ret |= LINK_TX_OLD_DATA;
	}
	else
	{
		/* 更新SN */
		s_blkConnStateInfo.m_u1TransmitSeqNum = 1-selfSN;
		ret |= LINK_TX_NEW_DATA;
	}

	return ret;
}

/**
 *@brief: 		UpdateLastUnmappedChannel
 *@details:		更新lastUnmapChannel值，在每次连接事件结束后调用
             
 *@retval:		void
 */
__INLINE static void UpdateLastUnmappedChannel(void)
{
	/* 在本次连接事件结束的时候将LastUnmappedChannel 设置为 UnmappedChannel */
	s_blkConnStateInfo.m_u1LastUnmappedChannel = s_blkConnStateInfo.m_u1UnmappedChannel;
}

/**
 *@brief: 		GetNextDataChannel
 *@details:		获取下个连接事件所在的数据通道，每次连接件开始前调用
 *@retval:		void
 */
__INLINE static u1 GetNextDataChannel(void)
{
	u1	pu1ReMapTable[BLE_DATA_CHANNEL_NUMBER];
	u1	u1RemappingIndex;
	u1	u1UnmappedChannel;
	u1	channelIndex,usedChannels;
	
	u1UnmappedChannel = (s_blkConnStateInfo.m_u1LastUnmappedChannel+s_blkConnStateInfo.m_u1HopIncrement)%BLE_DATA_CHANNEL_NUMBER;
	/* 保存,更新lastUnmappedChannel的时候需要用到  */
	s_blkConnStateInfo.m_u1UnmappedChannel = u1UnmappedChannel;
	
	if( s_blkConnStateInfo.m_pu1ChannelMap[u1UnmappedChannel/8]&(1<<(u1UnmappedChannel%8)) )
	{
		/* u1UnmappedChannel 通道号可以使用，则直接使用该值作为下一次连接事件时的数据通道 */
	}
	else
	{
		/* 否则，则需要重映射，先建立映射表 */
		usedChannels = 0;
		for( channelIndex = 0; channelIndex < BLE_DATA_CHANNEL_NUMBER; channelIndex++ )
		{
			if(s_blkConnStateInfo.m_pu1ChannelMap[channelIndex/8]&(1<<(channelIndex%8)))
			{
				/* 如果该通道可用则放在重映射表中*/
				pu1ReMapTable[usedChannels++] = channelIndex;
			}
		}
		u1RemappingIndex = u1UnmappedChannel%usedChannels;
		/* 获取重映射后的通道值 */
		u1UnmappedChannel = pu1ReMapTable[u1RemappingIndex];	
	}
	return u1UnmappedChannel;
}

__INLINE static void CfgCurrentDataChannel(u1 channel)
{
	s_blkConnStateInfo.m_u1CurrentChannel = channel;
}

/* 重发上一次发送的包*/
__INLINE static void LinkResendData(void)
{
    u2 len;
	u1 nesn = s_blkConnStateInfo.m_u1NextExpectedSeqNum; //重发包nesn还是要变的

    LINK_CONN_STATE_TX_BUFF[0] &= ~(1<<DATA_HEAD_NESN_POS);
    LINK_CONN_STATE_TX_BUFF[0] |= (nesn<<DATA_HEAD_NESN_POS);
    len = LINK_CONN_STATE_TX_BUFF[1]&0x1F;
	BleRadioSimpleTx(LINK_CONN_STATE_TX_BUFF, BLE_PDU_HEADER_LENGTH+len);
}
__INLINE static void LinkSendData(u1 u1PacketFlag, u1 *pu1Data, u2 u2len)
{
	u1	sn,nesn,md,llid,len;
	BlkBlePduHeader *pHeader;

	sn = s_blkConnStateInfo.m_u1TransmitSeqNum;
	nesn = s_blkConnStateInfo.m_u1NextExpectedSeqNum;

	len = u2len;
	if( CONTROL_PACKET==u1PacketFlag && u2len>0 )
	{
        llid = LLID_CONTROL;
	}
	else if( NULL==pu1Data || 0==u2len )
	{
		llid = LLID_EMPTY_PACKET;
		len = 0;
	}
	else
	{	
		/* 目前不实现延续包，一个间隔发一包，ATT MTU目前也只支持默认23，所以没有延续包的情况 */
		llid = LLID_START;	
	}
	md = NO_MORE_DATA;	/* 目前一个间隔就交互一次 */
	
	pHeader = (BlkBlePduHeader *)LINK_CONN_STATE_TX_BUFF;
	pHeader->m_u1Header1 = (sn<<DATA_HEAD_SN_POS) | (nesn<<DATA_HEAD_NESN_POS) | (llid<<DATA_HEAD_LLID_POS) | (md<<DATA_HEAD_MD_POS);
	pHeader->m_u1Header2 = len;
	if( NULL!=pu1Data )
	{
		memcpy(LINK_CONN_STATE_TX_BUFF+BLE_PDU_HEADER_LENGTH, pu1Data, len);
	}
	BleRadioSimpleTx(LINK_CONN_STATE_TX_BUFF, BLE_PDU_HEADER_LENGTH+len);
}

__INLINE static void ExtractRecvPacketSn(u1 *sn, u1 *nesn)
{
	BlkBlePduHeader *pHeader;

	pHeader = (BlkBlePduHeader *)LINK_CONN_STATE_RX_BUFF;
	*sn = (pHeader->m_u1Header1 >> DATA_HEAD_SN_POS) & 0x01;
	*nesn = (pHeader->m_u1Header1 >> DATA_HEAD_NESN_POS) & 0x01;
}

__INLINE static void BleLinkDataHandle(void)
{
	BlkLinkToHostData	data;
	BlkBlePduHeader *pHeader;
    u1  pu1NewChannelMap[BLE_CHANNEL_MAP_LEN];	
	u1	len;
	u2  u2Instant;
	
	pHeader = (BlkBlePduHeader *)LINK_CONN_STATE_RX_BUFF;
	len = pHeader->m_u1Header2 & 0x1F;	

    /* channelMap 更新控制过程需要单独特殊处理，不能延后软中断里面处理 */
	if( DH_SUCCESS== CheckLinkChannelMapUpdateReq(LINK_CONN_STATE_RX_BUFF, pu1NewChannelMap, &u2Instant) )
	{
	    /* 相减最高位为1说明溢出了 */
	    if( (u2Instant-s_blkConnStateInfo.m_u2ConnEventCounter) >= 32767)
	    {
            /*
                做断开处理并通知上层
            */
	    }
	    else
	    {
            memcpy(s_blkConnStateInfo.m_pu1NewChannelMap, pu1NewChannelMap, BLE_CHANNEL_MAP_LEN);
            s_blkConnStateInfo.m_u1ChannelMapNeedUpdateFlag = CHANNEL_MAP_UPDATING;
            s_blkConnStateInfo.m_u1ChannelMapUpdateInstant = u2Instant;
        }
        return DH_SUCCESS;
	}
	/* 非空包才处理*/
	if( len>0 )
	{
		memcpy(data.m_pu1LinkData, LINK_CONN_STATE_RX_BUFF, BLE_PDU_LENGTH);
		BleLinkDataToHostPush(data);
	}
}


/**
 *@brief: 		ConnStateRxPacketHandler
 *@details:		开始监听数据
 
 *@retval:		void
 */
static void ConnStateRxPacketHandler(void *pValue)
{
	u1	u1DataChannel;
	u4	u4PassTime;
	u4	u4PassTimeDeviation;
	u1  whiteIv;
	static u4	u4WinSize;

    if( CHANNEL_MAP_UPDATING==s_blkConnStateInfo.m_u1ChannelMapNeedUpdateFlag )
    {
        /* 更新 channel map的时间点到了 */
        if( s_blkConnStateInfo.m_u2ConnEventCounter == s_blkConnStateInfo.m_u1ChannelMapUpdateInstant )
        {
            UpdateChannelMap();
        }
    }
	/* 设置本次连接事件所在的数据通道 */
	u1DataChannel  = GetNextDataChannel();
	whiteIv = GetChannelWhiteIv(u1DataChannel);
	BleRadioWhiteIvCfg(whiteIv);
	CfgCurrentDataChannel(u1DataChannel);
	
	BleRadioRxData(u1DataChannel, LINK_CONN_STATE_RX_BUFF);

	/* 计算过去的时间由于时钟源造成的可能的误差 */
	u4PassTime = *((u4*)pValue);
	u4PassTimeDeviation = GetTimerDeviation(u4PassTime, s_blkConnStateInfo.m_u1PeerSCA, s_blkConnStateInfo.m_u1SelfSCA);

	if( CONN_CONNING_RX == s_blkConnStateInfo.m_enConnSubState )
	{
		/*
		接收监听窗口大小需要加上 RTC硬件机制误差，睡眠时钟瞬时误差，数据包接收过程允许超过接口窗口，所以还要加上
		一个数据包的传输时间。这里先按最长数据包来算吧。保证稳定性再说。
		监听包的开始时间是根据时钟精度做了 提前唤醒，但是时钟精度偏的方向是不确定的，所以监听时间也要加长 u4PassTimeDeviation 这个误差。
		WIN_WIDEN_ADDITIONAL+SLEEP_INSTANTANEOUS_DEVIATE_TIME+u4PassTimeDeviation 做了2倍操作，因为唤醒要提前，等待接收数据窗口要延后，延后是要在补回之前提前的基础上再延后
		*/
		u4WinSize = s_blkConnStateInfo.m_u1TransmitWindowSize*1250 + 2*(WIN_WIDEN_ADDITIONAL+SLEEP_INSTANTANEOUS_DEVIATE_TIME+u4PassTimeDeviation+PACKET_EXTEND_WINDOW);
		/* 监听延迟再加上监听时间由于时钟精度造成的误差 */


		//u4WinSize += GetTimerDeviation(u4WinSize, s_blkConnStateInfo.m_u1PeerSCA, s_blkConnStateInfo.m_u1SelfSCA);
		/* 
			启动定时器，最后一个参数是超时触发时使用的。
			接收超时的时候需要重新启动下一个间隔的监听定时器，定时时间是从这里的时间点开始算的。
			而接收超时后，时间已经过了 u4WinSize us，所以接收超时后重启定时器需要减去u4winsize
		*/
		//BleLPowerTimerStart(RECV_PACKET_LP_TIMER, u4WinSize, PacketRecvTimeout, &u4WinSize);

		/* 没必要为了等第一个数据包 ms级的时间内节省功耗而使用LPtimer，精度不准，影响第一次同步 */
		BleHAccuracyTimerStart(RECV_PACKET_HA_TIMER, u4WinSize, PacketRecvTimeout, &u4WinSize);
        DEBUG_INFO("channel:%d prepare conning rx:%d",u1DataChannel,u4WinSize);
	}
	else 
	{
		LinkConnSubStateSwitch(CONN_CONNECTED_RX);	// 切换到连接态的等待数据子状态
		/* 
			不是在监听连接后的第一个数据包，则没有监听窗口，数据到来时间理论应该是连接事件的 Anchor Point
			不过实际需要考虑睡眠过程中的时钟等误差和数据包的空中传输时间
		*/
		u4WinSize = 2*(WIN_WIDEN_ADDITIONAL + SLEEP_INSTANTANEOUS_DEVIATE_TIME +u4PassTimeDeviation + PACKET_EXTEND_WINDOW);
		BleHAccuracyTimerStart(RECV_PACKET_HA_TIMER, u4WinSize, PacketRecvTimeout, &u4WinSize);
		DEBUG_INFO("%d prepare conned rx:%d",u1DataChannel,u4WinSize);
	}
}

/**
 *@brief: 		PacketRecvTimeout
 *@details:		监听数据包超时了
 *@param[in]	pValue    定时器已经过去的时间
 
 *@retval:		void
 */
static void PacketRecvTimeout(void *pvalue)
{
	u4	u4PassTime;
	static u4	u4SleepTimer;

	/*超时关闭radio*/
	BleRadioDisable();
	if( CONN_CONNECTED_RX == s_blkConnStateInfo.m_enConnSubState )
	{
        LinkConnSubStateSwitch(CONN_CONNECTED_RXTIMEOUT);
	}
	else if( CONN_CONNING_RX == s_blkConnStateInfo.m_enConnSubState )
	{
        LinkConnSubStateSwitch(CONN_CONNING_RXTIMEOUT);
	}
	
	if ( s_blkConnStateInfo.m_u2ConnSupervisionTimeoutCounter == 0 )
	{
		/* 连接超时了 */

		BleLinkStateSwitch(BLE_LINK_STANDBY);	// 链路状态回到空闲态
		LinkConnSubStateSwitch(CONN_IDLE);		// 复位连接子状态

		if ( CONN_CONNING_RX != s_blkConnStateInfo.m_enConnSubState )
		{
			/*
				上报断开事件,建立连接过程中超时就不上报了吧。
				
			*/
		}
		return;
	}
	u4PassTime = *((u4*)pvalue);	// 之前监听的超时时间
	/* 没有接收到数据包，理论上在connInterval 时间后重新监听 */
	u4SleepTimer = s_blkConnStateInfo.m_u2ConnInterval*1250-WIN_WIDEN_ADDITIONAL-SLEEP_INSTANTANEOUS_DEVIATE_TIME;

	/* 需要减去之前监听窗口的时间 */
	u4SleepTimer -=  u4PassTime;

	/* 再减去时钟源误差可能导致的误差 */
	u4SleepTimer -= GetTimerDeviation(u4SleepTimer, s_blkConnStateInfo.m_u1PeerSCA, s_blkConnStateInfo.m_u1SelfSCA);
	BleLPowerTimerStart(WAIT_PACKET_SLEEP_TIMER, u4SleepTimer, ConnStateRxPacketHandler, &u4SleepTimer);
	

	/* 超时则等于数本次连接事件结束 */
	UpdateLastUnmappedChannel();				// 更新 LastUnmapChannel
	s_blkConnStateInfo.m_u2ConnEventCounter++;	// 更新连接计数
	
	s_blkConnStateInfo.m_u2ConnSupervisionTimeoutCounter--;// 没等到数据包，超时计数减1
	DEBUG_INFO("Rx timeout,restart:%d",u4SleepTimer);
}


static void LinkConnRadioEvtHandler(EnBleRadioEvt evt)
{
	u1	peerSN;
	u1	peerNESN;
	u4  ret;
	u1  crcTrue = 1;
	static u4	u4SleepTimer;
	static BlkHostToLinkData blkHostData = {0x00,DATA_PACKET,0x00}; //初值设置为空包

	if( BLE_RADIO_EVT_TRANS_DONE == evt )	/* 发送完成或接收完成*/
	{
		if( s_blkConnStateInfo.m_enConnSubState&BLE_CONN_SUB_STATE_RX_MASK )		//接收状态
		{
		    /* 先打开发送，nordic打开发送有一个130us左右的时间 */
		    /* STEP 1*/
		    NrfRadioStartTx();          //<----------------------------------------提前让ble进入发送准备状态
            if( !IsBleRadioCrcOk() )
            {
                DEBUG_INFO("CRC error!!!");
                /* CRC错误直接回空包，规范要求连续2个包crc错误应该关闭本次连接事件 */
                crcTrue = 0;
                return ;
            }

		
			BleHAccuracyTimerStop(RECV_PACKET_HA_TIMER);		// 停止接收监听定时器
			BleLPowerTimerStop(WAIT_PACKET_SLEEP_TIMER);        
			ExtractRecvPacketSn(&peerSN, &peerNESN);
			ret = UpdateSeqNum(peerSN, peerNESN, crcTrue);
            /* STEP 2: 从step 1 到实际调用发送调试时发现需要70us左右，影响了ble的时序，
                        所以STEP 1处直接先调用发送，因为nordic本身发送需要一个130ms的发送启动时间
                        这样就可以利用这130us中的70us来先处理一下紧急的事情
            */
            if( !crcTrue )
            {
                /* crc错了回空包，不过自己nesn没有更新，这样对方收不到ack就会重发 */
                LinkSendData(DATA_PACKET, NULL, 0);
            }
			else if( LINK_RX_OLD_DATA&ret || LINK_TX_OLD_DATA&ret )
			{
			    /* crc正确，无论是收到对方重发的包，还是对方没有对我回ack，都可以通过重发上一个包解决 */
                LinkResendData();
			}
			else 
			{
    			/*
                    有数据发送，没数据回空包
    			*/
    			LinkSendData(blkHostData.m_u1PacketFlag, blkHostData.m_pu1HostData, blkHostData.m_u1Length);
			}
            LinkConnSubStateSwitch(CONN_CONNECTED_TX); // 收到数据包则需要回复数据，切换到发送子状态
			/* 
				每次收到数据都刷新一下
			*/
			s_blkConnStateInfo.m_u2ConnSupervisionTimeoutCounter = s_blkConnStateInfo.m_u2connTimeoutBackup;

			/*
				收到本次连接事件的第一个数据包了，确定了anchor point
				启动下次连接事件的定时器
			*/
			u4SleepTimer = s_blkConnStateInfo.m_u4SleepTimer;
			BleLPowerTimerStart(WAIT_PACKET_SLEEP_TIMER, u4SleepTimer, ConnStateRxPacketHandler, &u4SleepTimer);
			
			if( crcTrue )
			{
			    BleLinkDataHandle();
			}

			DEBUG_INFO("recved packet,next anchor point:%d",u4SleepTimer);

			/* 先设置好下次要发送的数据 */
			if( DH_SUCCESS == BleHostDataToLinkPop(&blkHostData) )
			{
				DEBUG_INFO("next tx :");DEBUG_DATA(LINK_CONN_STATE_TX_BUFF, BLE_PDU_HEADER_LENGTH+blkHostData.m_u1Length);
			}
			else
			{
				blkHostData.m_u1Length = 0;
				blkHostData.m_u1PacketFlag = DATA_PACKET;
				DEBUG_INFO("next rsp empty");
			}	
		}
		else if ( CONN_CONNECTED_TX == s_blkConnStateInfo.m_enConnSubState )
		{
			/* 当前实现一个连接间隔只能发送一包数据，所以发送完成就是本次连接事件结束 */
			UpdateLastUnmappedChannel();				// 更新 LastUnmapChannel
			s_blkConnStateInfo.m_u2ConnEventCounter++;	// 更新连接计数
			DEBUG_INFO("tx done");
		}
		else
		{
            if( CONN_CONNECTED_RXTIMEOUT == s_blkConnStateInfo.m_enConnSubState )
            {
                /* 本次连接事件中没等到数据，可能空中包丢了，或者连接事件丢失,重置为接收态等待下个事件 */
                
                LinkConnSubStateSwitch(CONN_CONNECTED_RX);
                
            }
            else if(CONN_CONNING_RXTIMEOUT == s_blkConnStateInfo.m_enConnSubState )
            {
                /* 等待连接后第一个数据包超时了，重置状态等待下个连接事件 */
                LinkConnSubStateSwitch(CONN_CONNING_RX);
            }
            DEBUG_INFO("Rx timeout");
		}

	}
}

/**
 *@brief: 		LinkConnReqHandle
 *@details:		连接请求的处理
 *@param[in]	addrType    对端设备的地址类型
 				pu1Addr    	对端设备地址
 				pu1LLData  	连接参数
 *@retval:		void
 */
void LinkConnReqHandle(u1 addrType, u1 *pu1Addr, u1* pu1LLData)
{
	u4	u4CrcIV;			// crc初值 
	u4	u4AccAddr;
	u4  u4SleepTimer;
	static u4	u4AnchorPoint;		// 主机第一个包发送过来的期望时间
	s_blkConnStateInfo.m_u1PeerAddrType = addrType;
	memcpy(s_blkConnStateInfo.m_pu1PeerAddr, pu1Addr, BLE_ADDR_LEN);

	memcpy(s_blkConnStateInfo.m_pu1AccessAddress, pu1LLData+LLDATA_AA_POS, LLDATA_AA_SIZE);
	memcpy(s_blkConnStateInfo.m_pu1CrcInitValue, pu1LLData+LLDATA_CRCINIT_POS, LLDATA_CRCINIT_SIZE);
	s_blkConnStateInfo.m_u1TransmitWindowSize = pu1LLData[LLDATA_WINSIZE_POS];
	s_blkConnStateInfo.m_u2TransmitWindowOffset = pu1LLData[LLDATA_WINOFFSET_POS]+((pu1LLData[LLDATA_WINOFFSET_POS+1]<<8)&0xFF00);

	/*
		主机连接后第一个包发送过来的时间点
		理论时间点是连连接请求数据包后 1.25 ms+transmitWindowOffset
		实际需要减去RTC硬件机制误差和瞬时误差，让其提前唤醒等待主机的数据。
	*/
	u4AnchorPoint = FIXED_1_25_MS +s_blkConnStateInfo.m_u2TransmitWindowOffset*1250-WIN_WIDEN_ADDITIONAL-SLEEP_INSTANTANEOUS_DEVIATE_TIME-PACKET_EXTEND_WINDOW;
	/* u4AnchorPoint 还要减去这段时间由于rtc时钟源精度造成的误差，提前唤醒 */
	u4AnchorPoint = u4AnchorPoint-GetTimerDeviation(u4AnchorPoint, s_blkConnStateInfo.m_u1PeerSCA, s_blkConnStateInfo.m_u1SelfSCA);
	BleLPowerTimerStart(WAIT_PACKET_SLEEP_TIMER, u4AnchorPoint, ConnStateRxPacketHandler, &u4AnchorPoint);
	
	s_blkConnStateInfo.m_u2ConnInterval = pu1LLData[LLDATA_INTERVAL_POS]+((pu1LLData[LLDATA_INTERVAL_POS+1]<<8)&0xFF00);
	s_blkConnStateInfo.m_u2ConnSlaveLatency = pu1LLData[LLDATA_LATENCY_POS]+((pu1LLData[LLDATA_LATENCY_POS+1]<<8)&0xFF00);
	s_blkConnStateInfo.m_u2ConnSupervisionTimeout = pu1LLData[LLDATA_TIMEOUT_POS]+((pu1LLData[LLDATA_TIMEOUT_POS+1]<<8)&0xFF00);
	memcpy(s_blkConnStateInfo.m_pu1ChannelMap, pu1LLData+LLDATA_CHM_POS, LLDATA_CHM_SIZE);
	s_blkConnStateInfo.m_u1HopIncrement = pu1LLData[LLDATA_HOP_SCA_POS]&0x1F;
	s_blkConnStateInfo.m_u1PeerSCA = (pu1LLData[LLDATA_HOP_SCA_POS]>>5)&0x07;

	s_blkConnStateInfo.m_u1LastUnmappedChannel = 0;				// 连接设置上一次数据通道为0，为了计算下一次数据通信的通道。
	s_blkConnStateInfo.m_u2ConnSupervisionTimeoutCounter = 6;	// 连接建立过程中，超时时间为6次间隔。

	s_blkConnStateInfo.m_u1PeerMD = NO_MORE_DATA;	
	s_blkConnStateInfo.m_u2ConnEventCounter = 0;

	s_blkConnStateInfo.m_u1TransmitSeqNum = 0;
	s_blkConnStateInfo.m_u1NextExpectedSeqNum = 0;

	BleLinkStateSwitch(BLE_LINK_CONNECTED);	// 链路状态切换到连接态。
	LinkConnSubStateSwitch(CONN_CONNING_RX);	// 切换连接连接子状态到连接中状态。
	
	/* 设置连接过程中的CRC计算初值*/
	u4CrcIV = pu1LLData[LLDATA_CRCINIT_POS] + ((pu1LLData[LLDATA_CRCINIT_POS+1]<<8)&0xFF00) + ((pu1LLData[LLDATA_CRCINIT_POS+2]<<16)&0xFF0000);
	BleRadioCrcInit(u4CrcIV);

	/* 设置该连接使用的接入地址 */
	u4AccAddr = pu1LLData[LLDATA_AA_POS] + ((pu1LLData[LLDATA_AA_POS+1]<<8)&0xFF00) + ((pu1LLData[LLDATA_AA_POS+2]<<16)&0xFF0000) + ((pu1LLData[LLDATA_AA_POS+3]<<24)&0xFF000000);
	BleRadioTxRxAddrCfg(0, u4AccAddr);	// 都是只支持一个连接，所以直接用逻辑0地址

   /* 先计算好免得在每次收到连接事件的第一个数据包时再计算耽误会回响应的速度或者导致计算下次事件的时间不准 */
    u4SleepTimer = s_blkConnStateInfo.m_u2ConnInterval*1250-WIN_WIDEN_ADDITIONAL-SLEEP_INSTANTANEOUS_DEVIATE_TIME-PACKET_EXTEND_WINDOW;
	u4SleepTimer -= GetTimerDeviation(u4SleepTimer, s_blkConnStateInfo.m_u1PeerSCA, s_blkConnStateInfo.m_u1SelfSCA);
	s_blkConnStateInfo.m_u4SleepTimer = u4SleepTimer;
    
    /* 超时也是先计算好，每个连接事件收到数据包了就刷新，timeout单位为10ms，interval单位是1.25ms。 8倍关系 */
	s_blkConnStateInfo.m_u2connTimeoutBackup = s_blkConnStateInfo.m_u2ConnSupervisionTimeout*8/s_blkConnStateInfo.m_u2ConnInterval;
	DEBUG_INFO("tran offset:%d wind size:%d ,connected:%d",s_blkConnStateInfo.m_u2TransmitWindowOffset,s_blkConnStateInfo.m_u1TransmitWindowSize, u4AnchorPoint);
}





void LinkConnStateInit(u1 sca)
{
	memset(&s_blkConnStateInfo, 0x00, sizeof(BlkConnStateInfo));
	s_blkConnStateInfo.m_enConnSubState = CONN_IDLE;		//未连接
	s_blkConnStateInfo.m_u1SelfSCA = sca;
	BleLinkStateHandlerReg(BLE_LINK_CONNECTED, LinkConnRadioEvtHandler);
}

void LinkConnDataHandle()
{
	/*
		规范要求:4.5.1 Connection Events
The slave shall always send a packet if it receives a
packet from the master regardless of a valid CRC match, except after multiple
consecutive invalid CRC matches as specified in Section 4.5.6.		
	*/

	/*
		4.5.7
slave is required to re-synchronize to the master’s anchor point at each connection
event where it listens for the master
If the slave receives a packet from the
master regardless of a CRC match, the slave shall update its anchor point.
	*/
}



