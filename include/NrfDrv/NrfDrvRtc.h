/**
 * @file				NrfDrvRtc.h
 * @brief			NrfDrvRtc.c 头文件
 * @author			fengxun
 * @date			2017年10月31日
*/
#ifndef __NRFDRVRTC_H__
#define __NRFDRVRTC_H__
#include "../DhGlobalHead.h"

typedef enum
{
	NRF_CMPREG_0 = 0,
	NRF_CMPREG_1,
	NRF_CMPREG_2,
	NRF_CMPREG_3,
}EnNrfCmpReg;

typedef enum
{
	NRF_RTC0_EVT_COMPARE0 = 0,
	NRF_RTC0_EVT_COMPARE1,
	NRF_RTC0_EVT_COMPARE2,
	NRF_RTC0_EVT_COMPARE3,
}EnNrfRtc0Evt;


/* RTC0 的中断回调函数 */
typedef void (*Rtc0IntHandler)(EnNrfRtc0Evt event);

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern inline void NrfRtc0Clear(void);
extern inline void NrfRtc0GetCounter(u4 U4Counter);
extern void NrfRtc0Init(void);
extern void NrfRtc0Register(Rtc0IntHandler);
extern inline void NrfRtc0SetCmpReg(En reg, u4 value);
extern inline void NrfRtc0Start(void);
extern inline void NrfRtc0Stop(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __NRFDRVRTC_H__ */
