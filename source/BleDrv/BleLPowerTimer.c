/**
 * @file				BleLPowerTimer
 * @brief			BLE协议栈使用的低功耗时钟相关操作
 * @author			fengxun
 * @date			2017年11月3日
*/

#include "../../include/DhGlobalHead.h"

/**
 *@brief: 		BleLPowerTimerInit
 *@details:		初始化BLE使用的低功耗时钟
 *@param[in]	void  
 *@retval:		void
 */
void BleLPowerTimerInit(void)
{
	NrfRtc0Init();
}
