/**
 * @file				NrfDrvRtc.h
 * @brief			NrfDrvRtc.c 头文件
 * @author			fengxun
 * @date			2017年10月31日
*/
#ifndef __NRFDRVRTC_H__
#define __NRFDRVRTC_H__
#include "../DhGlobalHead.h"


#define RTC0_RESOLUTION					(30.517)				/* 30.517 μs RTC0的分辨率*/
#define RTC0_JITTER						(30)					/* 根据手册 RTC的CLEAR STOP START有平均30us的延迟*/
#define RTC_US_TO_COUNT(US)				(((US)-30+RTC0_RESOLUTION/2)/RTC0_RESOLUTION)	/*减去RTC的延迟，并做四舍五入*/

#define SET_CC_ABSOLUTE					(1)
#define SET_CC_RELATIVE					(0)

typedef enum
{
	NRF_CMPREG_0 = 0,
	NRF_CMPREG_1,
	NRF_CMPREG_2,
	NRF_CMPREG_INVALID
}EnNrfCmpReg;

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

extern void NrfRtc0Init(void);
extern void NrfRtc0HandlerRegister(Rtc0IntHandler IntHandler);
extern void NrfRtc0SetCmpReg(EnNrfCmpReg reg, u4 u4TimeoutUs, u1 isAbsolute);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __NRFDRVRTC_H__ */
