/**
 * @file			NrfClockDrv.c
 * @brief			nrf51的时钟控制
 * @author			fengxun
 * @date			2017年11月9日
*/
#include "../../include/DhGlobalHead.h"


/**
 *@brief: 		NrfLFClkStart
 *@details:		启动低频时钟
 *@param[in]	src		低频时钟源 
 *@retval:		void
 */
void NrfLFClkStart( EnNrfLFClockSrc src )
{
	NRF_CLOCK->LFCLKSRC = src;
	NRF_CLOCK->TASKS_LFCLKSTART = 1;
	while( !(NRF_CLOCK->LFCLKSTAT&(1<<16)) );	// 等待时钟启动
}


/**
 *@brief: 		NrfHFClkSrcSetXtal
 *@details:		配置高频时钟源为外部16M晶振

 *@retval:		void
 */
void NrfHFClkSrcSetXtal(void)
{
	NRF_CLOCK->XTALFREQ = 0xFF;		// 使用16M晶振
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART = 1;
	// 等待启振完成
	while(1)
	{
		if(1 == NRF_CLOCK->EVETNS_HFCLKSTARTED)
		{
			break;
		}
	}
}
