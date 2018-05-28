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
#ifndef __DH_ERROR_H__
#define __DH_ERROR_H__
#define DH_SUCCESS							(0x00)

#define ERR_LINK_BASE						(0xFF000000)
#define ERR_LINK_INVALID_PARAMS				(ERR_LINK_BASE+0x00000001)
#define ERR_LINK_INVALID_LEN				(ERR_LINK_BASE+0x00000003)
#define ERR_LINK_START_ERROR				(ERR_LINK_BASE+0x00000004)
#define ERR_LINK_NOT_CHANNEL_MAP_REQ        (ERR_LINK_BASE+0x00000005)
#define ERR_LINK_NOT_CONN_UPDATE_REQ        (ERR_LINK_BASE+0x00000006)
#define ERR_LINK_STATE_INVALID              (ERR_LINK_BASE+0x00000007)
#define ERR_LINK_MIC_FAILED                 (ERR_LINK_BASE+0x00000008)

#define ERR_HA_TIMER_BASE					(0xFE000000)
#define ERR_LP_TIMER_BASE					(0xFD000000)

#define ERR_DH_QUEUE_BASE					(0xFC000000)
#define ERR_DH_QUEUE_SIZE_INVALID			(ERR_DH_QUEUE_BASE + 0x00000001)
#define ERR_DH_QUEUE_FULL					(ERR_DH_QUEUE_BASE + 0x00000002)
#define ERR_DH_QEUEUE_EMPTY					(ERR_DH_QUEUE_BASE + 0x00000003)

#define ERR_L2CAP_BASE						(0xFB000000)
#define ERR_L2CAP_INVALID_PARAM				(ERR_L2CAP_BASE+0x00000001)
#define ERR_L2CAP_INVALID_CHANNEL			(ERR_L2CAP_BASE+0x00000002)
#define ERR_L2CAP_INVALID_LENGTH			(ERR_L2CAP_BASE+0x00000003)
#define ERR_L2CAP_INSUFFICIENT_RESOURCE		(ERR_L2CAP_BASE+0x00000004)

#define ERR_GATT_BASE						(0xFA000000)
#define ERR_GATT_INVALID_HANDLE				(ERR_GATT_BASE+0x00000001)
#define ERR_GATT_NOT_FIND_ATT				(ERR_GATT_BASE+0x00000002)
#define ERR_GATT_RESOURCE_INSUFFICCIENT     (ERR_GATT_BASE+0x00000003)
#define ERR_GATT_INVALID_PARAMS             (ERR_GATT_BASE+0x00000004)
#define ERR_GATT_NOT_FIND_CCCD              (ERR_GATT_BASE+0x00000005)
#define ERR_GATT_ATT_STATE_INVALID          (ERR_GATT_BASE+0x00000006)
#define ERR_GATT_VALUE_LEN                  (ERR_GATT_BASE+0x00000007)

#define ERR_ATT_BASE						(0xF9000000)
#define ERR_ATT_SEND_RSP_FAILED				(ERR_ATT_BASE+0x00000001)
#define ERR_ATT_INVALID_PARAMS              (ERR_ATT_BASE+0x00000002)
#define ERR_ATT_NOT_FIND                    (ERR_ATT_BASE+0x00000003)

#define ERR_DH_MEMORY_BASE                  (0xF8000000)
#define ERR_DH_MEMORY_PARAMS_IINVALID       (ERR_DH_MEMORY_BASE+0x00000001)
#define ERR_DH_MEMORY_INSUFFICIENT          (ERR_DH_MEMORY_BASE+0x00000002)

#define ERR_GAP_BASE                        (0xF7000000)
#define ERR_GAP_ADV_INVALID_PARAMS          (ERR_GAP_BASE+0x00000001)
#define ERR_GAP_INTERNAL                    (ERR_GAP_BASE+0x00000002)
#define ERR_GAP_INVALID_PARAMS              (ERR_GAP_BASE+0x00000003)
#define ERR_GAP_BUFF_LEN                    (ERR_GAP_BASE+0x00000004)

#define ERR_SM_BASE                         (0xF6000000)
#define ERR_SM_INVALID_PARAMS               (ERR_SM_BASE+0x00000001)

#define ERR_COMM_BASE                       (0xF5000000)
#define ERR_COMM_INVALID_PARAMS             (ERR_COMM_BASE+0x00000001)
#endif
