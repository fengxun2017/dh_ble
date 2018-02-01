/**
 * @file				BleGatt.c
 * @brief			实现GATT相关内容
 * @author			fengxun
 * @date			2018年1月3日
*/
#include "../../include/DhGlobalHead.h"

#define SERVER_INITIATE_NOTIFY              0x01
#define SERVER_INITIATE_INDICATION          0x02
#define SERVER_NOTIFY_ENABLED               0x0001
#define SERVER_INDCATION_ENABLE             0x0002
#define	BLE_GATT_ATT_MAX_COUNT      20


/*UUID内存资源池，从这里获取存放uuid的内存 */

CREATE_MEMORY_INSTANCE( UUID_RESOURCE_POOL, ( BLE_GATT_ATT_MAX_COUNT * 2 * UUID_TYPE_128BIT ) );

typedef struct
{
    u2	m_u1AttMaxCount;                      /* 最大允许存在的属性 */
    u2	m_u2AttCount;                         /* 当前属性个数 */
    BlkBleAttribute	*m_pblkBleServiceSets;    /* 当前属性数据库*/
    u2  m_u2DevNameHandle;                    /* 设备名句柄 */
} BlkBleGattInfo;

static BlkBleGattInfo s_blkBleGattInfo;

/* uuid数组 LSB */
//u1	pu1PrimaryServiceUuid[] = {BLE_PRIMARY_SERVICE_UUID & 0xff, BLE_PRIMARY_SERVICE_UUID >> 8};
//u1	pu1CharacteristicDeclaration[] = {BLE_CHARACTERISTIC_DECLARATION_UUID & 0xff, BLE_CHARACTERISTIC_DECLARATION_UUID >> 8};
//u1	pu1GenericAccessServiceUuid[] = {BLE_GENERIC_ACCESS_SERVICE_UUID & 0xff, BLE_GENERIC_ACCESS_SERVICE_UUID >> 8};
//u1	pu1GenericAttributeServiceUuid[] = {BLE_GENERIC_ATTRIBUTE_SERVICE_UUID & 0xff, BLE_GENERIC_ATTRIBUTE_SERVICE_UUID >> 8};
//u1	pu1DeviceNameUuid[] = {BLE_DEVICE_NAME_UUID & 0xff, BLE_DEVICE_NAME_UUID >> 8};
//u1	pu1CCCDUuid[] = {BLE_CCCD_UUID & 0xff, BLE_CCCD_UUID >> 8};

//u1	pu1DhServiceDemo[] = {00, 0xff};		/* demo service 0xff00 */
//u1	pu1DhServiceTx[] = {0x01, 0xff};		/* tx characteristic 0xff01  write*/
//u1	pu1DhTxDecl[] = {ATT_PROPERTIES_READ | ATT_PROPERTIES_NOTIFY, 0x06, 0x00, 0x01, 0xff};
//u1	pu1DhTxCCCD[2] = {0};
//u1	pu1DhServiceRx[] = {0x02, 0xff};		/* rx characteristic 0xff02  notify*/
//u1	pu1DhRxDecl[] = {ATT_PROPERTIES_READ | ATT_PROPERTIES_WRITE, 0x09, 0x00, 0x02, 0xff};
//u1	pu1TxValue[20];
//u1	pu1RxValue[20];
//u1	pu1DeviceName[] = {'D', 'H', '_', 'B', 'L', 'E'};

/* ble服务集合 */
static BlkBleAttribute		BleServiceSets[BLE_GATT_ATT_MAX_COUNT] =
{0x00
//    {
//        {UUID_TYPE_16BIT, pu1PrimaryServiceUuid},
//        {UUID_TYPE_16BIT, UUID_TYPE_16BIT, pu1GenericAccessServiceUuid,},
//        ATT_PERMISSION_READ,
//    },	/* 1 generic access service 0x1800*/
//    {
//        {UUID_TYPE_16BIT, pu1DeviceNameUuid},
//        {sizeof( pu1DeviceName ), sizeof( pu1DeviceName ), pu1DeviceName},
//        ATT_PERMISSION_READ | ATT_PERMISSION_WRITE,
//    },	/* 2 device name */

//    {
//        {UUID_TYPE_16BIT, pu1PrimaryServiceUuid},
//        {UUID_TYPE_16BIT, UUID_TYPE_16BIT, pu1GenericAttributeServiceUuid},
//        ATT_PERMISSION_READ,
//    },	/* 3 generic attribute service 0x1801 */

//    {
//        {UUID_TYPE_16BIT, pu1PrimaryServiceUuid},
//        {UUID_TYPE_16BIT, UUID_TYPE_16BIT, pu1DhServiceDemo},
//        ATT_PERMISSION_READ,
//    },	/* 4 dh demo service */
//    {
//        {UUID_TYPE_16BIT, pu1CharacteristicDeclaration},
//        {sizeof( pu1DhTxDecl ), sizeof( pu1DhTxDecl ), pu1DhTxDecl},
//        ATT_PERMISSION_READ,
//    },	/* 5 tx characteristic declaration */
//    {
//        {UUID_TYPE_16BIT, pu1DhServiceTx},
//        {sizeof( pu1TxValue ), 0, pu1TxValue},
//        ATT_PERMISSION_READ,
//    },	/* 6 tx value*/
//    {
//        {UUID_TYPE_16BIT, pu1CCCDUuid},
//        {2, 2, pu1CCCDUuid},
//        ATT_PERMISSION_READ | ATT_PERMISSION_WRITE,
//    },	/* 7 tx CCCD*/

//    {
//        {UUID_TYPE_16BIT, pu1CharacteristicDeclaration},
//        {sizeof( pu1DhRxDecl ), sizeof( pu1DhRxDecl ), pu1DhRxDecl},
//        ATT_PERMISSION_READ,
//    },	/* 8 rx characteristic declaration */
//    {
//        {UUID_TYPE_16BIT, pu1DhServiceRx},
//        {sizeof( pu1RxValue ), 0, pu1RxValue},
//        ATT_PERMISSION_READ | ATT_PERMISSION_WRITE,
//    },	/* 9 Rx value*/
};


static u1 CharPropsGet( BlkCharProperties CharProps )
{
    u1  u1Props = 0;

    if( CharProps.m_u1BroadcastEnable )
    {
        u1Props |= ATT_PROPERTIES_BROADCAST;
    }
    else if( CharProps.m_u1ReadEnable )
    {
        u1Props |= ATT_PROPERTIES_READ;
    }
    else if( CharProps.m_u1WriteCmdEnable )
    {
        u1Props |= ATT_PROPERTIES_WRITE_WITHOUT_RSP;
    }
    else if( CharProps.m_u1WriteReqEnable )
    {
        u1Props |= ATT_PROPERTIES_WRITE;
    }
    else if( CharProps.m_u1NotifyEnable )
    {
        u1Props |= ATT_PROPERTIES_NOTIFY;
    }
    else if( CharProps.m_u1IndicateEnable )
    {
        u1Props |= ATT_PROPERTIES_INDICATE;
    }
    else if( CharProps.m_u1SignedWriteEnable )
    {
        u1Props |= ATT_PROPERTIES_AUTH_SIGN_WRITE;
    }
    else if( CharProps.m_u1ExtendedProps )
    {
        u1Props |= ATT_PROPERTIES_EXTEND_PEOPERTIES;
    }

    return u1Props;
}

/**
 *@brief: 		BleGattInfoInit
 *@details:		GATT层相关信息初始化
 *@retval:		DH_SUCCESS
 */
u4	BleGattInfoInit( void )
{
	u1	pu1Uuid[UUID_TYPE_16BIT];
	BlkGattCharCfg charCfg;
	static u1 pu1DeviceNameBuff[BLE_DEV_NAME_MAX_SIZE];    // 设备名的缓存buff，应用层提供要用静态声明，名字最大20字节
	
    memset( &s_blkBleGattInfo, 0x00, sizeof( BlkBleGattInfo ) );
    s_blkBleGattInfo.m_u2AttCount = 0;
    s_blkBleGattInfo.m_u1AttMaxCount = BLE_GATT_ATT_MAX_COUNT;
    s_blkBleGattInfo.m_pblkBleServiceSets = BleServiceSets;
    s_blkBleGattInfo.m_u2DevNameHandle = BLE_ATT_INVALID_HANDLE;

    /* 添加gap服务 */
    pu1Uuid[0] = BLE_GENERIC_ACCESS_SERVICE_UUID&0xff;
    pu1Uuid[1] = (BLE_GENERIC_ACCESS_SERVICE_UUID>>8)&0xff;
	BleGattServiceDeclAdd(pu1Uuid, UUID_TYPE_16BIT);
    /* 添加特性 device name */
    pu1Uuid[0] = BLE_DEVICE_NAME_UUID&0xFF;
    pu1Uuid[1] = (BLE_DEVICE_NAME_UUID>>8)&0xFF;
    charCfg.m_BlkCharProps.m_u1ReadEnable = 1;
    charCfg.m_BlkCharProps.m_u1WriteReqEnable =1 ;
    charCfg.m_BlkCharProps.m_u1WriteCmdEnable = 1;
    charCfg.m_blkUuid.m_pu1Uuid = pu1Uuid;
    charCfg.m_blkUuid.m_u1UuidType = UUID_TYPE_16BIT;
    charCfg.m_u2ValuePermission = ATT_PERMISSION_READ|ATT_PERMISSION_WRITE;
	BleGattCharacteristicAdd(charCfg, pu1DeviceNameBuff, BLE_DEV_NAME_MAX_SIZE, &(s_blkBleGattInfo.m_u2DevNameHandle));
	
    /* 添加GATT服务 */
	pu1Uuid[0] = BLE_GENERIC_ATTRIBUTE_SERVICE_UUID&0xff;
	pu1Uuid[1] = (BLE_GENERIC_ATTRIBUTE_SERVICE_UUID>>8)&0xff;
	BleGattServiceDeclAdd(pu1Uuid, UUID_TYPE_16BIT);
	
    return DH_SUCCESS;
}


u2 BleDeviceNameAttHandleGet(void)
{
    return s_blkBleGattInfo.m_u2DevNameHandle;
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
    if( u2Handle > s_blkBleGattInfo.m_u2AttCount )
    {
        return ERR_GATT_NOT_FIND_ATT;
    }
    /* GATT层查找是从数组中查找，是从下标0开始的 */
    u2Handle -= 1;
    *ppblkAtt = &s_blkBleGattInfo.m_pblkBleServiceSets[u2Handle];

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
    for( u2Index = 0; u2Index < s_blkBleGattInfo.m_u2AttCount; u2Index++ )
    {
        pu1AttUUID = s_blkBleGattInfo.m_pblkBleServiceSets[u2Index].m_blkAttType.m_pu1Uuid;
        AttUUIDType = s_blkBleGattInfo.m_pblkBleServiceSets[u2Index].m_blkAttType.m_u1UuidType;
        if ( ( UUIDType == AttUUIDType ) && ( memcmp( pu1AttUUID, pu1UUID, UUIDType ) == 0 ) )
        {
            *ppblkAtt = &s_blkBleGattInfo.m_pblkBleServiceSets[u2Index];
            return DH_SUCCESS;
        }
    }
    return ERR_GATT_NOT_FIND_ATT;
}


u4 BleGattServiceDeclAdd( u1 *pu1ServiceUuid, u1 uuidType )
{
    u2 currCount;
    BlkBleAttribute  *pblkCurrAtt;
    u1 *pu1Uuid;
    currCount = s_blkBleGattInfo.m_u2AttCount;
    if( currCount >= s_blkBleGattInfo.m_u1AttMaxCount )
    {
        return ERR_GATT_RESOURCE_INSUFFICCIENT;
    }
    if( UUID_TYPE_128BIT != uuidType && UUID_TYPE_16BIT != uuidType )
    {
        return ERR_GATT_INVALID_PARAMS;
    }
    pblkCurrAtt = &s_blkBleGattInfo.m_pblkBleServiceSets[currCount];
    pblkCurrAtt->m_blkAttType.m_u1UuidType = UUID_TYPE_16BIT;
    pu1Uuid = DhMemoryAlloc( UUID_RESOURCE_POOL, UUID_TYPE_16BIT );
    if( NULL == pu1Uuid )
    {
        return ERR_GATT_RESOURCE_INSUFFICCIENT;
    }
    pu1Uuid[0] = BLE_PRIMARY_SERVICE_UUID & 0xff;
    pu1Uuid[1] = ( BLE_PRIMARY_SERVICE_UUID >> 8 ) & 0xff;
    pblkCurrAtt->m_blkAttType.m_pu1Uuid = pu1Uuid;

    pu1Uuid = DhMemoryAlloc( UUID_RESOURCE_POOL, uuidType );
    if( NULL == pu1Uuid )
    {
        return ERR_GATT_RESOURCE_INSUFFICCIENT;
    }
    memcpy( pu1Uuid, pu1ServiceUuid, uuidType );
    pblkCurrAtt->m_blkAttValue.m_u2MaxSize = uuidType;
    pblkCurrAtt->m_blkAttValue.m_u2CurrentLen = uuidType;
    pblkCurrAtt->m_blkAttValue.m_pu1AttValue = pu1Uuid;
    pblkCurrAtt->m_u2AttPermission = ATT_PERMISSION_READ;

    s_blkBleGattInfo.m_u2AttCount++;

    return DH_SUCCESS;
}

u4 BleGattCharacteristicAdd( BlkGattCharCfg charaCfg, u1 *pu1CharValueBuff, u2 u2BuffSize, u2 *pu2ValueHandle )
{

    u2 currCount;
    BlkBleAttribute *pblkCurrAtt;
    BlkBleAttribute *pblkAttValue;
    u1 *pu1Uuid;
    u1  *pu1AttValue;
    u1  index = 0;

    currCount = s_blkBleGattInfo.m_u2AttCount;
    if( currCount >= s_blkBleGattInfo.m_u1AttMaxCount )
    {
        return ERR_GATT_RESOURCE_INSUFFICCIENT;
    }
    if ( NULL == pu1CharValueBuff || NULL == charaCfg.m_blkUuid.m_pu1Uuid )
    {
        return ERR_GATT_INVALID_PARAMS;
    }
    if( UUID_TYPE_16BIT != charaCfg.m_blkUuid.m_u1UuidType && UUID_TYPE_128BIT != charaCfg.m_blkUuid.m_u1UuidType )
    {
        return ERR_GATT_INVALID_PARAMS;
    }
    /* 首先申请一个特性声明属性条目 */
    pblkCurrAtt = &s_blkBleGattInfo.m_pblkBleServiceSets[currCount];
    pblkCurrAtt->m_blkAttType.m_u1UuidType = UUID_TYPE_16BIT;
    pu1Uuid = DhMemoryAlloc( UUID_RESOURCE_POOL, UUID_TYPE_16BIT );
    if ( NULL == pu1Uuid )
    {
        return ERR_GATT_RESOURCE_INSUFFICCIENT;
    }
    pu1Uuid[index++] = BLE_CHARACTERISTIC_DECLARATION_UUID & 0xff;
    pu1Uuid[index++] = ( BLE_CHARACTERISTIC_DECLARATION_UUID >> 8 ) & 0xff;
    pblkCurrAtt->m_blkAttType.m_pu1Uuid = pu1Uuid;
    pblkCurrAtt->m_u2AttPermission = ATT_PERMISSION_READ;
    /* 特性声明的值为 CharProps CharValueHandle CharUuid */
    pu1AttValue = DhMemoryAlloc( UUID_RESOURCE_POOL, 1 + 2 + charaCfg.m_blkUuid.m_u1UuidType );
    if( NULL == pu1AttValue )
    {
        return ERR_GATT_RESOURCE_INSUFFICCIENT;
    }
    index = 0;
    pu1AttValue[index++] = CharPropsGet( charaCfg.m_BlkCharProps );
    /* 特性值紧随特性声明之后,+1是因为value再声明之后，再加1因为handle为数组下标+1 */
    pu1AttValue[index++] = ( currCount + 1 +1 ) & 0xff;         
    pu1AttValue[index++] = ( ( currCount + 1 +1 ) >> 8 ) & 0xff;
    memcpy( pu1AttValue + index, charaCfg.m_blkUuid.m_pu1Uuid, charaCfg.m_blkUuid.m_u1UuidType );
    /* 配置特性声明的值 */
    pblkCurrAtt->m_blkAttValue.m_pu1AttValue = pu1AttValue;
    pblkCurrAtt->m_blkAttValue.m_u2MaxSize = 1 + 2 + charaCfg.m_blkUuid.m_u1UuidType;
    pblkCurrAtt->m_blkAttValue.m_u2CurrentLen = 1 + 2 + charaCfg.m_blkUuid.m_u1UuidType;

    /* 再申请特性值属性条目 */
    currCount++;
    pu1Uuid = DhMemoryAlloc( UUID_RESOURCE_POOL, charaCfg.m_blkUuid.m_u1UuidType );
    if( NULL == pu1Uuid )
    {
        return ERR_GATT_RESOURCE_INSUFFICCIENT;
    }
    if( currCount >= s_blkBleGattInfo.m_u1AttMaxCount )
    {
        return ERR_GATT_RESOURCE_INSUFFICCIENT;
    }
    pblkCurrAtt = &s_blkBleGattInfo.m_pblkBleServiceSets[currCount];

    memcpy( pu1Uuid, charaCfg.m_blkUuid.m_pu1Uuid, charaCfg.m_blkUuid.m_u1UuidType );
    pblkCurrAtt->m_blkAttType.m_u1UuidType = charaCfg.m_blkUuid.m_u1UuidType;
    pblkCurrAtt->m_blkAttType.m_pu1Uuid = pu1Uuid;

    pblkCurrAtt->m_u2AttPermission = charaCfg.m_u2ValuePermission;
    pblkCurrAtt->m_blkAttValue.m_pu1AttValue = pu1CharValueBuff;
    pblkCurrAtt->m_blkAttValue.m_u2MaxSize = u2BuffSize;
    pblkCurrAtt->m_blkAttValue.m_u2CurrentLen = 0;
    pblkAttValue = pblkCurrAtt;
    
    if( NULL != pu2ValueHandle )
    {
        *pu2ValueHandle = (currCount+1);	// handle的值为数组下标+1，因为0x0000是无效的handle
    }
    index = 0;
    if ( charaCfg.m_BlkCharProps.m_u1NotifyEnable || charaCfg.m_BlkCharProps.m_u1IndicateEnable  )
    {
        /* 再添加Client Characteristic Configuration 描述符 */
        currCount++;
        pu1Uuid = DhMemoryAlloc( UUID_RESOURCE_POOL, UUID_TYPE_16BIT );
        if( NULL == pu1Uuid )
        {
            return ERR_GATT_RESOURCE_INSUFFICCIENT;
        }
        if( currCount >= s_blkBleGattInfo.m_u1AttMaxCount )
        {
            return ERR_GATT_RESOURCE_INSUFFICCIENT;
        }
        pblkCurrAtt = &s_blkBleGattInfo.m_pblkBleServiceSets[currCount];
        pu1Uuid[index++] = BLE_CCCD_UUID & 0xff;
        pu1Uuid[index++] = ( BLE_CCCD_UUID >> 8 ) & 0xff;
        pblkCurrAtt->m_blkAttType.m_u1UuidType = UUID_TYPE_16BIT;
        pblkCurrAtt->m_blkAttType.m_pu1Uuid = pu1Uuid;
        pu1AttValue = DhMemoryAlloc( UUID_RESOURCE_POOL, 2 ); // CCCD的值只有2字节，也从这里申请
        pblkCurrAtt->m_blkAttValue.m_u2CurrentLen = 0;
        pblkCurrAtt->m_blkAttValue.m_u2MaxSize = 2;
        pblkCurrAtt->m_blkAttValue.m_pu1AttValue = pu1AttValue;
        pblkCurrAtt->m_u2AttPermission = charaCfg.m_u2CCCDPermission;
        pblkAttValue->m_u2CCCDHandle = currCount+1;     // handle值为数组下标+1
        
    }
    currCount++;
    s_blkBleGattInfo.m_u2AttCount = currCount;

    return DH_SUCCESS;
}

static u1 IsNotifyEnable(u1 initiateType, u1 *pu1AttValue, u1 len)
{
    u2 state;
    if( 2 != len )
    {
        return 0;
    }

    state = pu1AttValue[0]+pu1AttValue[1]*256;
    if( SERVER_INITIATE_INDICATION==initiateType && SERVER_INDCATION_ENABLE==state )
    {
        return 1;
    }
    if( SERVER_INITIATE_NOTIFY==initiateType && SERVER_NOTIFY_ENABLED==state )
    {
        return 1;
    }
    return 0;
}

u4 BleGattSendNotify(u2 u2AttHandle, u1 *pu1AttValue, u2 len)
{
    /* 检查notify 是否使能了 */
    BlkBleAttribute *pblkAtt;
    BlkBleAttribute *pblkCccd;
    u2  cccdHandle;
    BleGattFindAttByHandle(u2AttHandle, &pblkAtt);
    
    if ( NULL == pblkAtt )
    {
        return ERR_GATT_INVALID_HANDLE;
    }
    cccdHandle = pblkAtt->m_u2CCCDHandle;
    BleGattFindAttByHandle(cccdHandle, &pblkCccd);
    if ( NULL == pblkCccd )
    {
        return ERR_GATT_NOT_FIND_CCCD;
    }
    if( !IsNotifyEnable(SERVER_INITIATE_NOTIFY, pblkCccd->m_blkAttValue.m_pu1AttValue, pblkCccd->m_blkAttValue.m_u2CurrentLen) )
    {
        return ERR_GATT_ATT_STATE_INVALID;  // 没有使能notify
    }
    if(len > (BLE_ATT_MTU_SIZE-3) )
    {
        return ERR_GATT_VALUE_LEN;
    }

    return BleAttSendNotify(u2AttHandle, pu1AttValue, len);
}

u4 BleGattSendIndication(u2 u2AttHandle, u1 *pu1AttValue, u2 len)
{
    /* 检查notify 是否使能了 */
    BlkBleAttribute *pblkAtt;
    BlkBleAttribute *pblkCccd;
    u2  cccdHandle;
    BleGattFindAttByHandle(u2AttHandle, &pblkAtt);
    
    if ( NULL == pblkAtt )
    {
        return ERR_GATT_INVALID_HANDLE;
    }
    cccdHandle = pblkAtt->m_u2CCCDHandle;
    BleGattFindAttByHandle(cccdHandle, &pblkCccd);
    if ( NULL == pblkCccd )
    {
        return ERR_GATT_NOT_FIND_CCCD;
    }
    if( !IsNotifyEnable(SERVER_INITIATE_INDICATION, pblkCccd->m_blkAttValue.m_pu1AttValue, pblkCccd->m_blkAttValue.m_u2CurrentLen) )
    {
        return ERR_GATT_ATT_STATE_INVALID;  // 没有使能notify
    }
    if(len > (BLE_ATT_MTU_SIZE-3) )
    {
        return ERR_GATT_VALUE_LEN;
    }
    return BleAttSendIndication(u2AttHandle, pu1AttValue, len);
}




