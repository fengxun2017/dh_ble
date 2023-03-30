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

#ifndef __DH_DEBUG_H__
#define __DH_DEBUG_H__

#ifdef DEBUG_LOG
//#include "SEGGER_RTT_Conf.h"
//#include "SEGGER_RTT.h"
//#include "rtt_log.h"
//#include "DhDebug.h"
/*
	不要在协议栈的内部时序部分代码中添加日志打印，调试信息的处理比较耗时可能需要一百多us，这已经可以影响
	BLE协议栈底层的时序精度要求了。
*/

#if defined(DEBUG_LOG_USE_RTT)
	#define DEBUG_INFO			logi
	#define DEBUG_WARNING		logw
	#define DEBUG_ERROR			loge
#elif defined(DEBUG_LOG_USE_UART)
	#define DEBUG_INFO			DhPrintWithfLinefeed
	#define DEBUG_WARNING		DhPrintWithfLinefeed
	#define DEBUG_ERROR			DhPrintWithfLinefeed
#endif


#define DEBUG_DATA			DebugData
#define DEBUG_ASCII			DebugAscii


void DebugData(u1 *pu1Data, u2 len);
void DebugAscii(u1 *pu1Data, u2 len);
#else
#define DEBUG_INFO(...)		
#define DEBUG_WARNING(...)	
#define DEBUG_ERROR(...)		
#define DEBUG_DATA(...)
#define DEBUG_ASCII(...)			

#endif


#endif	/* __DEBUG_H__ */

