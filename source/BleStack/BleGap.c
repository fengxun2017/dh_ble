/*
* (The MIT License)

* Copyright (c) 2018 Feng Xun

* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* 'Software'), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:

* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
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
