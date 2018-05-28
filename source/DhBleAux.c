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

#include "../include/DhGlobalHead.h"

#define RF_CHANNEL0_FREQ			(2402)
#define RF_CHANNEL1_FREQ			(2404)
#define RF_CHANNEL12_FREQ			(2426)
#define RF_CHANNEL13_FREQ			(2428)
#define RF_CHANNEL39_FREQ			(2480)
/**
 *@brief: 		GetRfChannelFreq
 *@details:		获取数据通道对应的频率
 *@param[in]	u1 u1DataChannel  0-36

 *@retval:		具体频率	
 */
u2 	BleGetChannelFreq(u1 u1Channel)
{
	if ( u1Channel <=BLE_DATA_CHANNEL_10 )
	{
		return RF_CHANNEL1_FREQ+u1Channel*2;
	}
	else if ( u1Channel>=BLE_DATA_CHANNEL_11 && u1Channel<=BLE_DATA_CHANNEL_36 )
	{
		return RF_CHANNEL13_FREQ+(u1Channel-BLE_DATA_CHANNEL_11)*2;
	}
	else if ( BLE_ADV_CHANNEL_37 == u1Channel )
	{
		return RF_CHANNEL0_FREQ;
	}
	else if ( BLE_ADV_CHANNEL_38 == u1Channel )
	{
		return RF_CHANNEL12_FREQ;
	}
	else if ( BLE_ADV_CHANNEL_39 == u1Channel )
	{
		return RF_CHANNEL39_FREQ;
	}
	return 0xFFFF;
}

/**
 *@brief: 		GetChannelWhiteIv
 *@details:		获得该通道的白化序列初值
 *@param[in]	channel   通道号

 *@retval:		whiteIv		白化初值
 */
u1	GetChannelWhiteIv(u1 channel )
{
	return channel;
}

/**
 *@brief: 		DhMemxor
 *@details:		数组异或运算
 *@param[in|out]	pu1Data1    原数据1，运算后输出
 *@param[in]	    pu1Data2    原数据2
 *@param[in]	    u2Len       数据长度
 *@retval:		    void
 */
void  DhMemxor(u1 *pu1Data1, u1 *pu1Data2, u2 u2Len)
{
    u2  u2Index;

    if( NULL==pu1Data1 || NULL==pu1Data2 )
    {
        return ;
    }

    for( u2Index = 0; u2Index < u2Len; u2Index++ )
    {
        pu1Data1[u2Index] = pu1Data1[u2Index]^pu1Data2[u2Index];
    }
    
}

/**
 *@brief: 		DhAesEnc
 *@details:		加密16字节数据
 *@param[in]	pu1In       数据Buff,LSB输入，内部加密前转换成MSB格式
 *@param[in]    pu1Key      秘钥，LSB输入，内部加密前转换成MSB。
 *@param[out]   pu1Out      输出密文，LSB输出
 
 *@retval:		DH_SUCCESS
 */
u4 DhAesEnc(u1 *pu1In, u1 *pu1Key, u1 *pu1Out)
{
    /* AES 算法内部有 U4指针转换过程，M0处理器要求地址访问对齐，所以这里确保一下地址是4字节对齐的 */
    u1  tmpIn[16]  __attribute__((aligned(4)));   
    u1  tmpKey[16] __attribute__((aligned(4)));
    u1  tmpOut[16] __attribute__((aligned(4)));
    u1  i;
    if ( NULL==pu1In || NULL==pu1Key || NULL==pu1Out )
    {
        return ERR_COMM_INVALID_PARAMS;
    }

    for(i = 0; i < 16; i++ )
    {
        tmpIn[i] = pu1In[16-i-1];
    }
    for(i = 0; i < 16; i++ )
    {
        tmpKey[i] = pu1Key[16-i-1];
    }
    SwAesEncryptData(tmpKey, tmpIn, 16, tmpOut);

    for(i = 0; i < 16; i++ )
    {
        pu1Out[i] = tmpOut[16-i-1];
    }

    return DH_SUCCESS;
}


/**
 *@brief: 		DhAesEnc2
 *@details:		加密16字节数据
 *@param[in]	pu1In       数据Buff,MSB,内部不需要转换
 *@param[in]    pu1Key      秘钥，LSB输入，内部加密前转换成MSB。
 *@param[out]   pu1Out      输出密文，MSB 输出
 
 *@retval:		DH_SUCCESS
 */
u4 DhAesEnc2(u1 *pu1In, u1 *pu1Key, u1 *pu1Out)
{
    /* AES 算法内部有 U4指针转换过程，M0处理器要求地址访问对齐，所以这里确保一下地址是4字节对齐的 */
    u1  tmpKey[16] __attribute__((aligned(4)));
    u1  i;
    
    if ( NULL==pu1In || NULL==pu1Key || NULL==pu1Out )
    {
        return ERR_COMM_INVALID_PARAMS;
    }
    for(i = 0; i < 16; i++ )
    {
        tmpKey[i] = pu1Key[16-i-1];
    }
    SwAesEncryptData(tmpKey, pu1In, 16, pu1Out);

    return DH_SUCCESS;
}


/**
 *@brief: 		DhGetRand
 *@details:		获取随机数
 *@param[out]	pu1Data  
 *@param[int]	len      要获取的长度 
 
 *@retval:      void
 */
void DhGetRand(u1 *pu1Data, u2 len)
{
    u2 i;

    if( NULL== pu1Data )
    {
        return ;
    }
    for( i = 0; i < len; i++ )
    {
        pu1Data[i] = i;
    }
}
