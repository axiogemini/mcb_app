/*
 * Device_Init.h
 *
 *  Created on: 2013-7-19
 *      Author:
 */

#ifndef DEVICE_INIT_H_
#define DEVICE_INIT_H_

extern void Device_Init(void);
extern void GPIO_Init(void);
extern void SystemInit(void);
void Init_Peripherals(void);
void DisablePeripherals(void);
extern void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);

#endif /* DEVICE_INIT_H_ */
