/**
 * @file				NrfTimerDrv.h
 * @brief			NrfTimerDrv.c 头文件
 * @author			fengxun
 * @date			2017年11月30日
*/
#ifndef __NRFTIMERDRV_H__
#define __NRFTIMERDRV_H__
#include "../DhGlobalHead.h"

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
