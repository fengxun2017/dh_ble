
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


#define nDEBUG_NRF_RTC0_ENABLE

#ifndef DEBUG_NRF_RTC0_ENABLE
#undef SEGGER_SYSVIEW_PrintfHost
#undef SEGGER_SYSVIEW_RecordEnterISR
#undef SEGGER_SYSVIEW_RecordExitISR

#define SEGGER_SYSVIEW_PrintfHost(...)
#define SEGGER_SYSVIEW_RecordEnterISR(...)
#define	SEGGER_SYSVIEW_RecordExitISR(...)
#endif



#define		INTEN_COMPARE0		(1<<16)
#define		INTEN_COMPARE1		(1<<17)
#define		INTEN_COMPARE2		(1<<18)
#define		INTEN_COMPARE3		(1<<19)

#define		EVTEN_COMPARE0		(1<<16)
#define		EVTEN_COMPARE1		(1<<17)
#define		EVTEN_COMPARE2		(1<<18)
#define		EVTEN_COMPARE3		(1<<19)

#define		PRESCALER_VALUE		(0)		// The COUNTER resolution will therefore be 30.517 μs.

#define RTC0_RESOLUTION					(30.517)				/* 30.517 μs RTC0的分辨率*/

/*	根据手册 RTC的CLEAR STOP START命令的实际生效最大可能有46us的延迟，
* 	如果RTC 在LFCLK启动之前启动，则抖动最大有250us，所以实际使用需要先明确启动LFCLK
*/
#define RTC0_JITTER						(46)		

#define RTC_US_TO_COUNT(US)				(((US)+RTC0_RESOLUTION/2)/RTC0_RESOLUTION)	/* 四舍五入 */

static u4 GET_CMP_OFFSET[] = {EVTEN_COMPARE0,EVTEN_COMPARE1,EVTEN_COMPARE2};
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
	
	// 使能COMPARE事件发生时触发对应中断
	NRF_RTC0->INTENSET = INTEN_COMPARE0|INTEN_COMPARE1|INTEN_COMPARE2;
	
	// COMPARE事件，先全部屏蔽，后续使用哪个再开启哪个
	NRF_RTC0->EVTEN = 0;

	// 启动时清除事件标记
	NRF_RTC0->EVENTS_COMPARE[0] = 0;
	NRF_RTC0->EVENTS_COMPARE[1] = 0;
	NRF_RTC0->EVENTS_COMPARE[2] = 0;
	NRF_RTC0->EVENTS_COMPARE[3] = 0;

	// 不分频，RTC 时间精度为1/32768 秒
	NRF_RTC0->PRESCALER = PRESCALER_VALUE;

	/* 协议栈的低功耗唤醒定时器，设置最高中断优先级吧 */
	NVIC_SetPriority(RTC0_IRQn, 0xff);	
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
 *@param[in]	isAbsolute	是否是设置绝对值，绝对值则直接设置cc寄存器，
 							否则为相对值，需要先获取当前的COUNT值相加后设置cc寄存器的值
							上层调用自己保证，相对值不会溢出计数器的范围
 *@retval:		void
 */
void NrfRtc0SetCmpReg(EnNrfRtcCmpReg reg, u4 u4TimeoutUs, u1 isAbsolute)
{
	u1 regNum;
	u4 curCounter = 0;
	
	regNum = reg;
	NRF_RTC0->EVTEN |= GET_CMP_OFFSET[reg];
	if( SET_RTC_CC_RELATIVE == isAbsolute )
	{
		curCounter = NRF_RTC0->COUNTER;
	}
	NRF_RTC0->CC[regNum] =curCounter + RTC_US_TO_COUNT(u4TimeoutUs);
}

/**
 *@brief: 		NrfRtc0DisableCmpReg
 *@details:		关闭某个比较器的中断功能
 *@param[in]	reg  比较器寄存器
 
 *@retval:		void
 */
void NrfRtc0DisableCmpReg(EnNrfRtcCmpReg reg)
{
		NRF_RTC0->EVTEN &= ~GET_CMP_OFFSET[reg];
}

/**
 *@brief: 		NrfRtc0CounterGet
 *@details:		获取当前的Rtc0 counter值
 *@retval:		void
 */
u4 NrfRtc0CounterGet(void)
{
	return NRF_RTC0->COUNTER;
}


void RTC0_IRQHandler(void)
{
	u4	regBackup;
	
	regBackup = NRF_RTC0->EVTEN;
	if ( NRF_RTC0->EVENTS_COMPARE[0] )
	{
		NRF_RTC0->EVENTS_COMPARE[0] = 0;
		NRF_RTC0->EVTEN &= ~EVTEN_COMPARE0;	// 产生事件后就除能产生事件，使定时器只作用一次。
		if ( NULL!=S_NrfRtc0IntCb && regBackup&EVTEN_COMPARE0 )
		{
			S_NrfRtc0IntCb(NRF_RTC0_EVT_COMPARE0);
		}
	}
	if ( NRF_RTC0->EVENTS_COMPARE[1] )
	{
		NRF_RTC0->EVENTS_COMPARE[1] = 0;
		NRF_RTC0->EVTEN &= ~EVTEN_COMPARE1;	// 产生事件后就除能产生事件，使定时器只作用一次。
		
		if ( NULL != S_NrfRtc0IntCb && regBackup&EVTEN_COMPARE1 )
		{
			S_NrfRtc0IntCb(NRF_RTC0_EVT_COMPARE1);
		}
		
	}
	if ( NRF_RTC0->EVENTS_COMPARE[2] )
	{
		NRF_RTC0->EVENTS_COMPARE[2] = 0;
		NRF_RTC0->EVTEN &= ~EVTEN_COMPARE2;
		if ( NULL != S_NrfRtc0IntCb && regBackup&EVTEN_COMPARE2 )
		{
			S_NrfRtc0IntCb(NRF_RTC0_EVT_COMPARE2);
		}
		
	}
}
