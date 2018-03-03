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

#define BLE_MAIN_DEBUG

#if !defined(BLE_MAIN_DEBUG)
#undef DEBUG_INFO
#define DEBUG_INFO(...)
#undef DEBUG_DATA
#define DEBUG_DATA(...)
#endif


#define ADV_INTERVAL_MS     500



u4 BleAdvDataInit(void)
{
    /* L T V 格式的广播数据 */
//    u1 pu1AdvData[]={0x07,0x09,'D','H','_','B','L','E'};      // 广播名
    u1 pu1ScanRspData[]={0x06,0xFF,0x33,0x33,0x33,0x33,0x33};   // 厂商自定义数据 
	
    BleAdvDataCfg(NULL, 0);
    BleScanRspDataCfg(pu1ScanRspData, sizeof(pu1ScanRspData));

    return DH_SUCCESS;
}

u4 AdvertisingStart(void)
{
	BlkAdvChannelOn channelOn;
	
	channelOn.m_ChannelOn_37 = 1;
	channelOn.m_ChannelOn_38 = 1;
	channelOn.m_ChannelOn_39 = 1;
	BleAdvStart(channelOn, ADV_INTERVAL_MS);
	return DH_SUCCESS;
}

BlkBleAttHandleInfo g_u2RxHandle;
BlkBleAttHandleInfo g_u2TxHandle;

u4 DemoServiceInit(void)
{
    u1  pu1MyServiceUuid[] = {0x00,0xFF};
    u1  pu1CharTxUuid[] = {0x01,0xFF};
    static u1  pu1CharTxBuff[BLE_ATT_MTU_SIZE]; // 特性值的缓存需要应用层提供，所以需要设置成静态变量
    u1  pu1CharRxUuid[] = {0x02,0xFF};
    static u1  pu1CharRxBuff[BLE_ATT_MTU_SIZE];
    BlkGattCharCfg charCfg;

	/* 创建一个演示的服务，有2个特征值，第一个具有读写和indication特性，第二个具有读写和notify特性*/
	BleGattServiceDeclAdd(pu1MyServiceUuid, UUID_TYPE_16BIT);
    charCfg.m_BlkCharProps.m_u1ReadEnable = 1;
    charCfg.m_BlkCharProps.m_u1WriteCmdEnable = 1;
	charCfg.m_BlkCharProps.m_u1IndicateEnable = 1;
    charCfg.m_blkUuid.m_pu1Uuid = pu1CharRxUuid;
    charCfg.m_blkUuid.m_u1UuidType = UUID_TYPE_16BIT;
    charCfg.m_u2ValuePermission = ATT_PERMISSION_READ | ATT_PERMISSION_WRITE;
    BleGattCharacteristicAdd(charCfg, pu1CharRxBuff, sizeof(pu1CharRxBuff), &g_u2RxHandle);
    DEBUG_INFO("Rx Handle:%04X    cccd handle:%04x",g_u2RxHandle.m_u2ValueHandle, g_u2RxHandle.m_u2CccdHandle);

    charCfg.m_BlkCharProps.m_u1ReadEnable = 1;
    charCfg.m_BlkCharProps.m_u1WriteReqEnable = 1;
    charCfg.m_BlkCharProps.m_u1NotifyEnable = 1;
    charCfg.m_blkUuid.m_pu1Uuid = pu1CharTxUuid;
    charCfg.m_blkUuid.m_u1UuidType = UUID_TYPE_16BIT;
    charCfg.m_u2ValuePermission = ATT_PERMISSION_READ | ATT_PERMISSION_WRITE;
    BleGattCharacteristicAdd(charCfg, pu1CharTxBuff, sizeof(pu1CharTxBuff), &g_u2TxHandle);
    DEBUG_INFO("Rx Handle:%04X    cccd handle:%04x",g_u2TxHandle.m_u2ValueHandle,g_u2TxHandle.m_u2CccdHandle);
	
	return DH_SUCCESS;
}


void PrintfConnInfo(BlkBleEvent *event)
{
    
    DEBUG_INFO("connected! params:");
    DEBUG_INFO("  peer addr type:%d   addr:",event->m_event.m_blkConnInfo.m_u1PeerBleAddrType);
    DEBUG_DATA(event->m_event.m_blkConnInfo.m_pu1PeerBleAddr,BLE_ADDR_LEN);
    DEBUG_INFO("\r\n  interval:%d",event->m_event.m_blkConnInfo.m_u2ConnInterval);
    DEBUG_INFO("  timeout:%d",event->m_event.m_blkConnInfo.m_u2ConnTimeout);
    DEBUG_INFO("  latency:%d",event->m_event.m_blkConnInfo.m_u2SlaveLatency);
}

void PrintfDisconnInfo(BlkBleEvent *event)
{
    DEBUG_INFO("Disconnected  reason: %02x", event->m_event.m_blkDisconnInfo.m_u1ErrCode);
}

void PrintfConnUpdateInfo(BlkBleEvent *event)
{
    DEBUG_INFO("connect update!!");
    DEBUG_INFO("  interval:%d",event->m_event.m_blkConnUpdateInfo.m_u2ConnInterval);
    DEBUG_INFO("  timeout:%d",event->m_event.m_blkConnUpdateInfo.m_u2ConnTimeout);
    DEBUG_INFO("  latency:%d",event->m_event.m_blkConnUpdateInfo.m_u2SlaveLatency);
}

void PrintfWriteInfo(BlkBleEvent *event)
{
    DEBUG_INFO(" handle :%04x   receive data:",event->m_event.m_blkWriteInfo.m_u2AttHandle);
    DEBUG_DATA(event->m_event.m_blkWriteInfo.m_pu1AttValue,event->m_event.m_blkWriteInfo.m_u2WriteLen);
    DEBUG_INFO("\r\n");
    
}

void MyBleEventHandler(BlkBleEvent *event)
{
    
    switch(event->m_u2EvtType)
    {
        case BLE_EVENT_CONNECTED:
            PrintfConnInfo(event);
        break;

        case BLE_EVENT_DISCONNECTED:
            PrintfDisconnInfo(event);
            AdvertisingStart(); // 重新广播
        break;

        case BLE_EVENT_CONN_UPDATE:
            PrintfConnUpdateInfo(event);
        break;

        case BLE_EVENT_RECV_WRITE:
            PrintfWriteInfo(event);
        break;

        case BlE_EVENT_RECV_HVC:
            DEBUG_INFO("receive handle value confirm");
		break;
    }
}


u4 BleStackInit(void)
{
    DhBleEventNtfInit();
    DhBleEventHandlerReg(MyBleEventHandler);
	BleLinkInit();
	
	return BleGattInfoInit();
}

void UartInit()
{
	HwUartSimpleInit(UART_TX_PIN, UART_RX_PIN, UART_BAUDRATE_115200);
}
void LowPower(void)
{
    __SEV();
    __WFE();
    __WFE();
}

void DebugInit(void)
{
    #ifdef DEBUG_LOG_USE_UART
    UartInit();
    #endif
}

/*
	已知的未处理问题：
1：未处理一个间隔发多包，可能手机会在发完一个包后，紧接着发另一个包，当前不能处理第二条，导致手机下个间隔会重发，而当前对于重发的都不处理，最终导致丢数据
2：属性数据库中的属性值应该在每次连接后清空才对
*/

int main(void)
{
	BlkBleAddrInfo	addr;
	u1 testSend[20] = {0x11,0x22,0x44};
	addr.m_u1AddrType = 0;	// public
	addr.m_pu1Addr[0] = 0x16;addr.m_pu1Addr[1] = 0x02;addr.m_pu1Addr[2] = 0x03;
	addr.m_pu1Addr[3] = 0x04;addr.m_pu1Addr[4] = 0x05;addr.m_pu1Addr[5] = 0x06;
    DebugInit();
    DEBUG_INFO("start");
    BleStackInit();

    /* 名字和地址要在广播数据设置之前设置好 */
	BleGapDeviceNameCfg("DH_BLE", strlen("DH_BLE"));
	BleGapAddrCfg(addr);
    
    BleAdvDataInit();
    DemoServiceInit();
    AdvertisingStart();
    nrf_gpio_cfg_input(20, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_PULLUP);
	while(1)
	{
        LowPower();
		if(nrf_gpio_pin_read(20) == 0 ) // 连接后每个连接都会唤醒，可以检测按键，不过可能不灵敏，测试先这样吧
		{
            BleGattSendNotify(g_u2TxHandle.m_u2ValueHandle, testSend, 20);
            while(nrf_gpio_pin_read(20)==0);
		}
		if(nrf_gpio_pin_read(19) == 0 ) // 连接后每个连接都会唤醒，可以检测按键，测试先这样吧
		{
            BleGattSendIndication(g_u2RxHandle.m_u2ValueHandle, testSend, 20);
            while(nrf_gpio_pin_read(19)==0);
		}
	}
	return 0;
}
