/*
 * TLShutter.c
 *
 *  Created on: 2017?2?21?
 *      Author: ZCTIAXIA
 */

#include "include.h"

static eShutterDir task = kShutterOn;
static eShutterDir status = kShutterOff;
static eShutterStatus process = kShutter_sts00;



// kShutterOff: default;  kShutterOn: reverse
static void
SetDirGpio(eShutterDir direction)
{
    if (direction == kShutterOn) {
        GpioDataRegs.GPBSET.bit.GPIO58 = 1;
    }else if (direction == kShutterOff) {
        GpioDataRegs.GPBCLEAR.bit.GPIO58 = 1;
    }
}

static void
SetPowerGpio(bool power)
{
    if (power) {
        GpioDataRegs.GPASET.bit.GPIO5 = 1;
    }else {
        GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
    }
}

void
ShutterMgr(void)
{
    switch (process) {
    case kShutter_sts00:                    // set direction
        if (task == status)
            return;
        if (!EPwmTimerisOver(2))
            return;
        if (task == kShutterOn) {
            SetDirGpio(kShutterOn);
            status = kShutterOn;
        }else if (task == kShutterOff) {
            SetDirGpio(kShutterOff);
            status = kShutterOff;
        }
        EPwmTimerStart(2, 50000);
        process = kShutter_sts01;
        break;
    case kShutter_sts01:                    // wait for 50ms
        if (EPwmTimerisOver(2))
            process = kShutter_sts02;
        else if (status != task)
            process = kShutter_sts00;
        break;
    case kShutter_sts02:                    // power on solenoid
        if (!EPwmTimerisOver(2))
            return;
        SetPowerGpio(true);
        EPwmTimerStart(2, 50000);
        process = kShutter_sts03;
        break;
    case kShutter_sts03:                    // wait for 50ms
        if (EPwmTimerisOver(2))
            process = kShutter_sts04;
        if (status != task)
            process = kShutter_sts04;
        break;
    case kShutter_sts04:                    // power off solenoid
        if (!EPwmTimerisOver(2))
            return;
        SetPowerGpio(false);
        EPwmTimerStart(2, 5000);
        process = kShutter_sts05;
        break;
    case kShutter_sts05:                    // wait for 5ms
        if (EPwmTimerisOver(2))
            process = kShutter_sts00;
        if (task != status)
            process = kShutter_sts00;
        break;
    default: break;
    }
}

void
TLShutterSet(void)
{
    if (g_Device.Serie == Axiolab)
        return;

    if (g_Device.VarType != VAR_FL)
        return;

    if (g_DeviceShadow.RLTLSw.Sw == g_Device.RLTLSw.Sw)
        return;

    if (g_Device.RLTLSw.Sw == LSW_UPPER) {          // TL shutter on
        task = kShutterOn;
    }else if (g_Device.RLTLSw.Sw == LSW_LOWER) {    // TL shutter off
        task = kShutterOff;
    }
}
