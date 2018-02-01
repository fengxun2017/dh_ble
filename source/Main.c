#include "../include/DhGlobalHead.h"

#define ADV_INTERVAL_MS     1000



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
	
	channelOn.m_ChannelOn_37 = 0;
	channelOn.m_ChannelOn_38 = 1;
	channelOn.m_ChannelOn_39 = 0;
	BleAdvStart(channelOn, ADV_INTERVAL_MS);
	return DH_SUCCESS;
}

u2 g_u2RxHandle;
u2 g_u2TxHandle;

u4 DemoServiceInit(void)
{
    u1  pu1MyServiceUuid[] = {0x00,0xFF};
    u1  pu1CharTxUuid[] = {0x01,0xFF};
    static u1  pu1CharTxBuff[BLE_ATT_MTU_SIZE]; // 特性值的缓存需要应用层提供，所以需要设置成静态变量
    u1  pu1CharRxUuid[] = {0x02,0xFF};
    static u1  pu1CharRxBuff[BLE_ATT_MTU_SIZE];
    BlkGattCharCfg charCfg;


	BleGattServiceDeclAdd(pu1MyServiceUuid, UUID_TYPE_16BIT);
    charCfg.m_BlkCharProps.m_u1ReadEnable = 1;
    charCfg.m_BlkCharProps.m_u1WriteCmdEnable = 1;
    charCfg.m_blkUuid.m_pu1Uuid = pu1CharRxUuid;
    charCfg.m_blkUuid.m_u1UuidType = UUID_TYPE_16BIT;
    charCfg.m_u2ValuePermission = ATT_PERMISSION_READ | ATT_PERMISSION_WRITE;
    BleGattCharacteristicAdd(charCfg, pu1CharRxBuff, sizeof(pu1CharRxBuff), &g_u2RxHandle);
    DEBUG_INFO("Rx Handle:%02X",g_u2RxHandle);

    charCfg.m_BlkCharProps.m_u1ReadEnable = 1;
    charCfg.m_BlkCharProps.m_u1WriteReqEnable = 1;
    charCfg.m_BlkCharProps.m_u1NotifyEnable = 1;
    charCfg.m_blkUuid.m_pu1Uuid = pu1CharTxUuid;
    charCfg.m_blkUuid.m_u1UuidType = UUID_TYPE_16BIT;
    charCfg.m_u2ValuePermission = ATT_PERMISSION_READ | ATT_PERMISSION_WRITE;
    BleGattCharacteristicAdd(charCfg, pu1CharTxBuff, sizeof(pu1CharTxBuff), &g_u2TxHandle);
    DEBUG_INFO("Rx Handle:%02X",g_u2TxHandle);
	
	return DH_SUCCESS;
}


void PrintfConnInfo(BlkBleEvent *event)
{
    u1 i;
    
    DhPrintf("connected! params:\r\n");
    DhPrintf("  peer addr type:%d   addr:",event->m_event.m_blkConnInfo.m_u1PeerBleAddrType);
    for( i = 0; i < BLE_ADDR_LEN; i++ )
    {
        DhPrintf("%02x ",event->m_event.m_blkConnInfo.m_pu1PeerBleAddr[i])
    }
    DhPrintf("\r\n");
    DhPrintf("  interval:%d\r\n",event->m_event.m_blkConnInfo.m_u2ConnInterval);
    DhPrintf("  timeout:%d\r\n",event->m_event.m_blkConnInfo.m_u2ConnTimeout);
    DhPrintf("  latency:%d\r\n",event->m_event.m_blkConnInfo.m_u2SlaveLatency);
}

void PrintfDisconnInfo(BlkBleEvent *event)
{
    DhPrintf("Disconnected  reason: %02x\r\n", event->m_event.m_blkDisconnInfo.m_u1ErrCode)
}

void PrintfConnUpdateInfo(BlkBleEvent *event)
{
    DhPrintf("connect update!!\r\n");
    DhPrintf("  interval:%d\r\n",event->m_event.m_blkConnUpdateInfo.m_u2ConnInterval);
    DhPrintf("  timeout:%d\r\n",event->m_event.m_blkConnUpdateInfo.m_u2ConnTimeout);
    DhPrintf("  latency:%d\r\n",event->m_event.m_blkConnUpdateInfo.m_u2SlaveLatency);
}

void PrintfWriteInfo(BlkBleEvent *event)
{
    u2 i;

    DhPrintf(" handle :%04x   receive data:",event->m_event.m_blkWriteInfo.m_u2AttHandle);
    for( i = 0; i < event->m_event.m_blkWriteInfo.m_u2WriteLen; i++)
    {
        DhPrintf("%02d ",event->m_event.m_blkWriteInfo.m_pu1AttValue[i]);
    }
    DhPrintf("\r\n");
    
}

void MyBleEventHandler(BlkBleEvent *event)
{
    
    switch(event.m_u2EvtType)
    {
        case BLE_EVENT_CONNECTED:
            PrintfConnInfo(event);
        break;

        case BLE_EVENT_DISCONNECTED:
            PrintfDisconnInfo(event);
        break;

        case BLE_EVENT_CONN_UPDATE:
            PrintfConnUpdateInfo(event);
        break;

        case BLE_EVENT_RECV_WRITE:
            PrintfWriteInfo(event);
        break;

        case BlE_EVENT_RECV_HVC:
            DhPrintf("receive handle value confirm\r\n");
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
	HwUartSimpleInit(UART_TX_PIN, UART_RX_PIN, UART_BAUDRATE_BAUDRATE_Baud115200);
}
void LowPower(void)
{
    __SEV();
    __WFE();
    __WFE();
}
int main(void)
{
	BlkBleAddrInfo	addr;
	
	addr.m_u1AddrType = 0;
	addr.m_pu1Addr[0] = 0x05;addr.m_pu1Addr[1] = 0x02;addr.m_pu1Addr[2] = 0x03;
	addr.m_pu1Addr[3] = 0x04;addr.m_pu1Addr[4] = 0x05;addr.m_pu1Addr[5] = 0x06;

	DEBUG_INFO("start");
    BleStackInit();
    UartInit();

	
    /* 名字和地址要在广播数据设置之前设置好 */
	BleGapDeviceNameCfg("DH_BLE", strlen("DH_BLE"));
	BleGapAddrCfg(addr);
    
    BleAdvDataInit();
    DemoServiceInit();
    AdvertisingStart();
    
	while(1)
	{
        LowPower();
	}
	return 0;
}
