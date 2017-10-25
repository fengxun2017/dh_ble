/**
 * @file				ble_aux.c
 * @brief			ble辅助功能函数集合
 * @author			fengxun
 * @date			2017年10月17日
*/
#include "../include/DhGlobalHead.h"

#define RF_CHANNEL0_FREQ			(2402)
#define RF_CHANNEL1_FREQ			(2404)
#define RF_CHANNEL12_FREQ			(2426)
#define RF_CHANNEL13_FREQ			(2428)
#define RF_CHANNEL39_FREQ			(2480)
/**
 *@brief: 		GetRfChannelFreq
 *@details:		获取数据通道对应的频率
 *@param[in]	u1 u1DataChannel  0-36

 *@retval:		具体频率	
 */
u2 	GetDataChannelFreq(u1 u1DataChannel)
{
	if ( u1DataChannel>=BLE_DATA_CHANNEL_0 && u1DataChannel <=BLE_DATA_CHANNEL_10 )
	{
		return RF_CHANNEL1_FREQ+u1DataChannel*2
	}
	else if ( u1DataChannel>=BLE_DATA_CHANNEL_11 && u1DataChannel<=BLE_DATA_CHANNEL_36 )
	{
		return RF_CHANNEL13_FREQ+(u1DataChannel-BLE_DATA_CHANNEL_11)*2;
	}
}

/**
 *@brief: 		GetAdvChannelFreq
 *@details:		获取广播通道对应频率
 *@param[in]	u1 u1AdvChannel  37 38 39
 
 *@retval:		通道号对应的具体频率
 */
u2 GetAdvChannelFreq(u1 u1AdvChannel)
{
	if ( BLE_ADV_CHANNEL_37 == u1AdvChannel )
	{
		return RF_CHANNEL0_FREQ;
	}
	else if ( BLE_ADV_CHANNEL_38 == u1AdvChannel )
	{
		return RF_CHANNEL12_FREQ;
	}
	else if ( BLE_ADV_CHANNEL_39 == u1AdvChannel )
	{
		return RF_CHANNEL39_FREQ;
	}
}
