/**
 * @file			NrfDrvRtc.c
 * @brief			nrf 的rtc驱动文件,提供nrf芯片底层RTC操作接口，只提供机制，不提供策略.即提供各种细化底层操作，上层
 					通过调用多个接口实现功能。
 * @author			fengxun
 * @date			2017年10月31日
*/
#include "../../include/DhGloablHead.h"

#define NRF_RTC0_ADDRESS                   0x4000B000UL

#define		INTEN_TICK			(1<<0)
#define		INTEN_OVRFLW		(1<<1)
#define		INTEN_COMPARE0		(1<<16)
#define		INTEN_COMPARE1		(1<<17)
#define		INTEN_COMPARE2		(1<<18)
#define		INTEN_COMPARE3		(1<<19)

#define		EVTEN_TICK			(1<<0)
#define		EVTEN_OVRFLW		(1<<1)
#define		EVTEN_COMPARE0		(1<<16)
#define		EVTEN_COMPARE1		(1<<17)
#define		EVTEN_COMPARE2		(1<<18)
#define		EVTEN_COMPARE3		(1<<19)

#define		PRESCALER_VALUE		(0)		// The COUNTER resolution will therefore be 30.517 μs.


typedef struct {                                    /*!< RTC Structure                                                         */
  volatile	u4  TASKS_START;                       /*!< Start RTC Counter.                                                    */
  volatile	u4  TASKS_STOP;                        /*!< Stop RTC Counter.                                                     */
  volatile	u4  TASKS_CLEAR;                       /*!< Clear RTC Counter.                                                    */
  volatile	u4  TASKS_TRIGOVRFLW;                  /*!< Set COUNTER to 0xFFFFFFF0.                                            */
  volatile	u4  RESERVED0[60];
  volatile	u4  EVENTS_TICK;                       /*!< Event on COUNTER increment.                                           */
  volatile	u4  EVENTS_OVRFLW;                     /*!< Event on COUNTER overflow.                                            */
  volatile	u4  RESERVED1[14];
  volatile	u4  EVENTS_COMPARE[4];                 /*!< Compare event on CC[n] match.                                         */
  volatile	u4  RESERVED2[109];
  volatile	u4  INTENSET;                          /*!< Interrupt enable set register.                                        */
  volatile	u4  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  volatile	u4  RESERVED3[13];
  volatile	u4  EVTEN;                             /*!< Configures event enable routing to PPI for each RTC event.            */
  volatile	u4  EVTENSET;                          /*!< Enable events routing to PPI. The reading of this register gives
                                                         the value of EVTEN.                                                   */
  volatile	u4  EVTENCLR;                          /*!< Disable events routing to PPI. The reading of this register
                                                         gives the value of EVTEN.                                             */
  volatile	u4  RESERVED4[110];
  volatile	u4  COUNTER;                           /*!< Current COUNTER value.                                                */
  volatile	u4  PRESCALER;                         /*!< 12-bit prescaler for COUNTER frequency (32768/(PRESCALER+1)).
                                                         Must be written when RTC is STOPed.                                   */
  volatile	u4  RESERVED5[13];
  volatile	u4  CC[4];                             /*!< Capture/compare registers.                                            */
  volatile	u4  RESERVED6[683];
  volatile	u4  POWER;                             /*!< Peripheral power control.                                             */
} BlkNrfRtc;

#define NRF_RTC0                        ((BlkNrfRtc            *) NRF_RTC0_ADDRESS)
#define RTC0_RESOLUTION					(30.517)				/* 30.517 μs 扩大1000倍4 */
#define RTC0_JITTER						(30)					/* 根据手册 RTC的CLEAR STOP START有平均30us的延迟*/

static Rtc0IntHandler S_NrfRtc0IntCb = NULL;
 
/**
 *@brief: 		NrfRtc0Init
 *@details:		nrf rt0初始化
 *@param[in]	void  
 */
void NrfRtc0Init(void)
{
	NRF_RTC0->TASKS_STOP = 1;
	NRF_RTC0->TASK_CLEAR = 1;
	NRF_RTC0->INTENSET = INTEN_COMPARE0|INTEN_COMPARE1|INTEN_COMPARE2|INTEN_COMPARE3;
	NRF_RTC0->EVTENSET = EVTEN_COMPARE0|EVTEN_COMPARE1|EVTEN_COMPARE2|EVTEN_COMPARE3;
	NRF_RTC0->EVENTS_COMPARE[0] = 0;
	NRF_RTC0->EVENTS_COMPARE[1] = 0;
	NRF_RTC0->EVENTS_COMPARE[2] = 0;
	NRF_RTC0->EVENTS_COMPARE[3] = 0;

	NRF_RTC0->PRESCALER = PRESCALER_VALUE;
	
	NVIC_CLEAR
	
}

/**
 *@brief: 		NrfRtc0Register
 *@details:		设置RTC0的中断事件处理函数
 *@param[in]	Rtc0IntHandler 中断事件处理函数  
 *@retval:		void
 */
void NrfRtc0Register ( Rtc0IntHandler )
{
	S_NrfRtc0IntCb = Rtc0IntHandler;
}

/**
 *@brief: 		NrfRtc0Start
 *@details:		启动RTC0开始计数
 *@param[in]	void  
 *@retval:		void 
 */
inline void NrfRtc0Start(void)
{
	NRF_RTC0->TASKS_START = 1;
}

/**
 *@brief: 		NrfRtc0Stop
 *@details:		停止RTC0时钟计数
 *@param[in]	void  
 *@retval:		void
 */
inline void NrfRtc0Stop(void)
{
	NRF_RTC0->TASKS_STOP = 1;
}

/**
 *@brief: 		NrfRtc0Clear
 *@details:		清空RTC0计数
 *@param[in]	void  
 *@retval:		void 
 */
inline void NrfRtc0Clear(void)
{
	NRF_RTC0->TASKS_CLEAR = 1;
}

/**
 *@brief: 		NrfRtc0GetCounter
 *@details:		获取当前RTC0的计数值
 *@param[out]	返回RTC0计数值  
 *@retval:		void
 */
inline void NrfRtc0GetCounter(u4 U4Counter)
{
	return NRF_RTC0->COUNTER;
}

/**
 *@brief: 		NrfRtc0SetCmpReg
 *@details:		设置RTC0的比较寄存器的值
 *@param[in]	En reg    
 				u4 value  0-0xFFFFFF
 *@retval:		void
 */
inline void NrfRtc0SetCmpReg(En reg, u4 value)
{
	u1 regNum;

	regNum = reg;
	NRF_RTC0->CC[regNum] = value;
}


void RTC0_IRQHandler(void)
{
	if ( NRF_RTC0->EVENTS_COMPARE[0] )
	{
		NRF_RTC0->EVENTS_COMPARE[0] = 0;
		if ( NULL != S_NrfRtc0IntCb )
		{
			S_NrfRtc0IntCb(NRF_RTC0_EVT_COMPARE0);
		}
	}
	if ( NRF_RTC0->EVENTS_COMPARE[1] )
	{
		NRF_RTC0->EVENTS_COMPARE[1] = 0;
		if ( NULL != S_NrfRtc0IntCb )
		{
			S_NrfRtc0IntCb(NRF_RTC0_EVT_COMPARE1);
		}
	}
	if ( NRF_RTC0->EVENTS_COMPARE[2] )
	{
		NRF_RTC0->EVENTS_COMPARE[2] = 0;
		if ( NULL != S_NrfRtc0IntCb )
		{
			S_NrfRtc0IntCb(NRF_RTC0_EVT_COMPARE2);
		}
	}
	if ( NRF_RTC0->EVENTS_COMPARE[3] )
	{
		NRF_RTC0->EVENTS_COMPARE[3] = 0;
		if ( NULL != S_NrfRtc0IntCb )
		{
			S_NrfRtc0IntCb(NRF_RTC0_EVT_COMPARE3);
		}
	}	
}
