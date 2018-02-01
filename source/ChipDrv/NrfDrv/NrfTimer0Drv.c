/**
 * @file			NrfTimer0Drv.c
 * @brief			nrf 的time0驱动文件,提供nrf芯片底层time0操作接口，只提供机制，不提供策略.即提供各种细化底层操作，上层
 					通过调用多个接口实现功能。
 * @author			fengxun
 * @date			2017年11月30日
*/
#include "../../include/DhGlobalHead.h"



#define		INTEN_COMPARE0		(1<<16)
#define		INTEN_COMPARE1		(1<<17)
#define		INTEN_COMPARE2		(1<<18)
#define		INTEN_COMPARE3		(1<<19)

#define		TIMER_MODE			(0)
#define		COUNTER_MODE		(1)

#define		BIT_MODE_8			(1)
#define		BIT_MODE_16			(0)
#define		BIT_MODE_24			(2)
#define		BIT_MODE_32			(3)

#define		PRESCALER_VALUE		(4)		// fTIMER = 16 MHz / (2^PRESCALER)		1us为计时单位

#define TIMER0_RESOLUTION					(1)				/* 1μs TIMER0的分辨率*/
#define RTC_US_TO_COUNT(US)					(US)//((US)/TIMER0_RESOLUTION)

static u4 GET_CMP_OFFSET[] = {INTEN_COMPARE0,INTEN_COMPARE1,INTEN_COMPARE2,INTEN_COMPARE3};

static Timer0IntHandler S_NrfTimer0IntCb = NULL;
 
/**
 *@brief: 		NrfTimer0Init
 *@details:		nrf timer0初始化
 *@param[in]	void  
 */
void NrfTimer0Init(void)
{
	NRF_TIMER0->TASKS_STOP = 1;
	NRF_TIMER0->TASKS_CLEAR = 1;
	NRF_TIMER0->INTENCLR = INTEN_COMPARE0|INTEN_COMPARE1|INTEN_COMPARE2;
	NRF_TIMER0->EVENTS_COMPARE[0] = 0;
	NRF_TIMER0->EVENTS_COMPARE[1] = 0;
	NRF_TIMER0->EVENTS_COMPARE[2] = 0;
	NRF_TIMER0->MODE = TIMER_MODE;
	NRF_TIMER0->BITMODE = BIT_MODE_32;
	NRF_TIMER0->PRESCALER = PRESCALER_VALUE;	

	/* 协议栈的高精度定时器，目前先设置最高中断优先级吧 */
	NVIC_SetPriority(TIMER0_IRQn, DH_IRQ_PRIORITY_0);	
	NVIC_ClearPendingIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);
}

/**
 *@brief: 		NrfTimer0HandlerRegister
 *@details:		设置TIMER0的中断事件处理函数
 *@param[in]	IntHandler 中断事件处理函数  
 *@retval:		void
 */
void NrfTimer0HandlerRegister ( Timer0IntHandler IntHandler)
{
	S_NrfTimer0IntCb = IntHandler;
}

/**
 *@brief: 		NrfTimer0SetCmpReg
 *@details:		设置TIMER0的比较寄存器的值
 *@param[in]	reg    		需要配置的TIMER0的比较寄存器
 *@param[in]	u4TimeoutUs	超时时间us，0-0xFFFFFFFF

 *@retval:		void
 */
void NrfTimer0SetCmpReg(EnNrfTimerCmpReg reg, u4 u4TimeoutUs)
{
	u1 regNum;
	
	regNum = reg;
	NRF_TIMER0->CC[regNum] = RTC_US_TO_COUNT(u4TimeoutUs);
	NRF_TIMER0->INTENSET = GET_CMP_OFFSET[reg];
}


void TIMER0_IRQHandler(void)
{
	if ( NRF_TIMER0->EVENTS_COMPARE[0] )
	{
		NRF_TIMER0->EVENTS_COMPARE[0] = 0;
		NRF_TIMER0->INTENCLR |= INTEN_COMPARE0;	// 产生事件后就除能产生中断，定时器只作用一次。

		if ( NULL!=S_NrfTimer0IntCb )
		{
			S_NrfTimer0IntCb(NRF_TIMER0_EVT_COMPARE0);
		}		
	}
	if ( NRF_TIMER0->EVENTS_COMPARE[1] )
	{
		NRF_TIMER0->EVENTS_COMPARE[1] = 0;
		NRF_TIMER0->INTENCLR |= INTEN_COMPARE1;
		if ( NULL != S_NrfTimer0IntCb )
		{
			S_NrfTimer0IntCb(NRF_TIMER0_EVT_COMPARE1);
		}
		
	}
	if ( NRF_TIMER0->EVENTS_COMPARE[2] )
	{
		NRF_TIMER0->EVENTS_COMPARE[2] = 0;
		NRF_TIMER0->INTENCLR |= INTEN_COMPARE2;
		if ( NULL != S_NrfTimer0IntCb )
		{
			S_NrfTimer0IntCb(NRF_TIMER0_EVT_COMPARE2);
		}
	}
	if ( NRF_TIMER0->EVENTS_COMPARE[3] )	
	{
		NRF_TIMER0->EVENTS_COMPARE[3] = 0;
		NRF_TIMER0->INTENCLR |= INTEN_COMPARE3;
		if ( NULL != S_NrfTimer0IntCb )
		{
			S_NrfTimer0IntCb(NRF_TIMER0_EVT_COMPARE3);
		}
	}	
}



