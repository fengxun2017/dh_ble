/**
 * @file				BleHAccuracyTimer.c
 * @brief			协议栈使用的高精度时钟相关实现
 * @author			fengxun
 * @date			2017年11月30日
*/


#include "../../include/DhGlobalHead.h"

/*
* @brief	高精度时钟内部维护信息
*/
typedef struct
{
    BleHAccuracyTimeroutHandler m_cb;		/* 回调函数 */
    void *m_pValue;							/* 传给回调函数的参数 */
} BlkHATimerCb;

typedef struct
{
    BlkHATimerCb m_pblkHATimer[BLE_HA_MAX];
} BlkHATimerManager;

BlkHATimerManager s_blkBleHATimer;

/**
 *@brief: 		Timer0Evt2HATimerId
 *@details:		由timer0的比较事件获取到是哪个高精度timer产生了超时
 *@param[in]	evtId  
 *@retval:		timerId
 */
static EnBleHAccuracyTimer Evt2HATimerId( EnNrfTimer0Evt evtId )
{
    if( NRF_TIMER0_EVT_COMPARE0 == evtId )
    {
        return BLE_HA_TIMER0;
    }

    return BLE_HA_TIMER_INVALID;
}

/**
 *@brief: 		Timerid2CmpReg
 *@details:		由timerid获取对应的比较寄存器
 *@param[in]	timerId  
 *@retval:		CmpReg
 */
static EnNrfTimerCmpReg Timerid2CmpReg(EnBleHAccuracyTimer timerId)
{
	if( BLE_HA_TIMER0 == timerId )
	{
		return NRF_TIMER_CMPREG_0;
	}

	return NRF_TIMER_CMPREG_INVALID;
}

/**
 *@brief: 		TImer0IntCb
 *@details:		注册的Timer0的中断回调函数，中断发生后，timer0中断处理调-
             	用该回调函数，参数为中断事件
 *@param[in]	evt  
 *@retval:		void
 */
static void Timer0IntCb( EnNrfTimer0Evt evt )
{
    BleHAccuracyTimeroutHandler cb = NULL;
    void *pvalue;
    u1 timerId;

    timerId = Evt2HATimerId(evt);
    
    if ( BLE_HA_TIMER_INVALID != timerId )
    {
    	cb = s_blkBleHATimer.m_pblkHATimer[timerId].m_cb;
    	pvalue = s_blkBleHATimer.m_pblkHATimer[timerId].m_pValue;
	}
    if ( NULL != cb )
    {
		cb(pvalue);
    }
   
}


/**
 *@brief: 		BleHAccuracyTimerInit
 *@details:		初始化BLE使用的低功耗时钟
 *@param[in]	void
 *@retval:		void
 */
void BleHAccuracyTimerInit( void )
{
    memset( &s_blkBleHATimer, 0x00, sizeof( s_blkBleHATimer ) );
    NrfTimer0HandlerRegister( Timer0IntCb );
    NrfTimer0Init();
}



/**
 *@brief: 		BleHAccuracyTimerStart
 *@details:		启动ble的高精度定时器
 *@param[in]	timerId 			需要启动的ble低功耗时钟id，目前就一个 BLE_HA_TIMER0      
 *@param[in]	timeoutUs		定时器超时时间          
 *@param[in]	cb				定时器超时后的回调函数
 *@param[in]	pvalue			回调函数触发时会获取到该值，该值为传递的是地址，所以需要保证运行期间传递的变量的有效性。  
 
 *@retval:		void
 */
void BleHAccuracyTimerStart( EnBleHAccuracyTimer timerId, u4 timeoutUs, BleHAccuracyTimeroutHandler cb, void *pvalue)
{	
	EnNrfTimerCmpReg reg;
	
	s_blkBleHATimer.m_pblkHATimer[timerId].m_cb = cb;
	s_blkBleHATimer.m_pblkHATimer[timerId].m_pValue = pvalue;
	
	NrfTimer0Stop();
	NrfTimer0Clear();
	reg = Timerid2CmpReg(timerId);
	NrfTimer0SetCmpReg(reg, timeoutUs);
	NrfTimer0Start();
}


void BleHAccuracyTimerStop(EnBleHAccuracyTimer timerId)
{
	/* 目前高频定时器协议栈只需要用到一个就可以了，所以直接关timer0定时器就行了 */
	NrfTimer0Stop();
}
