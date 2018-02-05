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

#ifndef __NRFTIMERDRV_H__
#define __NRFTIMERDRV_H__
#include "../../DhGlobalHead.h"

typedef enum
{
	NRF_TIMER_CMPREG_0 = 0,
	NRF_TIMER_CMPREG_1,
	NRF_TIMER_CMPREG_2,
	NRF_TIMER_CMPREG_3,
	NRF_TIMER_CMPREG_INVALID
}EnNrfTimerCmpReg;

typedef enum
{
	NRF_TIMER0_EVT_COMPARE0 = 0,
	NRF_TIMER0_EVT_COMPARE1,
	NRF_TIMER0_EVT_COMPARE2,
	NRF_TIMER0_EVT_COMPARE3,
}EnNrfTimer0Evt;


/* RTC0 的中断回调函数 */
typedef void (*Timer0IntHandler)(EnNrfTimer0Evt event);

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/**
 *@brief: 		NrfTimer0Clear
 *@details:		清空Timer0计数
 *@param[in]	void  
 *@retval:		void 
 */
__INLINE void NrfTimer0Clear(void){NRF_TIMER0->TASKS_CLEAR = 1;}


/**
 *@brief: 		NrfTimer0Start
 *@details:		启动Timer0开始计数
 *@param[in]	void  
 *@retval:		void 
 */
__INLINE void NrfTimer0Start(void){NRF_TIMER0->TASKS_START = 1;}

/**
 *@brief: 		NrfTimer0Stop
 *@details:		停止Timer0时钟计数
 *@param[in]	void  
 *@retval:		void
 */
__INLINE void NrfTimer0Stop(void){NRF_TIMER0->TASKS_STOP = 1;}

extern void NrfTimer0Init(void);
extern void NrfTimer0HandlerRegister(Timer0IntHandler IntHandler);
extern void NrfTimer0SetCmpReg(EnNrfTimerCmpReg reg, u4 u4TimeoutUs);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __NRFDRVRTC_H__ */
