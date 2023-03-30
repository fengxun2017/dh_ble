#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_


#define HARDWARE_NRF52840

// 硬件中断优先级
#define DH_IRQ_PRIORITY_0		(0)		// 最高优先级
#define DH_IRQ_PRIORITY_1		(1)
#define DH_IRQ_PRIORITY_2		(2)
#define DH_IRQ_PRIORITY_3		(3)

#define DEBUG_LOG
#define nDEBUG_LOG_USE_UART      /* 使用串口打印信息时，只供应用程显示使用。不要再协议栈内部打印信息，串口打印太慢影响时序 */
#define nDEBUG_LOG_USE_RTT      /* 协议栈内部可以使用RTT打印调试信息，但是打印的位置不能影响时序 */


#ifdef DEBUG_LOG
#include "../debug_use_systemview/SEGGER/SEGGER_SYSVIEW.h"
#endif

#ifdef HARDWARE_NRF51
    #define UART_RX_PIN             (8)
    #define UART_TX_PIN             (9)
    #define LITTLE_ENDIA
#endif

#ifdef HARDWARE_NRF52840
#define CONFIG_GPIO_AS_PINRESET     // nordic52840 P0.18引脚作为 reset 功能 
#endif 


#endif // PROJECT_CONF_H_
