#include "../include/DhGlobalHead.h"

u1 TestValue = 1;
u1 count= 0;
void TestHandler2(void *pContext);

void TestHandler(void *pContext)
{
	uint8_t value = *(uint8_t *)pContext;
	DEBUG_INFO("timeout:%d",value);
	nrf_gpio_pin_toggle(22);

	BleLPowerTimerStart(BLE_LP_TIMER0, 1000000, TestHandler, &TestValue);	
	count = 0;
	BleLPowerTimerStart(BLE_LP_TIMER1,200000,TestHandler2, NULL);

}

void TestHandler2(void *pContext)
{
	count++;
	if(count<2)
	{
		BleLPowerTimerStart(BLE_LP_TIMER1,200000,TestHandler2, NULL);
	}
	
}


int main(void)
{
	nrf_gpio_cfg_output(22);
	nrf_gpio_pin_clear(22);
	BleLPowerTimerInit();
	BleLPowerTimerStart(BLE_LP_TIMER0, 1000000, TestHandler, &TestValue);
	BleLPowerTimerStart(BLE_LP_TIMER1,200000,TestHandler2, NULL);
	while(1);
	return 0;
}
