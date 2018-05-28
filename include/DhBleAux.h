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

#ifndef __DHBLEAUX_H__
#define __DHBLEAUX_H__

#include "./DhGlobalHead.h"

#define DH_MIN(a,b) ((a)<(b)?(a):(b))
#define DH_MAX(a,b) ((a)>(b)?(a):(b))

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern u2   BleGetChannelFreq(u1 u1Channel);

extern u1   GetChannelWhiteIv(u1 u1Channel );

extern void DhMemxor(u1 *pu1Data1, u1 *pu1Data2, u2 u2Len);

extern u4   DhAesEnc(u1 *pu1In, u1 *pu1Key, u1 *pu1Out);

extern u4   DhAesEnc2(u1 *pu1In, u1 *pu1Key, u1 *pu1Out);

extern void DhGetRand(u1 *pu1Data, u2 len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DHBLEAUX_H__ */
