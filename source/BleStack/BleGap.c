/**
 * @file				BleGap.c
 * @brief			ble gap层
 * @author			fengxun
 * @date			2018年1月30日
*/
#include "../../include/DhGlobalHead.h"

u4 BleGapAddrCfg(BlkBleAddrInfo addr)
{
    LinkAdvAddrInfoCfg(addr);

    return DH_SUCCESS;
}

u4 BleGapDeviceNameCfg(char *pu1Name, u1 len)
{
    u2  handle;
    BlkBleAttribute *pblkAtt;

    if( NULL==pu1Name || len>BLE_DEV_NAME_MAX_SIZE )
    {
        return ERR_GAP_INVALID_PARAMS;
    }
    handle = BleDeviceNameAttHandleGet();
    if( BLE_ATT_INVALID_HANDLE == handle )
    {
        return ERR_GAP_INTERNAL;
    }
    BleGattFindAttByHandle(handle, &pblkAtt);
    if( NULL == pblkAtt )
    {
        return ERR_GAP_INTERNAL;
    }
    if( len > pblkAtt->m_blkAttValue.m_u2MaxSize )
    {
        len = pblkAtt->m_blkAttValue.m_u2MaxSize;
    }
    memcpy(pblkAtt->m_blkAttValue.m_pu1AttValue, pu1Name, len);
    pblkAtt->m_blkAttValue.m_u2CurrentLen = len;
	
	return DH_SUCCESS;
}

/**
 *@brief: 		BleGapDeviceNameGet
 *@details:		获取设备名
 *@param[out]	    pu1Name         返回设备名的Buff
 *@param[in|out]	pu1BuffSize     buff大小，返回实际设备名长度
 *@retval:		
 */
u4 BleGapDeviceNameGet(char *pu1Name, u1 *pu1BuffSize)
{
    u2  handle;
    BlkBleAttribute *pblkAtt;

    if( NULL==pu1Name )
    {
        return ERR_GAP_INVALID_PARAMS;
    }
    handle = BleDeviceNameAttHandleGet();
    if( BLE_ATT_INVALID_HANDLE == handle )
    {
        return ERR_GAP_INTERNAL;
    }
    BleGattFindAttByHandle(handle, &pblkAtt);
    if( NULL == pblkAtt )
    {
        return ERR_GAP_INTERNAL;
    }
    if( *pu1BuffSize < pblkAtt->m_blkAttValue.m_u2CurrentLen)
    {
        return ERR_GAP_BUFF_LEN;
    }
    memcpy(pu1Name, pblkAtt->m_blkAttValue.m_pu1AttValue, pblkAtt->m_blkAttValue.m_u2CurrentLen);
    *pu1BuffSize = pblkAtt->m_blkAttValue.m_u2CurrentLen;

    return DH_SUCCESS;
}
