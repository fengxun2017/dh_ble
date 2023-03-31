
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

#define nDEBUG_CLOCK_ENABLE

#if !defined(DEBUG_CLOCK_ENABLE)
#undef DEBUG_INFO
#define DEBUG_INFO(...)
#endif

#define TASK_START			(0x01)
#define LFCLK_STAT_POS		(16)
#define HFCLK_SRC_POS		(0)
#define HFCLK_SRC_XTAL		(1)
#define HFCLK_SRC_RC		(0)

#define INTENSET_CTTO		(1<<4)
#define INTENSET_DONE		(1<<3)

#define CAL_TIME_UNIT_MS	(250)

u4	g_CalTimeout = 0;


static u1 IsHFCLKSrcXtal(void)
{
	if( NRF_CLOCK->HFCLKSTAT&(1<<HFCLK_SRC_POS) )
	{
		return HFCLK_SRC_XTAL;
	}

	return HFCLK_SRC_RC;
}

static u1 IsHFCLKRunning(void) {
    if ((NRF_CLOCK->HFCLKSTAT & 0xff) == 1) {
        return 1;
    }
    return 0;
}

/**
 *@brief: 		NrfRcOscCalStart
 *@details:		启动内部rc震荡器的周期校准
 *@param[in]	timeoutMs   校准周期		0.25s-31.75s
 *@retval:		void
 */
static void NrfRcOscCalStart(u4 timeoutMs )
{
	DEBUG_INFO("start rc cal");
	NRF_CLOCK->INTENSET = INTENSET_CTTO | INTENSET_DONE;
	NRF_CLOCK->CTIV = timeoutMs/CAL_TIME_UNIT_MS;

	// 校准定时器，设置个低优先级就好了
	NVIC_SetPriority(POWER_CLOCK_IRQn, 3);	
	NVIC_ClearPendingIRQ(POWER_CLOCK_IRQn);
	NVIC_EnableIRQ(POWER_CLOCK_IRQn);

	NRF_CLOCK->TASKS_CTSTART = TASK_START;

	g_CalTimeout = timeoutMs;
}

/**
 *@brief: 		NrfLFClkStart
 *@details:		启动低频时钟
 *@param[in]	src		低频时钟源 
 *@retval:		void
 */
void NrfLFClkStart( EnNrfLFClockSrc src , u4 u4CalTimeoutMs, u1 enableCal)
{
	NRF_CLOCK->LFCLKSRC = src;
	
	NRF_CLOCK->TASKS_LFCLKSTART = TASK_START;
	while( !(NRF_CLOCK->LFCLKSTAT&(1<<LFCLK_STAT_POS)) );	// 等待时钟启动

    // 使用内部RC震荡作为 LFCLK 时钟源时，需要定期校准。
	if(LFCLK_SRC_RC == src && enableCal)
	{
		NrfRcOscCalStart(u4CalTimeoutMs);	//启动校准
	}
}



/**
 *@brief: 		NrfHFClkSrcSetXtal
 *@details:		配置高频时钟源为外部32M晶振，产生MCU需要的64M时钟

 *@retval:		void
 */
void NrfHFClkSrcSetXtal(void)
{
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;

    // 52840 内部高频时钟（64M HFCLK）来源有内部RC振荡器（64M HFINT）和晶体振荡器（64M HFXO，实际由外部32M物理晶振驱动获得）
    // 上电后，不做配置，即默认使用内部RC振荡器（HFINT）
    // 设置 TASKS_HFCLKSTART 寄存器来启动HFCLK，即表示使用HFXO（硬件上需要外接32M晶振）
	NRF_CLOCK->TASKS_HFCLKSTART = TASK_START;
	
    // 等待晶振启振完成，HFCLK运行
	while(1)
	{
		if ((1 == NRF_CLOCK->EVENTS_HFCLKSTARTED) 
            && IsHFCLKRunning() == 1 )
		{
			break;
		}
	}
}


void POWER_CLOCK_IRQHandler(void)
{
	if( NRF_CLOCK->EVENTS_CTTO )
	{
		NRF_CLOCK->EVENTS_CTTO = 0;
		if( HFCLK_SRC_XTAL == IsHFCLKSrcXtal() )
		{
			/* 使用了外部高频晶振时才能校准 */
			NRF_CLOCK->TASKS_CAL = TASK_START;
			DEBUG_INFO("cal....");
		}
		else
		{
			/* 重启校准定时器 */
			NRF_CLOCK->CTIV = g_CalTimeout/CAL_TIME_UNIT_MS;
			NRF_CLOCK->TASKS_CTSTART = TASK_START;
		}
	}
	if( NRF_CLOCK->EVENTS_DONE )
	{
		NRF_CLOCK->EVENTS_DONE = 0;
		/* 重启校准定时器 */
		NRF_CLOCK->CTIV = g_CalTimeout/CAL_TIME_UNIT_MS;
		NRF_CLOCK->TASKS_CTSTART = TASK_START;
		DEBUG_INFO("cal done");
		DEBUG_INFO("start cal time");
	}
}