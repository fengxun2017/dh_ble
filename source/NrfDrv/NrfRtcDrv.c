/**
 * @file			NrfDrvRtc.c
 * @brief			nrf 的rtc驱动文件,提供nrf芯片底层RTC操作接口，只提供机制，不提供策略.即提供各种细化底层操作，上层
 					通过调用多个接口实现功能。
 * @author			fengxun
 * @date			2017年10月31日
*/
#include "../../include/DhGlobalHead.h"


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

u4 GET_CMP_OFFSET[] = {EVTEN_COMPARE0,EVTEN_COMPARE1,EVTEN_COMPARE2};
static Rtc0IntHandler S_NrfRtc0IntCb = NULL;
 
/**
 *@brief: 		NrfRtc0Init
 *@details:		nrf rt0初始化
 *@param[in]	void  
 */
void NrfRtc0Init(void)
{
	NRF_RTC0->TASKS_STOP = 1;
	NRF_RTC0->TASKS_CLEAR = 1;
	NRF_RTC0->INTENSET = INTEN_COMPARE0|INTEN_COMPARE1|INTEN_COMPARE2;
	NRF_RTC0->EVTEN = 0;//EVTEN_COMPARE0|EVTEN_COMPARE1|EVTEN_COMPARE2;
	NRF_RTC0->EVENTS_COMPARE[0] = 0;
	NRF_RTC0->EVENTS_COMPARE[1] = 0;
	NRF_RTC0->EVENTS_COMPARE[2] = 0;

	NRF_RTC0->PRESCALER = PRESCALER_VALUE;

	/* 协议栈的低功耗唤醒定时器，目前先设置最高中断优先级吧 */
	NVIC_SetPriority(RTC0_IRQn, DH_IRQ_PRIORITY_0);	
	NVIC_ClearPendingIRQ(RTC0_IRQn);
	NVIC_EnableIRQ(RTC0_IRQn);
}

/**
 *@brief: 		NrfRtc0HandlerRegister
 *@details:		设置RTC0的中断事件处理函数
 *@param[in]	IntHandler 中断事件处理函数  
 *@retval:		void
 */
void NrfRtc0HandlerRegister ( Rtc0IntHandler IntHandler)
{
	S_NrfRtc0IntCb = IntHandler;
}

/**
 *@brief: 		NrfRtc0SetCmpReg
 *@details:		设置RTC0的比较寄存器的值
 *@param[in]	reg    		需要配置的RTC0的比较寄存器
 *@param[in]	u4TimeoutUs	不应该超过500s
 *@param[in]	isAbsolute	是否是设置绝对值，绝对值则直接设置cc寄存器，否则为相对值，需要先获取当前的COUNT值相加后设置cc寄存器的值
 *@retval:		void
 */
void NrfRtc0SetCmpReg(EnNrfCmpReg reg, u4 u4TimeoutUs, u1 isAbsolute)
{
	u1 regNum;
	u4 curCounter = 0;
	
	regNum = reg;
	NRF_RTC0->EVTENSET = GET_CMP_OFFSET[reg];
	if( SET_CC_RELATIVE == isAbsolute )
	{
		curCounter = NRF_RTC0->COUNTER;
		DEBUG_INFO("current counter:%d",curCounter);
	}
	NRF_RTC0->CC[regNum] =curCounter + RTC_US_TO_COUNT(u4TimeoutUs);
	DEBUG_INFO("set cc:%d",NRF_RTC0->CC[regNum]);
}


void RTC0_IRQHandler(void)
{
	if ( NRF_RTC0->EVENTS_COMPARE[0] )
	{
		NRF_RTC0->EVENTS_COMPARE[0] = 0;
		NRF_RTC0->EVTENCLR |= EVTEN_COMPARE0;	// 产生事件后就除能产生事件，使定时器只能作用一次。
		if ( NULL != S_NrfRtc0IntCb )
		{
			S_NrfRtc0IntCb(NRF_RTC0_EVT_COMPARE0);
		}
	}
	if ( NRF_RTC0->EVENTS_COMPARE[1] )
	{
		NRF_RTC0->EVENTS_COMPARE[1] = 0;
		NRF_RTC0->EVTENCLR |= EVTEN_COMPARE1;
		if ( NULL != S_NrfRtc0IntCb )
		{
			S_NrfRtc0IntCb(NRF_RTC0_EVT_COMPARE1);
		}
	}
	if ( NRF_RTC0->EVENTS_COMPARE[2] )
	{
		NRF_RTC0->EVENTS_COMPARE[2] = 0;
		NRF_RTC0->EVTENCLR |= EVTEN_COMPARE2;
		if ( NULL != S_NrfRtc0IntCb )
		{
			S_NrfRtc0IntCb(NRF_RTC0_EVT_COMPARE2);
		}
	}
	if ( NRF_RTC0->EVENTS_COMPARE[3] )	/* 手册说RTC0只有3个比较寄存器，应该不会有这个中断的 */
	{
		NRF_RTC0->EVENTS_COMPARE[3] = 0;
		NRF_RTC0->EVTENCLR |= EVTEN_COMPARE3;
		
	}	
}
