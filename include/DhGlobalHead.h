/*
* (The MIT License)

* Copyright (c) 2018 Feng Xun

* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* 'Software'), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:

* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#ifndef __DHGLOBALHEAD_H__
#define __DHGLOBALHEAD_H__


#include <stdio.h>
#include <stdarg.h>
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

#include "./Crypto/SwAes/aes.h"
#include "./Crypto/SwAes/aes_locl.h"
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
#include "./BleStack/BleL2cap.h"
#include "./BleStack/BleAtt.h"
#include "./BleStack/BleGatt.h"
#include "./BleStack/BleGap.h"
#include "./BleStack/BleSecurityManager.h"
#include "./BleStack/DhBleEventNtf.h"

#endif /* __DHGLOBALHEAD_H__ */
