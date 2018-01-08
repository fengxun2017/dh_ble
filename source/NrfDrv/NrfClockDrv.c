/**
 * @file			NrfClockDrv.c
 * @brief			nrf51的时钟控制
 * @author			fengxun
 * @date			2017年11月9日
*/
#include "../../include/DhGlobalHead.h"

#define nDEBUG_CLOCK_ENABLE

#if !defined(DEBUG_CLOCK_ENABLE)
#undef DEBUG_INFO
#define DEBUG_INFO(...)
#endif

#define XTALFREQ_16MHZ		(0xff)
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

	NVIC_SetPriority(POWER_CLOCK_IRQn, DH_IRQ_PRIORITY_3);	
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
void NrfLFClkStart( EnNrfLFClockSrc src , u4 u4CalTimeoutMs)
{
	NRF_CLOCK->LFCLKSRC = src;
	NRF_CLOCK->TASKS_LFCLKSTART = TASK_START;
	while( !(NRF_CLOCK->LFCLKSTAT&(1<<LFCLK_STAT_POS)) );	// 等待时钟启动

	if(LFCLK_SRC_RC == src )
	{
		NrfRcOscCalStart(u4CalTimeoutMs);	//启动校准
	}
}



/**
 *@brief: 		NrfHFClkSrcSetXtal
 *@details:		配置高频时钟源为外部16M晶振

 *@retval:		void
 */
void NrfHFClkSrcSetXtal(void)
{
	NRF_CLOCK->XTALFREQ = XTALFREQ_16MHZ;
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART = TASK_START;
	// 等待启振完成
	while(1)
	{
		if(1 == NRF_CLOCK->EVENTS_HFCLKSTARTED)
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
