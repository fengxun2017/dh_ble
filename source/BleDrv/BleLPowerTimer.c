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


#include "../../include/DhGlobalHead.h"


typedef struct
{
    BleLPowerTimeroutHandler m_cb;		/* 回调函数 */
    void *m_pValue;						/* 传给回调函数的参数 */
} BlkLPTimerCb;


typedef struct
{
    BlkLPTimerCb m_pblkLPTimer[BLE_LP_MAX];
} BlkLPTimerManager;

BlkLPTimerManager s_blkBleLPTimer;

/**
 *@brief: 		RtcEvt2LPTimerId
 *@details:		由rtc的比较事件获取到是哪个timer产生了超时
 *@param[in]	evtId  
 *@retval:		timerId
 */
static EnBleLPowerTimer RtcEvt2LPTimerId( EnNrfRtc0Evt evtId )
{
    if( NRF_RTC0_EVT_COMPARE0 == evtId )
    {
        return BLE_LP_TIMER0;
    }
    else if ( NRF_RTC0_EVT_COMPARE1 == evtId )
    {
        return BLE_LP_TIMER1;
    }
    else if( NRF_RTC0_EVT_COMPARE2 == evtId )
    {
        return BLE_LP_TIMER2;
    }

    return BLE_LP_TIMER_INVALID;
}

/**
 *@brief: 		RtcTimerid2CmpReg
 *@details:		由timerid获取对应的比较寄存器
 *@param[in]	timerId  
 *@retval:		CmpReg
 */
static EnNrfRtcCmpReg RtcTimerid2CmpReg(EnBleLPowerTimer timerId)
{
	if( BLE_LP_TIMER0 == timerId )
	{
		return NRF_RTC_CMPREG_0;
	}
	else if( BLE_LP_TIMER1 == timerId )
	{
		return NRF_RTC_CMPREG_1;
	}
	else if( BLE_LP_TIMER2 == timerId )
	{
		return NRF_RTC_CMPREG_2;
	}

	return NRF_RTC_CMPREG_INVALID;
}

/**
 *@brief: 		Rtc0IntCb
 *@details:		注册的RTC0的中断回调函数，中断发生后，RTC0调-
             	用该回调函数，参数为中断事件
 *@param[in]	evt  
 *@retval:		void
 */
static void Rtc0IntCb( EnNrfRtc0Evt evt )
{
    BleLPowerTimeroutHandler cb = NULL;
    void *pvalue;
    u1 timerId;

    timerId = RtcEvt2LPTimerId(evt);
    
    if ( BLE_LP_TIMER_INVALID != timerId )
    {
    	cb = s_blkBleLPTimer.m_pblkLPTimer[timerId].m_cb;
    	pvalue = s_blkBleLPTimer.m_pblkLPTimer[timerId].m_pValue;
	}
    if ( NULL != cb )
    {
		cb(pvalue);
    }
   
}


/**
 *@brief: 		BleLPowerTimerInit
 *@details:		初始化BLE使用的低功耗时钟
 *@param[in]	src	低功耗时钟的时钟源，
 *@param[in]	u4CalTimeout  时钟源为LP_CLK_SRC_RC时的内部RC校准周期
 
 *@retval:		void
 */
void BleLPowerTimerInit( EnBleLPowerClkSrc src, u4 u4CalTimeoutMs )
{
    memset( &s_blkBleLPTimer, 0x00, sizeof( s_blkBleLPTimer ) );
	if( LP_CLK_SRC_RC_250_PPM == src)
	{
    	NrfLFClkStart(LFCLK_SRC_RC, u4CalTimeoutMs);
    }
    NrfRtc0HandlerRegister( Rtc0IntCb );
    NrfRtc0Init();
}


/*
	利用RTC实现3个ble需要使用的低功耗时钟
	LP_TIMER0: 用来定时长时间的定时器(几十ms-几十s)，使用前会重启底层硬件RTC，使用RTC0的 compare0 寄存器实现,不能超过500s
	LP_TIMER1: 用来定时短时间的定时器(几百us-几ms)，在长定时器启动的情况下才能使用，使用RTC0的 compare1 寄存器实现
	LP_TIMER2: 同LP_TIMER1，使用RTC0的 compare2 寄存器实现
*/
/**
 *@brief: 		BleLPowerTimerStart
 *@details:		启动ble的某个低功耗定时器
 *@param[in]	timerId 			需要启动的ble低功耗时钟id，BLE_LP_TIMER0-2      
 *@param[in]	timeoutUs		定时器超时时间       
 *@param[in]	cb				定时器超时后的回调函数
 *@param[in]	pvalue			回调函数触发时会获取到该值，该值为传递的是地址，所以需要保证运行期间传递的变量的有效性。  
 
 *@retval:		u4				成功返回0
 */
u4 BleLPowerTimerStart( EnBleLPowerTimer timerId, u4 timeoutUs, BleLPowerTimeroutHandler cb, void *pvalue)
{	
	if( timerId >= BLE_LP_MAX)
	{
		return ERR_LP_TIMER_BASE+0x01;
	}
	s_blkBleLPTimer.m_pblkLPTimer[timerId].m_cb = cb;
	s_blkBleLPTimer.m_pblkLPTimer[timerId].m_pValue = pvalue;
	
		/* 如果是 BLE_LP_TIMER0 ，则复位rtc状态让其从0开始计数*/
	if( BLE_LP_TIMER0 == timerId )
	{
		NrfRtc0Stop();
		NrfRtc0Clear();
		NrfRtc0SetCmpReg(NRF_RTC_CMPREG_0, timeoutUs, SET_RTC_CC_ABSOLUTE);
		NrfRtc0Start();
	}
		/* 
			如果是BLE_LP_TIMER1 或 BLE_LP_TIMER2,则在协议栈中用是在BLE_LP_TIMER长定时器中定义更短的定时器来处理其中的其他超时
			所以使用时不需要复位硬件RTC0,只需读取当前的计数值，然后加上超时值后配置CC比较寄存器，因为是在BLE_LP_TIMER0的使用
			范围内使用的区间更小的定时器，所以不会出现当前值加上超时值后溢出的情况
		*/
	else if( BLE_LP_TIMER_INVALID != timerId )
	{
		NrfRtc0SetCmpReg(RtcTimerid2CmpReg(timerId), timeoutUs, SET_RTC_CC_RELATIVE);
	}

	return DH_SUCCESS;
}


/**
 *@brief: 		BleLPowerTimerStop
 *@details:		地址低功耗定时器
 *@param[in]	timerId  
 
 *@retval:		void
 */
u4 BleLPowerTimerStop( EnBleLPowerTimer timerId)
{
	if( BLE_LP_TIMER0 == timerId )
	{
		NrfRtc0Stop();
	}
	else
	{
		NrfRtc0DisableCmpReg(RtcTimerid2CmpReg(timerId));
	}

	return DH_SUCCESS;
}

/**
 *@brief: 		BleLPowerTimerTickGet
 *@details:		获取低功耗时钟的底层的当前滴答数。用来在开发
 				过程中测试一些过程的耗时而已
 *@retval:		tick
 */
u4 BleLPowerTimerTickGet(void)
{
	return NrfRtc0CounterGet();
}
