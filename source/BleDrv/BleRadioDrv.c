/**
 * @file			BleRadioDrv.c
 * @brief			ble radio驱动文件，基于底层硬件radio上实现的抽象层
 * @author			fengxun
 * @date			2017年11月20日
*/
#include "../../include/DhGlobalHead.h"

BleRadioEvtHandler s_BleRadioHandler = NULL;

#define DEFAULT_TXPOWER		(-4)

static void HwRadioIntHandler( EnNrfRadioEvt evt )
{
    if( NULL != s_BleRadioHandler )
    {
        if ( NRF_RADIO_EVT_DISABLED == evt )
        {
            s_BleRadioHandler( BLE_RADIO_EVT_TRANS_DONE );
        }
    }
}



void BleRadioEvtHandlerReg( BleRadioEvtHandler intHandler )
{
    s_BleRadioHandler = intHandler;
}

/**
 *@brief: 		BleRadioInit
 *@details:		Ble的Radio初始化
 				启动16M外部晶振，初始化radio，设置默认发射功率，注册radio中断处理函数

 *@retval:		void
 */
void BleRadioInit( void )
{
    NrfHFClkSrcSetXtal();	// ble需要外部16M晶振
    NrfRadioInit();
    NrfRadioTxPowerSet( DEFAULT_TXPOWER );
    NrfRadioHandlerReg(HwRadioIntHandler);
}

/**
 *@brief: 		BleRadioRssiMeasuStart
 *@details:		启动RSSI的自动测量

 *@retval:		void
 */
void BleRadioRssiMeasuStart( void )
{
    NrfRssiAutoMeasuEnable();
}

/**
 *@brief: 		BleRadioRssiMeasuStop
 *@details:		关闭RSSI的自动测量

 *@retval:		void
 */
void BleRadioRssiMeasuStop( void )
{
    NrfRssiAutoMeasuDisable();
}


/**
 *@brief: 		BleRadioRssiGet
 *@details:		获取RSSI的值

 *@retval:		RSSI值
 */
s1 BleRadioRssiGet( void )
{
    return NrfRadioRssiResultGet();
}

/**
 *@brief: 		BleRadioWhiteIvCfg
 *@details:		配置ble白化序列的初始值

 *@retval:		void
 */
void BleRadioWhiteIvCfg( u1 iv )
{
    NrfRadioDataWhiteIvCfg( iv );
}

/**
 *@brief: 		BleRadioCrcInit
 *@details:		配置crc的初值
 *@param[in]	u4CrcInit	广播crc初值为0x555555，连接后crc初值由连接请求中的参数指定

 *@retval:		void
 */
void BleRadioCrcInit( u4 u4CrcInit )
{
    NrfRadioCrcInitValueCfg( u4CrcInit );
}


/**
 *@brief: 		BleRadioTxAddrCfg
 *@details:		配置Ble的接入地址，并以该地址作为后续的发送和接收的接入地址
 *@param[in]	u4AccAddr	BLE的接入地址，广播接入地址为0x8E89BED6，连接下接入地址由连接请求中的参数指定

 *@retval:		void
 */
void BleRadioTxRxAddrCfg( u1 logicAddr, u4 u4AccAddr )
{

    NrfLogicAddrCfg( logicAddr, u4AccAddr );
    NrfRadioTxAddrCfg( logicAddr );
    NrfRadioRxAddrEnable( logicAddr );
}



/**
 *@brief: 		BleRadioTxData
 *@details:		ble发送数据
 *@param[in]	channel		数据的发送通道	0-39
 *@param[in]	pu1Data		待发送的数据buff，函数内部不会拷贝缓存数据，需要保证发送期间buff数据的有效性
 *@param[in]	len			待发送数据的长度

 *@note		该函数中的长度参数在nrf51实现上没有实际作用。51会识别协议中的长度字段

 *@retval:		void
 */
void BleRadioTxData( u1 u1channel, u1 *pu1Data, u2 len )
{
    u2 freq;

    freq = BleGetChannelFreq( u1channel );

    NrfRadioFreqCfg( freq );
    NrfRadioPacketPtrCfg( pu1Data );
    NrfRadioStartTx();
}

/**
 *@brief: 		BleRadioSimpleTx
 *@details:		ble发送数据，直接在当前通道下发送不做通道切换操作
 *@param[in]	pu1Data  
 *@param[in]	len  
 
 *@retval:		void
 */
void BleRadioSimpleTx(u1 *pu1Data, u2 len)
{
    NrfRadioPacketPtrCfg( pu1Data );
//    NrfRadioStartTx();
}

/**
 *@brief: 		BleRadioRxData
 *@details:		ble接收数据
 *@param[in]	u1Channel	接收数据的通道0-39
 *@param[in]	pu1Data		接收数据的buff，上层自己保证不会溢出

 *@retval:		void
 */
void BleRadioRxData( u1 u1Channel, u1 *pu1Data )
{
    u2 freq;

    freq = BleGetChannelFreq( u1Channel );
    NrfRadioFreqCfg( freq );
    NrfRadioPacketPtrCfg( pu1Data );
    NrfRadioStartRx();
}

/**
 *@brief: 		BleRadioSimpleRx
 *@details:		ble接收数据，直接在当前通道下接收不做通道切换操作
 *@param[in]	pu1Data  
 
 *@retval:		void
 */
void BleRadioSimpleRx(u1 *pu1Data)
{
    NrfRadioPacketPtrCfg( pu1Data );
    NrfRadioStartRx();
}

/**
 *@brief: 		IsBleRadioCrcOk
 *@details:		判断ble数据包crc校验是否正确

 *@retval:		1-ok  0-err
 */
u1 IsBleRadioCrcOk( void )
{
    return NrfRadioCrcStatusGet();
}



/**
 *@brief: 		BleRadioDisable
 *@details:		强制关闭radio

 *@retval:		void
 */
void BleRadioDisable( void )
{
    NrfRadioDisable();
}

void BleAutoToRxEnable(void)
{
    NrfRadioAutoToRxEnable();
}

void BleAutoToRxDisable(void)
{
    NrfRadioAutoToRxDisable();
}

