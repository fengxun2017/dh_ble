/**
 * @file				DhGlobalHead.h
 * @brief			全局头文件，任何.c文件包含改头文件即可
 * @author			fengxun
 * @date			2017年10月25日
*/
#ifndef __DHGLOBALHEAD_H__
#define __DHGLOBALHEAD_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "DhTypes.h"
#include "DhBleDefine.h"
#include "DhConfig.h"
#include "DhBleAux.h"
#include "./Common/DhQueue.h"
#include "./Common/DhBuffManage.h"
#include "./debug/DhDebug.h"
#include "./DhError.h"

#define CRITICAL_REGION_ENTER()		__disable_irq()
#define CRITICAL_REGION_EXIT()		__enable_irq()

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#include "HardwareHead.h"
#include "./BleDrv/BleLPowerTimer.h"
#include "./BleDrv/BleHAccuracyTimer.h"
#include "./BleDrv/BleRadioDrv.h"
#include "./BleStack/BleAdvertising.h"
#include "./BleStack/BleLink/BleLink.h"
#include "./BleStack/BleLink/BleLinkAdvertising.h"
#include "./BleStack/BleLink/BleLinkConnect.h"
#include "./BleStack/BleLink/BleLinkControl.h"
#include "./BleStack/DhBleEventNtf.h"
#include "./BleStack/BleL2cap.h"
#include "./BleStack/BleAtt.h"
#include "./BleStack/BleGatt.h"
#include "./BleStack/BleGap.h"

#endif /* __DHGLOBALHEAD_H__ */
