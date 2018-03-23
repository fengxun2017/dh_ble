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
#ifndef __BLESECURITYMANAGER_H__
#define __BLESECURITYMANAGER_H__

#define BLE_SM_PASSKEY_SIZE         (6)

typedef enum
{
    DISPLAY_ONLY = 0,
    DISPLAY_YES_NO,
    KEYBOARD_ONLY,
    NO_INPUT_NO_OUTPUT,
    KEYBOARD_DISPLAY,
}EnDevIoCapacity;

typedef struct
{
    u1  m_bEncKey:1;
    u1  m_bIdKey:1;
    u1  m_bSignKey:1;
}BlkKeyDistribution;
typedef struct
{
    EnDevIoCapacity     m_enIoCapacity;
    u1                  m_u1OobFlag;
    u1                  m_u1BondFlag;
    u1                  m_u1MimtFlag;
    u1                  m_u1MaxEncKeySize;
    BlkKeyDistribution  m_blkInitiatorKeyMap;
    BlkKeyDistribution  m_blkResponderKeyMap;
}BlkBleSmParsms;

typedef struct
{
    u1  pu1DispKey[BLE_SM_PASSKEY_SIZE];
}BlkBleSmDispKeyEvent;


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLESECURITYMANAGER_H__ */
