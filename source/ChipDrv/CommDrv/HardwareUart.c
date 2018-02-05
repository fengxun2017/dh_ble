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

#include "../../../include/DhGlobalHead.h"

void HwUartSimpleInit(u1 txPin, u1 rxPin, EnUartBaudrate enBaud)
{
#ifdef HARDWARE_NRF51
    u4 baud;

    baud = NrfUartBaudGet(enBaud);
    NrfUartSimpleInit(rxPin, txPin, baud);
#endif
}

void HwUartSimpleTxData(u1 *pu1Data, u2 len)
{
    u2 index;
#ifdef HARDWARE_NRF51
    for ( index = 0; index < len; index++ )
    {
        NrfUartSimpleTxByte(pu1Data[index]);
    }
#endif
}


/* 带换行的打印*/
void DhPrintfLineFeed(const char *fmt,...)
{
    u2  outLen;
    u1  buff[1024];
    va_list args;
    
    va_start(args,fmt);
    outLen = vsnprintf((char *)buff, sizeof(buff), fmt, args);
	if ( outLen < (sizeof(buff)-2) )
	{
		buff[outLen] = '\r';
		buff[outLen+1] = '\n';
	}
	outLen += 2;
	va_end(args);

    HwUartSimpleTxData(buff, outLen);
	
}

void DhPrintf(const char *fmt,...)
{
    u2  outLen;
    u1  buff[1024];
    va_list args;
    
    va_start(args,fmt);
    outLen = vsnprintf((char *)buff, sizeof(buff), fmt, args);
	va_end(args);

    HwUartSimpleTxData(buff, outLen);
	
}
