#include "../../include/DhGlobalHead.h"

#define BLE_GAP_ADV_FLAG_LEN    (0x03)      // L(0x02) T(0x01) V(flag)

#define BLE_ADVDATA_FLAG        (0x01)



/**
 *@brief: 		BleAdvDataCfg
 *@details:		设置广播数据，广播名字不需要设置，通过BleGapDeviceNameCfg函数配置
 *@param[in]	pu1Data     L T V 格式的数据
 				len         数据长度

 *@retval:		DH_SUCCESS
 */
u4 BleAdvDataCfg(u1 *pu1Data, u2 len)
{
    u1  tmp[BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH];
    char  pu1DeviceName[BLE_DEV_NAME_MAX_SIZE];
    u1  nameLen = BLE_DEV_NAME_MAX_SIZE;
    u2  index = 0;
    BlkBleAddrInfo addr;

    BleGapDeviceNameGet(pu1DeviceName, &nameLen);
    if( len>(BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH-BLE_ADDR_LEN-BLE_GAP_ADV_FLAG_LEN-nameLen-2) )    // 2为 L T占用的字节
    {
        return ERR_GAP_ADV_INVALID_PARAMS;
    }
    addr = LinkAdvAddrInfoGet();
    memcpy(tmp+index, addr.m_pu1Addr, BLE_ADDR_LEN);
    index += BLE_ADDR_LEN;
	
	tmp[index++] = 0x02;
    tmp[index++] = BLE_ADVDATA_FLAG;
    tmp[index++] = 0x06;    // General Discoverable Mode & BR/EDR Not Supported

    tmp[index++] = nameLen+1;
    tmp[index++] = 0x09;
    memcpy(tmp+index, pu1DeviceName, nameLen);
    index += nameLen;

	if( NULL != pu1Data )
	{
        memcpy(tmp+index, pu1Data, len);
        index += len;
    }
	LinkAdvDataCfg(tmp, BLE_PDU_HEADER_LENGTH, index);

	return DH_SUCCESS;
}

u4 BleScanRspDataCfg(u1 *pu1Data, u2 len)
{
    u1  tmp[BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH];
    u2  index = 0;
    BlkBleAddrInfo addr;
    
    if( len>(BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH-BLE_ADDR_LEN) )
    {
        return ERR_GAP_ADV_INVALID_PARAMS;
    }
    addr = LinkAdvAddrInfoGet();
    memcpy(tmp+index, addr.m_pu1Addr, BLE_ADDR_LEN);
    index += BLE_ADDR_LEN;
	if( NULL != pu1Data )
	{
		memcpy(tmp+index, pu1Data, len);
		index += len;
	}
	LinkScanRspCfg(tmp, BLE_PDU_HEADER_LENGTH, index);

	return DH_SUCCESS;
}

u4 BleAdvStart(BlkAdvChannelOn channels, u2 IntervalMs)
{
    u2  len;
    u1  pu1Header[BLE_PDU_HEADER_LENGTH];
    BlkBleAddrInfo addr;
    
    LinkAdvParamsCfg(channels, IntervalMs);
    addr = LinkAdvAddrInfoGet();
    len = LinkAdvDataLenGet();
    pu1Header[0] = (addr.m_u1AddrType<<6) | PDU_TYPE_ADV;
    pu1Header[1] = len&0x3F;
    LinkAdvDataCfg(pu1Header, 0, BLE_PDU_HEADER_LENGTH);

    len = LinkScanRspLenGet();
    pu1Header[0] = addr.m_u1AddrType<<6 | PDU_TYPE_SCAN_RSP;
    pu1Header[1] = len&0x3F;
    LinkScanRspCfg(pu1Header, 0, BLE_PDU_HEADER_LENGTH);
    
	return LinkAdvStart();
	
}




