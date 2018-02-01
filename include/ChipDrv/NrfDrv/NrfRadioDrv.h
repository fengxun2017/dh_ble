/**
 * @file				RadioDrv.h
 * @brief			NrfRadioDrv.c 的头文件
 * @author			fengxun
 * @date			2017年10月18日
*/
#ifndef __RADIODRV_H__
#define __RADIODRV_H__
#include "../../DhGlobalHead.h"

#define NRF_RADIO_SHORT_DISABLED_TXEN			(1<<2)
#define NRF_RADIO_SHORT_DISABLED_RXEN			(1<<3)

#define NRF_RADIO_SHORT_ADDRESS_RSSISTART		(1<<4)
#define NRF_RADIO_SHORT_DISABLED_RSSISTOP		(1<<8)

typedef enum
{
	NRF_RADIO_EVT_DISABLED = 0,
}EnNrfRadioEvt;

typedef void (*NrfRadioIntHandler)(EnNrfRadioEvt evt);

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 *@brief: 		NrfRadioCrcStatusGet
 *@details:		获得radio当前接收的包的crc校验状态
 *@param[in]	void  

 *@retval:		1-CRC校验通过		0-CRC校验失败
 */
__INLINE u1 NrfRadioCrcStatusGet(void){ return NRF_RADIO->CRCSTATUS&0x01; }


/**
 *@brief: 		NrfRadioRxMatchIndex
 *@details:		获取当前包是在哪个逻辑地址接收到的
 *@param[in]	void  
 
 *@retval:		逻辑地址0-7
 */
__INLINE u1 NrfRadioRxMatchIndexGet(void){return NRF_RADIO->RXMATCH;}


/**
 *@brief: 		NrfRadioPacketPtrCfg
 *@details:		设置radio发送/接收数据包的缓存地址
 *@param[in]	pu1Packet	发送/接收数据包的地址  
 
 *@retval:		void
 */
__INLINE void NrfRadioPacketPtrCfg(u1 *pu1Packet){NRF_RADIO->PACKETPTR = (u4)pu1Packet;}


/**
 *@brief: 		NrfRadioFreqCfg
 *@details:		设置radio的通道频率
 *@param[in]	u2Freq 通道频率

 *@retval:		void
 */
__INLINE void NrfRadioFreqCfg(u2 u2Freq){NRF_RADIO->FREQUENCY = (u2Freq-2400);}

/**
 *@brief: 		NrfRadioTxAddrCfg
 *@details:		配置ble接入地址，0-7 逻辑地址
 *@param[in]	u1LogicAddr  0-7
 
 *@retval:		void
 */
__INLINE void NrfRadioTxAddrCfg(u1 u1LogicAddr){ NRF_RADIO->TXADDRESS = u1LogicAddr;}


/**
 *@brief: 		NrfRadioRxAddrCfg
 *@details:		使能ble的接收接入地址，0-7逻辑地址。
 *@param[in]	u1LogicAddr	0-7逻辑地址
 
 *@retval:		void
 */
__INLINE void NrfRadioRxAddrEnable(u1 u1LogicAddr){NRF_RADIO->RXADDRESSES = 1<<u1LogicAddr;}


/**
 *@brief: 		NrfRadioRxAddrDisable
 *@details:		关闭ble的某个接收接入地址，0-7逻辑地址
 *@param[in]	u1LogicAddr	0-7逻辑地址

 *@retval:		void
 */
__INLINE void NrfRadioRxAddrDisable(u1 u1LogicAddr){NRF_RADIO->RXADDRESSES &= ~(1<<u1LogicAddr);}


/**
 *@brief: 		NrfRadioCrcInitValue
 *@details:		设置radio 的 crc初始值
 *@param[in]	u4CrcInit  crc初值
 
 *@retval:		void
 */
__INLINE void NrfRadioCrcInitValueCfg(u4 u4CrcInit){NRF_RADIO->CRCINIT = (u4CrcInit&0xFFFFFF);}


/**
 *@brief: 		NrfRadioMeasuRssiStart
 *@details:		启动接收信号强度测量

 *@retval:		void
 */
__INLINE void NrfRadioMeasuRssiStart(void){NRF_RADIO->TASKS_RSSISTART = 1;}

/**
 *@brief: 		NrfRadioMeasuRssiStop
 *@details:		关闭接收信号强度测量

 *@retval:		void
 */
__INLINE void NrfRadioMeasuRssiStop(void){NRF_RADIO->TASKS_RSSISTART = 1;}


/**
 *@brief: 		NrfRadioAutoMeasuEnable
 *@details:		打开接收信号强度的自动测量

 *@retval:		void
 */
__INLINE void NrfRssiAutoMeasuEnable(void){NRF_RADIO->SHORTS |=  NRF_RADIO_SHORT_ADDRESS_RSSISTART|NRF_RADIO_SHORT_DISABLED_RSSISTOP;}


/**
 *@brief: 		NrfRadioAutoMeasuDisable
 *@details:		关闭接收信号强度的自动测量

 *@retval:		void
 */
__INLINE void NrfRssiAutoMeasuDisable(void){NRF_RADIO->SHORTS &= ~(NRF_RADIO_SHORT_ADDRESS_RSSISTART|NRF_RADIO_SHORT_DISABLED_RSSISTOP);}

/**
 *@brief: 		IsNrfRadioMeasuRSSIEnd
 *@details:		查询有否有信号强度测量结果

 *@retval:		1--有测量结果		0--无测量结果
 */
__INLINE u1 IsNrfRadioMeasuRssiEnd(void)
{
	if(NRF_RADIO->EVENTS_RSSIEND){NRF_RADIO->EVENTS_RSSIEND = 0; return 1;}
	return 0;
}


/**
 *@brief: 		NrfRadioRssiResultGet
 *@details:		返回RSSI的值

 *@retval:		RSSI的值
 */
__INLINE s1 NrfRadioRssiResultGet(void){return ((s1)NRF_RADIO->RSSISAMPLE)*(-1);}


/**
 *@brief: 		NrfRadioDataWhiteIvCfg
 *@details:		设置ble白化序列初始值
 *@param[in]	IV 初值
 *@retval:		
 */
__INLINE void NrfRadioDataWhiteIvCfg(u1 IV){NRF_RADIO->DATAWHITEIV = IV;}


/**
 *@brief: 		NrfRadioStart
 *@details:		启动radio的发送

 *@retval:		void
 */
__INLINE void NrfRadioStartTx(void){NRF_RADIO->TASKS_TXEN = 1;}


/**
 *@brief: 		NrfRadioStartRx
 *@details:		启动radio的接收

 *@retval:		void
 */
__INLINE void NrfRadioStartRx(void){NRF_RADIO->TASKS_RXEN = 1;}



/**
 *@brief: 		NrfRadioStop
 *@details:		停止radio

 *@retval:		void
 */
__INLINE void NrfRadioDisable(void){NRF_RADIO->TASKS_DISABLE = 1;}



/**
 *@brief: 		NrfRadioAutoToRxEnable
 *@details:		配置radio结束后自动进入接收准备状态，
                这样每次发送完数据后，自动设置 TASK_RXEN，这样在程序从中断发生到中断处理过程中，radio自动进入到发送准备好状态
                之后调用发送函数就可以立刻发送了。

 *@retval:		void
 */
__INLINE void NrfRadioAutoToRxEnable(void){NRF_RADIO->SHORTS |= NRF_RADIO_SHORT_DISABLED_RXEN;}

/**
 *@brief: 		NrfRadioAutoToTxEnable
 *@details:		配置radio结束后自动进入发送准备状态

 *@retval:		void
 */
__INLINE void NrfRadioAutoToTxEnable(void){NRF_RADIO->SHORTS |= NRF_RADIO_SHORT_DISABLED_TXEN;}


/**
 *@brief: 		NrfRadioAutoTx2RxDisable
 *@details:		除能tx发送结束后自动切换进入rx的配置

 *@retval:		void
 */
__INLINE void NrfRadioAutoToRxDisable(void){NRF_RADIO->SHORTS &= ~(NRF_RADIO_SHORT_DISABLED_RXEN);}


/**
 *@brief: 		NrfRadioAutoRx2TxDisable
 *@details:		除能rx接收结束后自动切换进入tx的配置

 *@retval:		void
 */
__INLINE void NrfRadioAutoToTxDisable(void){NRF_RADIO->SHORTS &= ~(NRF_RADIO_SHORT_DISABLED_TXEN);}



extern void NrfLogicAddrCfg(u1 u1LogicAddr, u4 u4PhyAddr);
extern void NrfRadioInit(void);
extern void NrfRadioTxPowerSet(s1 s1TxPower);
extern void NrfRadioHandlerReg(NrfRadioIntHandler intHandler);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __RADIODRV_H__ */
