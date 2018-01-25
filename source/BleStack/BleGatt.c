/**
 * @file				BleGatt.c
 * @brief			实现GATT相关内容
 * @author			fengxun
 * @date			2018年1月3日
*/
#include "../../include/DhGlobalHead.h"

#define	BLE_GATT_ATT_MAX_COUNT      20

/*UUID内存资源池，从这里获取存放uuid的内存 */

CREATE_MEMORY_INSTANCE(UUID_RESOURCE_POOL, (BLE_GATT_ATT_MAX_COUNT*2*UUID_TYPE_128BIT));

typedef struct
{
    u2	u1AttMaxCount;                      /* 最大允许存在的属性 */
    u2	u2AttCount;                         /* 当前属性个数 */
    BlkBleAttribute	*blkBleServiceSets;     /* 当前属性数据库*/
} BlkBleGattInfo;

static BlkBleGattInfo s_blkBleGattInfo;

/* uuid数组 LSB */
u1	pu1PrimaryServiceUuid[] = {BLE_PRIMARY_SERVICE_UUID & 0xff, BLE_PRIMARY_SERVICE_UUID >> 8};
u1	pu1CharacteristicDeclaration[] = {BLE_CHARACTERISTIC_DECLARATION_UUID & 0xff, BLE_CHARACTERISTIC_DECLARATION_UUID >> 8};
u1	pu1GenericAccessServiceUuid[] = {BLE_GENERIC_ACCESS_SERVICE_UUID & 0xff, BLE_GENERIC_ACCESS_SERVICE_UUID >> 8};
u1	pu1GenericAttributeServiceUuid[] = {BLE_GENERIC_ATTRIBUTE_SERVICE_UUID & 0xff, BLE_GENERIC_ATTRIBUTE_SERVICE_UUID >> 8};
u1	pu1DeviceNameUuid[] = {BLE_DEVICE_NAME_UUID & 0xff, BLE_DEVICE_NAME_UUID >> 8};
u1	pu1CCCDUuid[] = {BLE_CCID_UUID & 0xff, BLE_CCID_UUID >> 8};

u1	pu1DhServiceDemo[] = {00, 0xff};		/* demo service 0xff00 */
u1	pu1DhServiceTx[] = {0x01, 0xff};		/* tx characteristic 0xff01  write*/
u1	pu1DhTxDecl[] = {ATT_PROPERTIES_READ | ATT_PROPERTIES_NOTIFY, 0x06, 0x00, 0x01, 0xff};
u1	pu1DhTxCCCD[2] = {0};
u1	pu1DhServiceRx[] = {0x02, 0xff};		/* rx characteristic 0xff02  notify*/
u1	pu1DhRxDecl[] = {ATT_PROPERTIES_READ | ATT_PROPERTIES_WRITE, 0x09, 0x00, 0x02, 0xff};
u1	pu1TxValue[20];
u1	pu1RxValue[20];
u1	pu1DeviceName[] = {'D', 'H', '_', 'B', 'L', 'E'};

/* ble服务集合 */
BlkBleAttribute		BleServiceSets[BLE_GATT_ATT_MAX_COUNT] =
{
    {
        {UUID_TYPE_16BIT, pu1PrimaryServiceUuid},
        {UUID_TYPE_16BIT, UUID_TYPE_16BIT, pu1GenericAccessServiceUuid,},
        ATT_PERMISSION_READ,
    },	/* 1 generic access service 0x1800*/
    {
        {UUID_TYPE_16BIT, pu1DeviceNameUuid},
        {sizeof( pu1DeviceName ), sizeof( pu1DeviceName ), pu1DeviceName},
        ATT_PERMISSION_READ | ATT_PERMISSION_WRITE,
    },	/* 2 device name */

    {
        {UUID_TYPE_16BIT, pu1PrimaryServiceUuid},
        {UUID_TYPE_16BIT, UUID_TYPE_16BIT, pu1GenericAttributeServiceUuid},
        ATT_PERMISSION_READ,
    },	/* 3 generic attribute service 0x1801 */

    {
        {UUID_TYPE_16BIT, pu1PrimaryServiceUuid},
        {UUID_TYPE_16BIT, UUID_TYPE_16BIT, pu1DhServiceDemo},
        ATT_PERMISSION_READ,
    },	/* 4 dh demo service */
    {
        {UUID_TYPE_16BIT, pu1CharacteristicDeclaration},
        {sizeof( pu1DhTxDecl ), sizeof( pu1DhTxDecl ), pu1DhTxDecl},
        ATT_PERMISSION_READ,
    },	/* 5 tx characteristic declaration */
    {
        {UUID_TYPE_16BIT, pu1DhServiceTx},
        {sizeof( pu1TxValue ), 0, pu1TxValue},
        ATT_PERMISSION_READ,
    },	/* 6 tx value*/
    {
        {UUID_TYPE_16BIT, pu1CCCDUuid},
        {2, 2, pu1CCCDUuid},
        ATT_PERMISSION_READ | ATT_PERMISSION_WRITE,
    },	/* 7 tx CCCD*/

    {
        {UUID_TYPE_16BIT, pu1CharacteristicDeclaration},
        {sizeof( pu1DhRxDecl ), sizeof( pu1DhRxDecl ), pu1DhRxDecl},
        ATT_PERMISSION_READ,
    },	/* 8 rx characteristic declaration */
    {
        {UUID_TYPE_16BIT, pu1DhServiceRx},
        {sizeof( pu1RxValue ), 0, pu1RxValue},
        ATT_PERMISSION_READ | ATT_PERMISSION_WRITE,
    },	/* 9 Rx value*/
};

/**
 *@brief: 		BleGattInfoInit
 *@details:		GATT层相关信息初始化
 *@retval:		DH_SUCCESS
 */
u4	BleGattInfoInit( void )
{
    memset( &s_blkBleGattInfo, 0x00, sizeof( BlkBleGattInfo ) );
    s_blkBleGattInfo.u2AttCount = 0;
    s_blkBleGattInfo.u1AttMaxCount = BLE_GATT_ATT_MAX_COUNT;
    s_blkBleGattInfo.blkBleServiceSets = BleServiceSets;

    return DH_SUCCESS;
}

/**
 *@brief: 		BleGattFindAttByHandle
 *@details:		通过handle查找属性数据库中的attribute
 *@param[in]	u2Handle	attribute的handle,att协议handle是从1开始的，gatt这里的数据库是从0开始的
 *@param[out]	ppblkAtt	输出查找到的attribute,没找到返回NULL

 *@retval:		DH_SUCCESS					查找到了，通过ppblkAtt返回
 				ERR_GATT_INVALID_HANDLE		句柄不对
 */

u4	BleGattFindAttByHandle( u2 u2Handle, BlkBleAttribute **ppblkAtt )
{
    *ppblkAtt = NULL;
    /* 	att handle要求从1开始*/
    if( 0 == u2Handle )
    {
        return ERR_GATT_INVALID_HANDLE;
    }
    /* 句柄允许到0xFFFF */
    if( u2Handle > s_blkBleGattInfo.u2AttCount )
    {
        return ERR_GATT_NOT_FIND_ATT;
    }
    /* GATT层查找是从数组中查找，是从下标0开始的 */
    u2Handle -= 1;
    *ppblkAtt = &s_blkBleGattInfo.blkBleServiceSets[u2Handle];

    return DH_SUCCESS;
}

    /**
     *@brief: 		BleGattFindAttByType
     *@details:		通过UUID查找属性
     *@param[in]	u2StartHandle	开始句柄
     *@param[in]	u2EndHandle		结束句柄
     *@param[in]	pu1UUID			查找的UUID
     *@param[in]	UUIDType		uuid类型
     *@param[out]	ppblkAtt  		返回查找到的属性

     *@retval:		DH_SUCCESS					查找到了，通过ppblkAtt返回
     *@retval:		ERR_GATT_INVALID_HANDLE		句柄不对
     *@retval:		ERR_GATT_NOT_FIND_ATT		没有在范围累找到符合要求的att
    */
    u4 BleGattFindAttByType( u2 u2StartHandle, u2 u2EndHandle, u1 *pu1UUID, u1 UUIDType, BlkBleAttribute **ppblkAtt )
{
    u2	u2Index;
    u1	*pu1AttUUID;
    u1	AttUUIDType;

    *ppblkAtt = NULL;
    if( ( 0 == u2StartHandle ) || ( u2StartHandle > u2EndHandle ) )
    {
        return ERR_GATT_INVALID_HANDLE;
    }

    /* 规范要求16位uuid要补足128位后比较，这里就不做补足了直接比较*/
    for( u2Index = 0; u2Index < s_blkBleGattInfo.u2AttCount; u2Index++ )
    {
        pu1AttUUID = s_blkBleGattInfo.blkBleServiceSets[u2Index].attType.pu1Uuid;
        AttUUIDType = s_blkBleGattInfo.blkBleServiceSets[u2Index].attType.uuidType;
        if ( ( UUIDType == AttUUIDType ) && ( memcmp( pu1AttUUID, pu1UUID, UUIDType ) == 0 ) )
        {
            *ppblkAtt = &s_blkBleGattInfo.blkBleServiceSets[u2Index];
            return DH_SUCCESS;
        }
    }
    return ERR_GATT_NOT_FIND_ATT;
}


u4 BleGattAddServiceDecl(u1 *pu1ServiceUuid, u1 uuidType)
{
    u2 currCount;
    BlkBleAttribute  *currAtt;
    u1 *pu1Uuid;
    currCount = s_blkBleGattInfo.u2AttCount;
    if( currCount >= s_blkBleGattInfo.u1AttMaxCount )
    {
        return ERR_GATT_RESOURCE_INSUFFICCIENT;
    }
    if( UUID_TYPE_128BIT!=uuidType && UUID_TYPE_16BIT!=uuidType )
    {
        return ERR_GATT_PARAMS_INVALID;
    }
    currAtt = s_blkBleGattInfo.blkBleServiceSets[currCount];
    currAtt->attType.uuidType = UUID_TYPE_16BIT;
    pu1Uuid = DhMemoryAlloc(UUID_RESOURCE_POOL, UUID_TYPE_16BIT);
    if( NULL == pu1Uuid )
    {
        ERR_GATT_RESOURCE_INSUFFICCIENT;
    }
    pu1Uuid[0] = BLE_PRIMARY_SERVICE_UUID&0xff;
    pu1Uuid[1] = (BLE_PRIMARY_SERVICE_UUID>>8)&0xff;
    currAtt->attType.pu1Uuid = pu1Uuid;

    pu1Uuid = DhMemoryAlloc(UUID_RESOURCE_POOL, uuidType);
    if( NULL == pu1Uuid )
    {
        ERR_GATT_RESOURCE_INSUFFICCIENT;
    }
    memcpy(pu1Uuid, pu1ServiceUuid, uuidType);
    currAtt->attValue.u2MaxSize = uuidType;
    currAtt->attValue.u2CurrentLen = uuidType;
    currAtt->attValue.pu1AttValue = pu1Uuid;

    currAtt->attPermission = ATT_PERMISSION_READ;

    return DH_SUCCESS;
}

u4 BleGattAddCharacteristic(BlkCharacteristicCfg charaCfg, u2 *u2ValueHandle )
{
    
}

