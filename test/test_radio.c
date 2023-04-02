
#include "../include/DhGlobalHead.h"
#define LEDS_NUMBER     4
#define LED_ACTIVE      0
static const uint32_t m_board_led_list[LEDS_NUMBER] = {13, 14, 15, 16};

static void nrf_gpio_cfg_output(uint32_t pin_number) {
    NRF_P0->PIN_CNF[pin_number] =   ((1<<0)         // output
                                    | (1<<1)        
                                    | (3<<2));
}          


static void nrf_gpio_cfg_interrupt(uint32_t pin_number) {
    NRF_P0->PIN_CNF[pin_number] =   ((0<<0)         // input
                                    | (0<<1)        
                                    | (3<<2))
                                    | (3<<16);
}          

static void nrf_gpio_pin_write(uint32_t pin_number, uint32_t value)
{
    if (value == 0)
    {
        NRF_P0->OUTCLR = (1<<pin_number);
    }
    else
    {
        NRF_P0->OUTSET = (1<<pin_number);
    }
}

static void nrf_gpio_pin_toggle(uint32_t pin_number)
{
    NRF_GPIO_Type * reg        = NRF_P0;
    uint32_t        pins_state = reg->OUT;

    reg->OUTSET = (~pins_state & (1UL << pin_number));
    reg->OUTCLR = (pins_state & (1UL << pin_number));
}

static void bsp_board_leds_init(void)
{
    uint32_t i;
    for(i = 0; i < LEDS_NUMBER; ++i)
    {
        nrf_gpio_cfg_output(m_board_led_list[i]);
        nrf_gpio_pin_write(m_board_led_list[i], ( LED_ACTIVE));
    }
}


#define LED_1   (13)
#define LED_2   (14)


void rtc0_timeout_callback(EnNrfRtc0Evt event) {
	SEGGER_SYSVIEW_RecordEnterISR();

    // 广播周期到了，再次发送之前设置的广播
    NrfRadioStartTx();

    // 重启定时器
    NrfRtc0Clear();
    NrfRtc0SetCmpReg(NRF_RTC_CMPREG_0, 100000, 1);
    NrfRtc0Start();

	SEGGER_SYSVIEW_RecordExitISR();
}

int main(void) {
    uint32_t v;
    bsp_board_leds_init();
    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_OnIdle();

    // 启动HFCLK，LFCLK。LFCLK每秒校准一次（校准源为HFCLK）
    NrfHFClkSrcSetXtal();   
    NrfLFClkStart(LFCLK_SRC_RC, 1000, 1);
    
    // 设置rtc0中断回调，初始化rtc0, 设置 1 秒中断一次。
    NrfRtc0HandlerRegister(rtc0_timeout_callback);
    NrfRtc0Init();
    NrfRtc0SetCmpReg(NRF_RTC_CMPREG_0, 100000, 1);
    
    // 初始化 radio，发射功率设置为-4,dBm
    NrfRadioInit();
    NrfRadioTxPowerSet( -4 );

    // 广播通道的CRC初始值为固定的 0x555555
    NrfRadioCrcInitValueCfg(BLE_ADV_CHANNEL_CRC_INIT);
    // 都使用逻辑0地址，关联的物理接入地址为广播使用的0x8E89BED6
    NrfLogicAddrCfg( 0, BLE_ADV_ACCESS_ADDR );
    NrfRadioTxAddrCfg( 0 );
    NrfRadioRxAddrEnable( 0 );
    
    // 配置Tx接收后（disabled），自动切换到Rx
    //NrfRadioAutoToRxEnable()
    
    // 白化初始值为数据所在BLE逻辑通道号（0-39），这里设置37，测试就在37上广播
    // 37广播通道，物理频率为2402
    NrfRadioDataWhiteIvCfg(37);
    NrfRadioFreqCfg(2402);

    u1 adv_data[] = {0x02,0x12,0xFF,0x01,0x02,0x03,0x05,0xff, 0x02,0x01,0x06, 0x08,0x09,0x66,0x65,0x6e,0x67,0x78,0x75,0x6e};
    NrfRadioPacketPtrCfg( adv_data );
    NrfRadioStartTx();

    NrfRtc0Start();

    for( ; ; ) {
        nrf_gpio_pin_toggle(LED_2);
        nrf_delay_ms(1000);
        //SEGGER_SYSVIEW_PrintfHost("toggle\n");
    }
    
    return 0;    
}


