/**
 * @file			dh_config.h
 * @brief			一些宏配置
 * @author			fengxun
 * @date			2017年11月7日
*/

#define DH_IRQ_PRIORITY_0		(0)		// 最高优先级
#define DH_IRQ_PRIORITY_1		(1)
#define DH_IRQ_PRIORITY_2		(2)
#define DH_IRQ_PRIORITY_3		(3)

#define UART_RX_PIN             (8)
#define UART_TX_PIN             (9)

#define HARDWARE_NRF51          

#define DEBUG_LOG
#define DEBUG_LOG_USE_UART      /* 使用串口打印信息时，只供应用程显示使用。不要再协议栈内部打印信息，串口打印太慢影响时序 */
#define nDEBUG_LOG_USE_RTT      /* 协议栈内部可以使用RTT打印调试信息，但是打印的位置不能影响时序 */
