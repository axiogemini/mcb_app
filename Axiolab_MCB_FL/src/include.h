/*
 * Include.h
 *
 *  Created on: 2015?4?24?
 *      Author: xiatian
 */

#ifndef INCLUDE_H_
#undef INCLUDE_H_

#define  FLASH
#define  FW_UPDATE 0x55555555
#define  FWBL 0xffff0000
#define  FWAPP 0xaaaaaaaa

#include "math.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_usb.h"

#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/usb.h"
#include "driverlib/usb_hal.h"

#include "usblib/usblib.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdbulk.h"

#include "utils/uartstdio.h"

#include "usb_bulk_structs.h"
#include "Flash2806x_API_Library.h"

//#include "PeripheralHeaderIncludes.h"
#include "F2806x_Device.h"
//#include "F2806x_GlobalPrototypes.h"
#include "F2806x_Config.h"
#include "F2806x_CpuTimers.h"
#include "Adc.h"
#include "app.h"
#include "CAN29.h"
#include "CmdHandler.h"
#include "DescriptorTask.h"
#include "Device_Init.h"
#include "ECAN.h"
#include "eeprom.h"
#include "eeprom_mmap.h"
#include "EPWM.h"
#include "EQEP.h"
#include "I2C.h"
#include "lightcontrol.h"
#include "LightManager.h"
#include "MainControl.h"
#include "monitoring.h"
#include "nosepiece.h"
#include "Parcenter.h"
#include "rltl.h"
#include "reflector.h"
#include "SCI.h"
#include "SPI.h"
#include "TaskMgr.h"
#include "Timer.h"
#include "USB.h"
#include "XINT.h"
#include "monitoring.h"
#include "TLShutter.h"


#endif /* INCLUDE_H_ */
