
#include "../../../include/DhGlobalHead.h"



void SwAesEncryptData(u1 *pu1Key, u1* pu1InData, u4 u4InLen, u1 *pu1OutData)
{
	u4	u4Index = 0;
	AES_KEY	aesKey;
	
	if( (NULL==pu1InData) || (NULL==pu1OutData) )
	{
		return ;
	}
	
	AES_set_encrypt_key(pu1Key, 128, &aesKey);
	while ( u4InLen > 0 )
	{
		AES_encrypt(pu1InData+u4Index, pu1OutData+u4Index, &aesKey);
		u4Index += 16;
		u4InLen -= 16;
	}
	
}

void SwAesDecryptData(u1 *pu1Key, u1* pu1InData, u4 u4InLen, u1 *pu1OutData)
{
	u4	u4Index = 0;
	AES_KEY	aesKey;
	
	if( (NULL==pu1InData) || (NULL==pu1OutData) )
	{
		return ;
	}
	
	AES_set_decrypt_key(pu1Key, 128, &aesKey);
	while ( u4InLen > 0 )
	{
		AES_decrypt(pu1InData+u4Index, pu1OutData+u4Index, &aesKey);
		u4Index += 16;
		u4InLen -= 16;
	}
	
}

unsigned int DhIrol(unsigned int u4Value, unsigned char b)
{
	unsigned int temp;
	unsigned int highBit;
	unsigned char i;
	temp = u4Value;
	for(i = 0; i < b; i++)
	{
		highBit = temp&0x80000000;
		temp = temp<<1;
		if(highBit)
			temp |= 1;
	}
	
    return temp;
}


unsigned int DhIror(unsigned int u4Value, unsigned char b)
{
	unsigned int temp;
	unsigned char bit;
	unsigned char i;
	
	temp = u4Value;
	for(i = 0; i < b; i++)
	{
		bit = temp&0x01;
		temp = temp>>1;
		if(bit)
			temp |= 0x80000000;
	}
	
    return temp;
}
