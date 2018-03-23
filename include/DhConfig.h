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

#ifndef __DH_CONFIG_H__
#define __DH_CONFIG_H__

#define DH_IRQ_PRIORITY_0		(0)		// 最高优先级
#define DH_IRQ_PRIORITY_1		(1)
#define DH_IRQ_PRIORITY_2		(2)
#define DH_IRQ_PRIORITY_3		(3)



#define HARDWARE_NRF51          

#define DEBUG_LOG
#define DEBUG_LOG_USE_UART      /* 使用串口打印信息时，只供应用程显示使用。不要再协议栈内部打印信息，串口打印太慢影响时序 */
#define nDEBUG_LOG_USE_RTT      /* 协议栈内部可以使用RTT打印调试信息，但是打印的位置不能影响时序 */


#ifdef HARDWARE_NRF51
    #define UART_RX_PIN             (8)
    #define UART_TX_PIN             (9)
    #define LITTLE_ENDIA
#endif

#endif
