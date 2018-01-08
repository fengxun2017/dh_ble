/**
 * @file				BleLinkConnect.h
 * @brief			BleLinkConnect.c µÄÍ·ÎÄ¼þ
 * @author			fengxun
 * @date			2017å¹´12æœˆ26æ—¥
*/
#ifndef __BLELINKCONNECT_H__
#define __BLELINKCONNECT_H__
#include "../../DhGlobalHead.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void LinkConnReqHandle(u1 addrType, u1 *pu1Addr, u1* pu1LLData);
extern void LinkConnSelfSCACfg(u1 sca);
extern void LinkConnStateInit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLELINKCONNECT_H__ */
