/*
 * DescriptorBaseTypes.h
 *
 *  Created on: 2017Äê2ÔÂ8ÈÕ
 *      Author: zcleicai
 *		reuse Goettingen Code
 */

#ifndef SRC_DESCRIPTORBASETYPES_H_
#define SRC_DESCRIPTORBASETYPES_H_

#include "include.h"


/*******************************************************************************
* Definition
*******************************************************************************/
#define MAX_STR_DESCR_SIZE  		(20+1)
#define MAX_STR_DESCR_SIZE_30  		(30+1)
#define MAX_STR_DESCR_SIZE_40  		(40+1)
#define MAX_STR_DESCR_SIZE_60  		(60+1)
#define MAX_STR_DESCR_SIZE_100  	(100+1)





/*******************************************************************************
* Rumpf eines Deskriptoreintrags
*******************************************************************************/
typedef __packed struct
{
    Uint8              EntryNr;
    Uint8              Flags;
    Uint8              DataType;
} T_DS_ENTRY;

/*******************************************************************************
* Uint8(uchar)-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    Uint8       Value;
} T_DS_U8;

/*******************************************************************************
* Int8(signed char)-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    Int8        Value;
} T_DS_S8;

/*******************************************************************************
* Uint16(ushort)-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    Uint16      Value;
} T_DS_U16;

/*******************************************************************************
* Int16(signed short)-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    Int16       Value;
} T_DS_S16;

/*******************************************************************************
* Uint32(ulong)-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    Uint32      Value;
} T_DS_U32;

/*******************************************************************************
* Int32(signed long)-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    Int32       Value;
} T_DS_S32;

/*******************************************************************************
* FLOAT-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    float       Value;
} T_DS_FLOAT;

/*******************************************************************************
* DOUBLE-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    double      Value;
} T_DS_DOUBLE;

/*******************************************************************************
* STRING-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    Uint8       Value[MAX_STR_DESCR_SIZE];
} T_DS_STR20;

/*******************************************************************************
* STRING-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    Uint8       Value[MAX_STR_DESCR_SIZE_30];
} T_DS_STR30;

/*******************************************************************************
* STRING-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    Uint8       Value[MAX_STR_DESCR_SIZE_40];
} T_DS_STR40;

/*******************************************************************************
* STRING-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    Uint8       Value[MAX_STR_DESCR_SIZE_60];
} T_DS_STR60;

/*******************************************************************************
* STRING-Deskriptor
*******************************************************************************/
typedef __packed struct
{
    T_DS_ENTRY  Descr;
    Uint8       Value[MAX_STR_DESCR_SIZE_100];
} T_DS_STR100;





/*******************************************************************************
* BYTEARRAY-Deskriptor zum Kopieren von Daten
*******************************************************************************/
typedef T_DS_STR T_DS_BYTEARRAY;

/*******************************************************************************
* "Fixed Descriptor Entries" nach CAN29-Standard
*******************************************************************************/
typedef __packed struct
{
    T_DS_STR20		Name;
    T_DS_U16		DescrId;
    T_DS_U16		DeviceType;
    T_DS_U8			DeviceId;
    T_DS_U8			SubDevices;
    T_DS_U16		ParentId;
} T_DS_HEAD;

/*******************************************************************************
*
*******************************************************************************/
typedef __packed struct
{
    T_DS_U16    MagicNumber;
    T_DS_U16    Checksum;

} T_DS_TAIL;

#endif



#endif /* SRC_DESCRIPTORBASETYPES_H_ */
