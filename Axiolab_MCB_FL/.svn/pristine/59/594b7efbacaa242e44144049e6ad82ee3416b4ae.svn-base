/*
 * rltl.c
 *
 *  Created on: 2016Äê12ÔÂ20ÈÕ
 *      Author: zcleicai
 */

#include "include.h"
#include "maincontrol.h"
#include "rltl.h"
#include "usb.h"
#include "can29.h"
#include "monitoring.h"

char AppName_RLTL[] = "GEM_RL/TL_Switch";

 void AnswerBios_RLTL(uint8_t *pFrame)
 {
    UsbTxTask usbtx;
    Uint8 i, usbtxmsg[USBFRAME_MAXLEN];
    Uint8 string_len;
 
    usbtxmsg[0] = 0x10;
    usbtxmsg[1] = 0x02;
    GetByte(usbtxmsg, USBIDX_TARGET) = GetByte(pFrame, USBIDX_SRC);
    GetByte(usbtxmsg, USBIDX_SRC) = CANNODE_RLTL;
    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
    GetByte(usbtxmsg, USBIDX_CMDNUM) = GetByte(pFrame, USBIDX_CMDNUM);
    GetByte(usbtxmsg, USBIDX_PID) = GetByte(pFrame, USBIDX_PID);
    GetByte(usbtxmsg, USBIDX_SUBID) = GetByte(pFrame, USBIDX_SUBID);
 // GetByte(usbtxmsg, USBIDX_DEVID) = GetByte(pFrame, USBIDX_DEVID);  // leilc: the BIOS commands does not have deviceID
    // leilc: what's this?
    if (GetByte(usbtxmsg, USBIDX_SUBID) == FIND_CAN_ADDR) {
        GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD) = CANNODE_RLTL;
        i = 1;
        string_len = 1;
    }else {
        switch (GetByte(pFrame, USBIDX_SUBID)) {
			case 0x05:	// bios version
            case 0x07:	// app version
                string_len = strlen(AppVersion);
                for (i=0; i<string_len; i++) {
                    GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD + i) = *(AppVersion+i);
                }
                break;
			case 0x06:
            case 0x08:
                string_len = strlen(AppDate);
                for (i=0; i<string_len; i++) {
                    GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD + i) = *(AppDate+i);
                }
                break;
			case 0x0a:
            case 0x09:
                string_len = strlen(AppName_RLTL);
                for (i=0; i<string_len; i++) {
                    GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD + i) = *(AppName_RLTL +i);
                }
                break;
 
            default :
                return;
        }
    }
    GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD + i++) = 0x10;
    GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD + i++) = 0x03;
    usbtx.Len = string_len + USB_PKT_OVERHEAD + CAN29_BIOS_OVERHEAD;
    GetByte(usbtxmsg, USBIDX_LEN) = string_len + CAN29_BIOS_OVERHEAD;
    usbtx.pBuf = usbtxmsg;
    StartUsbTxTask(&usbtx);
}

void ChangerHandler_RLTL(uint8_t *pFrame) {
    UsbTxTask usbtx;
    Uint8 usbtxmsg[USBFRAME_MAXLEN];
    Uint8 dataLen;
    Uint16 targetPos;
    Int16 relPos;
    tMonClient clientRLTL;
    int i;

    usbtxmsg[0] = 0x10;
    usbtxmsg[1] = 0x02;
    GetByte(usbtxmsg, USBIDX_TARGET) = GetByte(pFrame, USBIDX_SRC);
    GetByte(usbtxmsg, USBIDX_SRC) = CANNODE_RLTL;
    GetByte(usbtxmsg, USBIDX_CMDNUM) = GetByte(pFrame, USBIDX_CMDNUM);
    GetByte(usbtxmsg, USBIDX_PID) = GetByte(pFrame, USBIDX_PID);
    GetByte(usbtxmsg, USBIDX_SUBID) = GetByte(pFrame, USBIDX_SUBID);
    GetByte(usbtxmsg, USBIDX_DEVID) = GetByte(pFrame, USBIDX_DEVID);

    switch (GetByte(pFrame, USBIDX_SUBID)) {
        case 0x00:   // init device
            if (GetByte(pFrame, USBIDX_CMDCL) == 0x19) {
                 dataLen = 2;
                 GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                 GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
                 GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.RLTLSw.Sw & 0xff;
                 break;
            } else
                return;

        case 0x01:   // read device status
            if (GetByte(pFrame, USBIDX_CMDCL) == 0x18) {
                 dataLen = 2;
                 GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
                 GetByte(usbtxmsg, USBIDX_PAYLOAD) = (g_Device.RLTLSw.status & 0xff00) >> 8;
                 GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.RLTLSw.status & 0xff;
                 break;
            } else
                return;
 
        case 0x02:  // move absolute
            if (GetByte(pFrame, USBIDX_CMDCL) == 0x1B || GetByte(pFrame, USBIDX_CMDCL) == 0x19) {
                if (g_Device.RLTLSw.Lock.Status == true) {
                    dataLen = 4;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.RLTLSw.Sw;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = (ERR_DEV_LOCKED & 0xff00) >> 8;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = ERR_DEV_LOCKED & 0xff;
                 } else if (GetByte(pFrame, USBIDX_PAYLOAD + 1) > MAX_POSITIONS_RLTL) {
                    dataLen = 4;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.RLTLSw.Sw;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = (ERR_INVALID_PARAM & 0xff00) >> 8;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = ERR_INVALID_PARAM & 0xff;
                 } else {
                    g_Device.RLTLSw.Sw = GetByte(pFrame, USBIDX_PAYLOAD + 1);
                    switchLampLed(g_Device.RLTLSw.Sw);
                    if (GetByte(pFrame, USBIDX_CMDCL) == 0x1B)
                        return;
                    dataLen = 2;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = GetByte(pFrame, USBIDX_PAYLOAD + 1);
                 }
                 break;
            } else if (GetByte(pFrame, USBIDX_CMDCL) == 0x18) {
                dataLen = 2;
                GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
                GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
                GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.RLTLSw.Sw;
                break;
            } else
                return;

        case 0x12:  // move absolute locked
            if (GetByte(pFrame, USBIDX_CMDCL) == 0x1B || GetByte(pFrame, USBIDX_CMDCL) == 0x19) {
                 if (g_Device.RLTLSw.Lock.Status == false) {
                    dataLen = 4;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.RLTLSw.Sw;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = (ERR_DEV_NOT_LOCKED & 0xff00) >> 8;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = ERR_DEV_NOT_LOCKED & 0xff;
                } else if (GetByte(pFrame, USBIDX_PAYLOAD + 1) > MAX_POSITIONS_RLTL) {
                    dataLen = 4;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.RLTLSw.Sw;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = (ERR_INVALID_PARAM & 0xff00) >> 8;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = ERR_INVALID_PARAM & 0xff;
                } else {
                    g_Device.RLTLSw.Sw = GetByte(pFrame, USBIDX_PAYLOAD + 1);
                    switchLampLed(g_Device.RLTLSw.Sw);
                    if (GetByte(pFrame, USBIDX_CMDCL) == 0x1B)
                        return;
                    dataLen = 2;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = GetByte(pFrame, USBIDX_PAYLOAD + 1);
                 }
                 break;
            } else
                return;

        case 0x03:  // move relative
            if (GetByte(pFrame, USBIDX_CMDCL) == 0x1B || GetByte(pFrame, USBIDX_CMDCL) == 0x19) {
                relPos = (GetByte(pFrame, USBIDX_PAYLOAD) << 8) + GetByte(pFrame, USBIDX_PAYLOAD + 1);
                targetPos = g_Device.RLTLSw.Sw + relPos;
                targetPos = targetPos % MAX_POSITIONS_RLTL;
                targetPos = (targetPos > 0) ? targetPos : (targetPos + MAX_POSITIONS_RLTL);
                if (g_Device.RLTLSw.Lock.Status == true) {
                    dataLen = 4;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.RLTLSw.Sw;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = (ERR_DEV_LOCKED & 0xff00) >> 8;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = ERR_DEV_LOCKED & 0xff;
                } else {
                    g_Device.RLTLSw.Sw = GetByte(pFrame, USBIDX_PAYLOAD + 1);
                    switchLampLed(g_Device.RLTLSw.Sw);
                    if (GetByte(pFrame, USBIDX_CMDCL) == 0x1B)
                        return;
                    dataLen = 2;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = GetByte(pFrame, USBIDX_PAYLOAD + 1);
                }
                break;
            } else
                return;

        case 0x13:  // move relative locked
            if (GetByte(pFrame, USBIDX_CMDCL) == 0x1B || GetByte(pFrame, USBIDX_CMDCL) == 0x19) {
                relPos = (GetByte(pFrame, USBIDX_PAYLOAD) << 8) + GetByte(pFrame, USBIDX_PAYLOAD + 1);
                targetPos = g_Device.RLTLSw.Sw + relPos;
                targetPos = targetPos % MAX_POSITIONS_RLTL;
                targetPos = (targetPos > 0) ? targetPos : (targetPos + MAX_POSITIONS_RLTL);
                if (g_Device.RLTLSw.Lock.Status == false) {
                    dataLen = 4;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.RLTLSw.Sw;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = (ERR_DEV_NOT_LOCKED & 0xff00) >> 8;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = ERR_DEV_NOT_LOCKED & 0xff;
                } else {
                    g_Device.RLTLSw.Sw = GetByte(pFrame, USBIDX_PAYLOAD + 1);
                    switchLampLed(g_Device.RLTLSw.Sw);
                    if (GetByte(pFrame, USBIDX_CMDCL) == 0x1B)
                        return;
                    dataLen = 2;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = GetByte(pFrame, USBIDX_PAYLOAD + 1);
                }
                break;
            } else
                return;

        case 0x05:  // stop device
             dataLen = 4;
             GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
             GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
             GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.RLTLSw.Sw & 0xff;
             GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = (ERR_CMD_NOT_IMPLEMENTED & 0xff00) >> 8;
             GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = ERR_CMD_NOT_IMPLEMENTED & 0xff;
             break;
 
        case 0x08:  // read number of positions
             dataLen = 2;
             GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
             GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
             GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = 2;
             break;

        case 0x1f:  // device monitoring
            if (GetByte(pFrame, USBIDX_CMDCL) == 0x19 || GetByte(pFrame, USBIDX_CMDCL) == 0x1B) {
                clientRLTL.devId = GetByte(pFrame, USBIDX_DEVID);
                clientRLTL.mode = GetByte(pFrame, USBIDX_PAYLOAD);
                clientRLTL.interval = (GetByte(pFrame, USBIDX_PAYLOAD + 1) << 8) + GetByte(pFrame, USBIDX_PAYLOAD + 2);
                clientRLTL.address = GetByte(pFrame, USBIDX_PAYLOAD + 3);
                clientRLTL.pid = GetByte(pFrame, USBIDX_PAYLOAD + 4);
                if (!isValidMode(clientRLTL.mode) || clientRLTL.address == 0) {
                    dataLen = 7;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = clientRLTL.mode;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = (clientRLTL.interval & 0xff00) >> 8;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = clientRLTL.interval & 0xff;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = clientRLTL.address;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 4) = clientRLTL.pid;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 5) = (ERR_INVALID_PARAM & 0xff00) >> 8;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 6) = ERR_INVALID_PARAM & 0xff;
                 } else {
                    if (clientRLTL.mode > 0) {
                        if (addMonClient(eMON_RLTL, &clientRLTL) == false)
                            return;     // todo: add error message here
                    } else {
                        if (delMonClient(eMON_RLTL, &clientRLTL) == false)
                            return;
                    }
                    if (GetByte(pFrame, USBIDX_CMDCL) == 0x1B)
                        return;
                    dataLen = 5;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = clientRLTL.mode;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = (clientRLTL.interval & 0xff00) >> 8;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = clientRLTL.interval & 0xff;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = clientRLTL.address;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 4) = clientRLTL.pid;
                }
                break;
            } else if (GetByte(pFrame, USBIDX_CMDCL) == 0x15) {
                usbGetClient(eMON_RLTL, pFrame, eMON_MODE_MULTI);
                return;
            } else if (GetByte(pFrame, USBIDX_CMDCL) == 0x18) {
                usbGetClient(eMON_RLTL, pFrame, eMON_MODE_LEGACY);
                return;
            } else
                return;
 
        case 0x61:  // lock / unlock device
            if (GetByte(pFrame, USBIDX_CMDCL) == 0x1B || GetByte(pFrame, USBIDX_CMDCL) == 0x19) {
                g_Device.RLTLSw.Lock.Status= (GetByte(pFrame, USBIDX_PAYLOAD) != 0) ? true : false;
                g_Device.RLTLSw.Lock.StartTime = GetSysTick0();
                if (GetByte(pFrame, USBIDX_CMDCL) == 0x1B)
                    return;
                dataLen = 1;
                GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                GetByte(usbtxmsg, USBIDX_PAYLOAD) = g_Device.RLTLSw.Lock.Status;
                break;
            } else if (GetByte(pFrame, USBIDX_CMDCL) == 0x18) {
                dataLen = 1;
                GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
                GetByte(usbtxmsg, USBIDX_PAYLOAD) = g_Device.RLTLSw.Lock.Status;
                break;
            } else
                return;
        default :
             return;
    }
 
    GetByte(usbtxmsg, USBIDX_PAYLOAD + dataLen) = 0x10;
    GetByte(usbtxmsg, USBIDX_PAYLOAD + dataLen + 1) = 0x03;
    usbtx.Len = dataLen + USB_PKT_OVERHEAD + CAN29_OVERHEAD;
    GetByte(usbtxmsg, USBIDX_LEN) = dataLen + CAN29_OVERHEAD;
    usbtx.pBuf = usbtxmsg;
    StartUsbTxTask(&usbtx);
}



