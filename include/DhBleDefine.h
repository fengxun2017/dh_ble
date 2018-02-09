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

#ifndef __BLECONFIG_H__
#define __BLECONFIG_H__


#define DH_BLE_VERSION      0x0001      //2018-2-9 0.01版本

#define BLE_VERSION			0x40

#if (BLE_VERSION == 0x40)
#define	BLE_PDU_LENGTH				(39)		/* 协议数据单元长度为39字节，2字节header+37字节payload*/
#define BLE_PDU_HEADER_LENGTH		(2)
#define BLE_PREAMBLE_LENGTH			(1)
#define BLE_ACC_ADDR_LENGTH			(4)
#define BLE_CRC_LENGTH				(3)
#define BLE_PACKET_LENGTH	(BLE_PREAMBLE_LENGTH+BLE_ACC_ADDR_LENGTH+BLE_PDU_LENGTH+BLE_CRC_LENGTH)
#define BLE_VERSION_NUMBER			(0x06)		/* 6-4.0 7-4.1 8-4.2 9-5.0*/

#define BLE_ATT_MTU_DEFALUT_SIZE    (23)
#endif

/* 通道使用表为5字节 */
#define BLE_CHANNEL_MAP_LEN         (5)

/* 目前只支持默认包，不支持协议自动分包 */
#define BLE_ATT_MTU_SIZE			BLE_ATT_MTU_DEFALUT_SIZE

/* 当前资源下允许的最大ATT MTU */
#define BLE_ATT_MTU_MAX_SIZE        BLE_ATT_MTU_DEFALUT_SIZE


/* 按一个广播包来看 37，去掉6字节地址，去掉3字节广播flag, 去掉 L T 2字节还剩可以放名字26字节 */
#define BLE_DEV_NAME_MAX_SIZE       (26)     

/* 广播的接入地址是固定的*/
#define BLE_ADV_ACCESS_ADDR			(0x8E89BED6)

/* BLE CRC多项式为x24 + x10 + x9 + x6 + x4 + x3 + x + 1 */
#define BLE_CRC_POLY				(0x100065B)

/* BLE广播通道下CRC初始值 */
#define BLE_ADV_CHANNEL_CRC_INIT	(0x555555)

/* BLE帧间间隔为150us */
#define BLE_TIFS_VALUE				(150)

/* BLE地址长度 */
#define BLE_ADDR_LEN				(6)

#define BLE_ADV_CHANNEL_NUMBER		(3)
#define BLE_ADV_CHANNEL_37			(37)
#define BLE_ADV_CHANNEL_38			(38)
#define BLE_ADV_CHANNEL_39			(39)

#define BLE_DATA_CHANNEL_NUMBER		(37)
#define BLE_DATA_CHANNEL_0			(0)
#define BLE_DATA_CHANNEL_1			(1)
#define BLE_DATA_CHANNEL_2			(2)
#define BLE_DATA_CHANNEL_3			(3)
#define BLE_DATA_CHANNEL_4			(4)
#define BLE_DATA_CHANNEL_5			(5)
#define BLE_DATA_CHANNEL_6			(6)
#define BLE_DATA_CHANNEL_7			(7)
#define BLE_DATA_CHANNEL_8			(8)
#define BLE_DATA_CHANNEL_9			(9)
#define BLE_DATA_CHANNEL_10			(10)
#define BLE_DATA_CHANNEL_11			(11)
#define BLE_DATA_CHANNEL_12			(12)
#define BLE_DATA_CHANNEL_13			(13)
#define BLE_DATA_CHANNEL_14			(14)
#define BLE_DATA_CHANNEL_15			(15)
#define BLE_DATA_CHANNEL_16			(16)
#define BLE_DATA_CHANNEL_17			(17)
#define BLE_DATA_CHANNEL_18			(18)
#define BLE_DATA_CHANNEL_19			(19)
#define BLE_DATA_CHANNEL_20			(20)
#define BLE_DATA_CHANNEL_21			(21)
#define BLE_DATA_CHANNEL_22			(22)
#define BLE_DATA_CHANNEL_23			(23)
#define BLE_DATA_CHANNEL_24			(24)
#define BLE_DATA_CHANNEL_25			(25)
#define BLE_DATA_CHANNEL_26			(26)
#define BLE_DATA_CHANNEL_27			(27)
#define BLE_DATA_CHANNEL_28			(28)
#define BLE_DATA_CHANNEL_29			(29)
#define BLE_DATA_CHANNEL_30			(30)
#define BLE_DATA_CHANNEL_31			(31)
#define BLE_DATA_CHANNEL_32			(32)
#define BLE_DATA_CHANNEL_33			(33)
#define BLE_DATA_CHANNEL_34			(34)
#define BLE_DATA_CHANNEL_35			(35)
#define BLE_DATA_CHANNEL_36			(36)
#define BLE_INVALID_CHANNEL			(0xFF)


#endif /* __BLECONFIG_H__ */
