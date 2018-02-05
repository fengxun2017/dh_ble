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

#ifndef __NRFDRVRTC_H__
#define __NRFDRVRTC_H__
#include "../../DhGlobalHead.h"


#define SET_RTC_CC_ABSOLUTE					(1)
#define SET_RTC_CC_RELATIVE					(0)

typedef enum
{
	NRF_RTC_CMPREG_0 = 0,
	NRF_RTC_CMPREG_1,
	NRF_RTC_CMPREG_2,
	NRF_RTC_CMPREG_INVALID
}EnNrfRtcCmpReg;

typedef enum
{
	NRF_RTC0_EVT_COMPARE0 = 0,
	NRF_RTC0_EVT_COMPARE1,
	NRF_RTC0_EVT_COMPARE2,
}EnNrfRtc0Evt;


/* RTC0 的中断回调函数 */
typedef void (*Rtc0IntHandler)(EnNrfRtc0Evt event);

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/**
 *@brief: 		NrfRtc0Clear
 *@details:		清空RTC0计数
 *@param[in]	void  
 *@retval:		void 
 */
__INLINE void NrfRtc0Clear(void){NRF_RTC0->TASKS_CLEAR = 1;}


/**
 *@brief: 		NrfRtc0Start
 *@details:		启动RTC0开始计数
 *@param[in]	void  
 *@retval:		void 
 */
__INLINE void NrfRtc0Start(void){NRF_RTC0->TASKS_START = 1;}

/**
 *@brief: 		NrfRtc0Stop
 *@details:		停止RTC0时钟计数
 *@param[in]	void  
 *@retval:		void
 */
__INLINE void NrfRtc0Stop(void){NRF_RTC0->TASKS_STOP = 1;}

extern u4 NrfRtc0CounterGet(void);
extern void NrfRtc0Init(void);
extern void NrfRtc0HandlerRegister(Rtc0IntHandler IntHandler);
extern void NrfRtc0SetCmpReg(EnNrfRtcCmpReg reg, u4 u4TimeoutUs, u1 isAbsolute);
extern void NrfRtc0DisableCmpReg(EnNrfRtcCmpReg reg);

extern void NrfRtc0HandlerRegister ( Rtc0IntHandler IntHandler);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __NRFDRVRTC_H__ */
