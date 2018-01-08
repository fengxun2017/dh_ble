/**
 * @file				BleHAccuracyTimer.h
 * @brief			BleHAccuracyTimer.c的头文件
 * @author			fengxun
 * @date			2017年12月1日
*/
#ifndef __BLEHACCURACYTIMER_H__
#define __BLEHACCURACYTIMER_H__

/**@brief	协议栈使用的高精度时钟 
			使用的nrf的timer0，目前好像只需要一个高精度的短时间定时器。就先做一个吧
			PS:timer虽然有4个比较寄存器但是都是用的同一个内部COUNTER计数的。无法做到完全独立。
*/
typedef enum
{
	BLE_HA_TIMER0 = 0,
	BLE_HA_MAX,
	BLE_HA_TIMER_INVALID,
}EnBleHAccuracyTimer;

typedef void (*BleHAccuracyTimeroutHandler)(void * pvalue);

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void BleHAccuracyTimerInit( void );
extern void BleHAccuracyTimerStart( EnBleHAccuracyTimer timerId, u4 timeoutUs, BleHAccuracyTimeroutHandler cb, void *pvalue);
extern void BleHAccuracyTimerStop(EnBleHAccuracyTimer timerId);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLEHACCURACYTIMER_H__ */
