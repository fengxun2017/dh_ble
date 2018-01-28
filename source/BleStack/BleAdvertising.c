#include "../../include/DhGlobalHead.h"

#define BLE_GAP_ADV_FLAG_LEN    (0x03)      // L(0x02) T(0x01) V(flag)

#define BLE_ADVDATA_FLAG        (0x01)


u4 BleGapAdvDataCfg(u1 *pu1Data, u2 len)
{
    u1  tmp[BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH];
    u2  index = 0;
    BlkBleAddrInfo addr;
    
    if( NULL==pu1Data || len>(BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH-BLE_ADDR_LEN-BLE_GAP_ADV_FLAG_LEN) )
    {
        return ERR_GAP_ADV_INVALID_PARAMS;
    }
    addr = LinkAdvAddrInfoGet();
    tmp[index++] = 0x02;
    tmp[index++] = BLE_ADVDATA_FLAG;
    tmp[index++] = 0x06;    // General Discoverable Mode & BR/EDR Not Supported
    memcpy(tmp+index, addr.m_pu1Addr, BLE_ADDR_LEN)
    index += BLE_ADDR_LEN;
    memcpy(tmp+index, pu1Data, len);
	LinkAdvDataCfg(tmp, BLE_PDU_HEADER_LENGTH, len+BLE_GAP_ADV_FLAG_LEN);

	return DH_SUCCESS;
}

u4 BleGapScanRspDataCfg(u1 *pu1Data, u2 len)
{
    u1  tmp[BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH];
    u2  index = 0;
    BlkBleAddrInfo addr;
    
    if( NULL==pu1Data || len>(BLE_PDU_LENGTH-BLE_PDU_HEADER_LENGTH-BLE_ADDR_LEN) )
    {
        return ERR_GAP_ADV_INVALID_PARAMS;
    }
    addr = LinkAdvAddrInfoGet();
    memcpy(tmp+index, addr.m_pu1Addr, BLE_ADDR_LEN)
    index += BLE_ADDR_LEN;
    memcpy(tmp+index, pu1Data, len);
	LinkScanRspCfg(tmp, BLE_PDU_HEADER_LENGTH, len+BLE_GAP_ADV_FLAG_LEN);

	return DH_SUCCESS;
}

u4 BleGapAdvStart(BlkAdvChannelOn channels, u2 IntervalMs)
{
    u2  len;
    u1  pu1Header[BLE_PDU_HEADER_LENGTH];
    BlkBleAddrInfo addr;
    
    LinkAdvParamsCfg(channels, IntervalMs);
    len = LinkAdvDataLenGet();
    addr = LinkAdvAddrInfoGet();
    pu1Header[0] = addr.m_u1AddrType<<6 || PDU_TYPE_ADV;
    pu1Header[1] = len&0x3F;
    LinkAdvDataCfg(pu1Header, 0, BLE_PDU_HEADER_LENGTH);

    len = LinkScanRspLenGet();
    pu1Header[0] = addr.m_u1AddrType<<6 || PDU_TYPE_SCAN_RSP;
    pu1Header[1] = len&0x3F;
    LinkScanRspCfg(pu1Header, 0, BLE_PDU_HEADER_LENGTH);
    
	return LinkAdvStart();
	
}

u4 BleGapAddrCfg(BlkBleAddrInfo addr)
{
    
    LinkAdvAddrInfoCfg(addr);

    return DH_SUCCESS;
}


