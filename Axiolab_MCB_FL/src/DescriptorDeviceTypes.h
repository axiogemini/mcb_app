/*
 * DescriptorDeviceTypes.h
 *
 *  Created on: 2017Äê2ÔÂ8ÈÕ
 *      Author: zcleicai
 */

#ifndef SRC_DESCRIPTORDEVICETYPES_H_
#define SRC_DESCRIPTORDEVICETYPES_H_

#include "DescriptorBaseTypes.h"

/*******************************************************************************
* Objective Descriptor
*******************************************************************************/
typedef __packed struct {
	T_DS_STR60	Name;	
	T_DS_STR20	OrderId;
	T_DS_FLOAT	Magnification;
	T_DS_FLOAT	Aperture;
	T_DS_STR30	Immersion;
	T_DS_STR100	Contrast;
	T_DS_U16	WorkingDistance;
	T_DS_STR30	ID;
	T_DS_STR30	Features;
	T_DS_STR20	Category;
	T_DS_FLOAT	CoverSlipThickMax;
	T_DS_FLOAT	CoverSlipThickMin;
	T_DS_S16	UpperTransmission;
	T_DS_S16	LowerTransmission;
	T_DS_U32	ChipIdLow;
	T_DS_U32	ChipIdHigh;
} T_NOSEPIECE;

typedef __packed struct {
    T_DS_HEAD   	Header;
	T_NOSEPIECE		T_NOSEPIECE[6];
} T_DS_OBJECTIVE;


#endif /* SRC_DESCRIPTORDEVICETYPES_H_ */
