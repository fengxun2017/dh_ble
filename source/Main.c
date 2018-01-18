#include "../include/DhGlobalHead.h"

u1 TestValue = 1;
u4 count= 0;
void TestHandler2(void *pContext);
void TestHandler3(void *pContext);

void TestHandler(void *pContext)
{
	uint8_t value = *(uint8_t *)pContext;
	static  u1 LPTimer0Count = 0;
	
	if(LPTimer0Count >=5){ 	DEBUG_INFO("LP0");return;}
	DEBUG_INFO("LP0");

	BleLPowerTimerStart(BLE_LP_TIMER0, 1000000, TestHandler, &TestValue);	
	count = 0;
	BleLPowerTimerStart(BLE_LP_TIMER1,200000,TestHandler2, NULL);
	if(LPTimer0Count == 2)
	{
		BleLPowerTimerStart(BLE_LP_TIMER2,300000,TestHandler3, NULL);
	}
	LPTimer0Count++;
}

void TestCalRtc(void *pContext)
{
	BleLPowerTimerStart(BLE_LP_TIMER0, 1000000, TestCalRtc, &TestValue);	
	count++;
	DEBUG_INFO("count:%d",count);
}
void TestHandler2(void *pContext)
{
	count++;
	if(count<2)
	{
		BleLPowerTimerStart(BLE_LP_TIMER1,200000,TestHandler2, NULL);
	}
	DEBUG_INFO("LP1");
}

void TestHandler3(void *pContext)
{
		DEBUG_INFO("LP2");	
		BleLPowerTimerStop(BLE_LP_TIMER1);
}

void TestHATimerHandler(void *pContext)
{
//	BleHAccuracyTimerStart(BLE_HA_TIMER0, 1000000, TestHATimerHandler, NULL);
//	nrf_gpio_pin_toggle(22);
//	DEBUG_INFO("high accuracy timerout");
	BleLPowerTimerStop(BLE_LP_TIMER0);
}

void delayMs(u4 ms)
{
	volatile u4 i, j;
	for(i = 0; i<ms; i++)
	{
		for(j = 0; j<16; j++);
	}
}
//int main(void)
//{
//	DEBUG_INFO("!!!!!!!!!!!!!!!!!!!!START!!!!!!!!!!!!!!!!!");
//	nrf_gpio_cfg_output(22);
//	nrf_gpio_pin_clear(22);
//	nrf_gpio_cfg_input(20, NRF_GPIO_PIN_PULLUP);
//	
//	BleLPowerTimerInit();
//	BleHAccuracyTimerInit();
//	BleLPowerTimerStart(BLE_LP_TIMER0, 1000000, TestHandler, &TestValue);
//	BleLPowerTimerStart(BLE_LP_TIMER1,200000,TestHandler2, NULL);
//	BleHAccuracyTimerStart(BLE_HA_TIMER0, 4200000, TestHATimerHandler, NULL);
//	while(1)
//	{
//		if(nrf_gpio_pin_read(20)==0)
//		{
//			delayMs(10);
//			BleLPowerTimerStart(BLE_LP_TIMER0, 1000000, TestHandler, &TestValue);
//			while(nrf_gpio_pin_read(20)==0);
//		}
//	}
//	return 0;
//}
//int main(void)
//{
//	DEBUG_INFO("!!!!!!!!!!!!!!!!!!!!START!!!!!!!!!!!!!!!!!");
//	nrf_gpio_cfg_output(22);
//	nrf_gpio_pin_clear(22);
//	nrf_gpio_cfg_input(20, NRF_GPIO_PIN_PULLUP);
//	
//	BleLinkInit();
//	BleLPowerTimerStart(BLE_LP_TIMER0, 2000000, TestCalRtc, &TestValue);
//	while(1)
//	{
//		if(nrf_gpio_pin_read(20)==0)
//		{
//			while(nrf_gpio_pin_read(20)==0);
//		}
//	}
//	return 0;
//}

u1 test_data[]={0,0,1,2,3,4,5,6,0x02,0x01,0x04,0x06,0x09,0x31,0x31,0x32,0x32,0x32};
u1 test_data2[]={4,0,1,2,3,4,5,6,0x06,0xFF,0x33,0x33,0x33,0x33,0x33};

int main(void)
{
	BlkAdvChannelOn channelOn;
	BlkBleAddrInfo	addr;
	
	addr.m_u1AddrType = 0;
	addr.m_pu1Addr[0] = 0x01;addr.m_pu1Addr[1] = 0x02;addr.m_pu1Addr[2] = 0x03;
	addr.m_pu1Addr[3] = 0x04;addr.m_pu1Addr[4] = 0x05;addr.m_pu1Addr[5] = 0x06;
	channelOn.m_ChannelOn_37 = 1;
	channelOn.m_ChannelOn_38 = 1;
	channelOn.m_ChannelOn_39 = 1;
	DEBUG_INFO("start");

	
	test_data[1] = 6+10;
	test_data2[1] = 13;
	BleLinkInit();
	LinkAdvDataCfg(test_data, sizeof(test_data));
	LinkScanRspCfg(test_data2, sizeof(test_data2));
	LinkAdvParamsCfg(channelOn, 20);
	LinkAdvAddrInfoCfg(addr);
	LinkAdvStart();
	
	while(1);
	return 0;
}
