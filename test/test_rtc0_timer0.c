
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

    NrfRtc0Clear();
    NrfRtc0SetCmpReg(NRF_RTC_CMPREG_0, 500000, 1);
	SEGGER_SYSVIEW_RecordExitISR();

}

void timer0_timeout_callback(EnNrfTimer0Evt event) {
    SEGGER_SYSVIEW_RecordEnterISR();
    NrfTimer0Clear();
    NrfTimer0SetCmpReg(NRF_TIMER_CMPREG_0, 400000);
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
    NrfRtc0SetCmpReg(NRF_RTC_CMPREG_0, 1000000, 1);

    // 初始化timer，2秒中断一次
    NrfTimer0Init();
    NrfTimer0SetCmpReg(NRF_TIMER_CMPREG_0, 2000000);
    NrfTimer0HandlerRegister(timer0_timeout_callback);

    NrfRtc0Start();
    NrfTimer0Start();
    for( ; ; ) {
        nrf_gpio_pin_toggle(LED_2);
        nrf_delay_ms(1000);
        //SEGGER_SYSVIEW_PrintfHost("toggle\n");
    }
    
    return 0;    
}


