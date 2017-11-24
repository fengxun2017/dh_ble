/**
 * @file			NrfClockDrv.h
 * @brief			NrfClockDrv.c 的头文件
 * @author			fengxun
 * @date			2017年11月9日
*/
#ifndef __NRFCLOCKDRV_H__
#define __NRFCLOCKDRV_H__

typedef enum
{
	LFCLK_SRC_RC = 0,
	LFCLK_SRC_XTAL,
	LFCLK_SRC_SYNTH,		// 从高频时钟同步而来
}EnNrfLFClockSrc;

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void NrfLFClkStart( EnNrfLFClockSrc src );
extern void NrfHFClkSrcSetXtal(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __NRFCLOCKDRV_H__ */
