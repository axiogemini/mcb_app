/*
 * nosepiece.c
 *
 *  Created on: 2016��12��20��
 *      Author: zcleicai
 */

#include "include.h"
#include "maincontrol.h"
#include "nosepiece.h"
#include "usb.h"
#include "can29.h"


char AppName_Nosepiece[] = "GEM_Nosepiece";

 void AnswerBios_Nosepiece(uint8_t *pFrame)
 {
     UsbTxTask usbtx;
     uint8_t i, usbtxmsg[USBFRAME_MAXLEN];
     uint8_t string_len;
 
     usbtxmsg[0] = 0x10;
     usbtxmsg[1] = 0x02;
     GetByte(usbtxmsg, USBIDX_TARGET) = GetByte(pFrame, USBIDX_SRC);
     GetByte(usbtxmsg, USBIDX_SRC) = CANNODE_NOSEPIECE;
     GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
     GetByte(usbtxmsg, USBIDX_CMDNUM) = GetByte(pFrame, USBIDX_CMDNUM);
     GetByte(usbtxmsg, USBIDX_PID) = GetByte(pFrame, USBIDX_PID);
     GetByte(usbtxmsg, USBIDX_SUBID) = GetByte(pFrame, USBIDX_SUBID);
 //  GetByte(usbtxmsg, USBIDX_DEVID) = GetByte(pFrame, USBIDX_DEVID);  // leilc: the BIOS commands does not have deviceID
     // leilc: what's this?
     if (GetByte(usbtxmsg, USBIDX_SUBID) == FIND_CAN_ADDR) {
         GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD) = CANNODE_NOSEPIECE;
         i = 1;
         string_len = 1;
     }else {
         switch (GetByte(pFrame, USBIDX_SUBID)) {
             case 0x07:   // app version
                 string_len = strlen(AppVersion);
                 for (i=0; i<string_len; i++) {
                     GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD + i) = *(AppVersion+i);
                 }
                 break;
             case 0x08:
                 string_len = strlen(AppDate);
                 for (i=0; i<string_len; i++) {
                     GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD + i) = *(AppDate+i);
                 }
                 break;
             case 0x09:
                 string_len = strlen(AppName_Nosepiece);
                 for (i=0; i<string_len; i++) {
                     GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD + i) = *(AppName_Nosepiece +i);
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

 void ChangerHandler_Nosepiece(uint8_t *pFrame)
 {
     UsbTxTask usbtx;
     uint8_t usbtxmsg[USBFRAME_MAXLEN];
     uint8_t dataLen;
     tMonClient clientNosepiece;
 
     usbtxmsg[0] = 0x10;
     usbtxmsg[1] = 0x02;
     GetByte(usbtxmsg, USBIDX_TARGET) = GetByte(pFrame, USBIDX_SRC);
     GetByte(usbtxmsg, USBIDX_SRC) = CANNODE_NOSEPIECE;
     GetByte(usbtxmsg, USBIDX_CMDNUM) = GetByte(pFrame, USBIDX_CMDNUM);
     GetByte(usbtxmsg, USBIDX_PID) = GetByte(pFrame, USBIDX_PID);
     GetByte(usbtxmsg, USBIDX_SUBID) = GetByte(pFrame, USBIDX_SUBID);
     GetByte(usbtxmsg, USBIDX_DEVID) = GetByte(pFrame, USBIDX_DEVID);
 
     // todo: filter those devId != 0, because only have one device with ID 0
 
     switch (GetByte(pFrame, USBIDX_SUBID)) {
         case 0x00:   // init device
             dataLen = 2;
             GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
             GetByte(usbtxmsg, USBIDX_PAYLOAD) = (g_Device.NosePiece.Position & 0xff00) >> 8;
             GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.NosePiece.Position & 0xff;
             break;
         case 0x01:   // read device status
             dataLen = 2;
             GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
             GetByte(usbtxmsg, USBIDX_PAYLOAD) = (g_Device.NosePiece.status & 0xff00) >> 8;
             GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.NosePiece.status & 0xff;
             break;
         case 0x02:  // move absolute, intended fall through
         case 0x12:  // move absolute locked, intended fall through
         case 0x05:  // stop device, intended fall through
            if (GetByte(pFrame, USBIDX_SUBID) == 0x02 && GetByte(pFrame, USBIDX_CMDCL) == 0x18) {
                dataLen = 2;
				if (GetByte(pFrame, USBIDX_DEVID) == 0x80) {
					GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
	                GetByte(usbtxmsg, USBIDX_PAYLOAD) = (g_Device.NosePiece.Encoder& 0xff00) >> 8;
	                GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.NosePiece.Encoder & 0xff;
				} else {
	                GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
	                GetByte(usbtxmsg, USBIDX_PAYLOAD) = (g_Device.NosePiece.Position & 0xff00) >> 8;
	                GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.NosePiece.Position & 0xff;
				}
            } else {
                dataLen = 4;
                GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
                GetByte(usbtxmsg, USBIDX_PAYLOAD) = (g_Device.NosePiece.Position & 0xff00) >> 8;
                GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = g_Device.NosePiece.Position & 0xff;
                GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = (ERR_CMD_NOT_IMPLEMENTED & 0xff00) >> 8;
                GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = ERR_CMD_NOT_IMPLEMENTED & 0xff;
            }
            break;

        case 0x08:  // read number of positions
             dataLen = 2;
             GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
             GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0;
             if (g_Device.Serie == Axioscope)
                GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = 6;
             else
                GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = 5;
             break;

        case 0x1f:  // device monitoring
            if (GetByte(pFrame, USBIDX_CMDCL) == 0x19 || GetByte(pFrame, USBIDX_CMDCL) == 0x1B) {
                clientNosepiece.devId = GetByte(pFrame, USBIDX_DEVID);
                clientNosepiece.mode = GetByte(pFrame, USBIDX_PAYLOAD);
                clientNosepiece.interval = (GetByte(pFrame, USBIDX_PAYLOAD + 1) << 8) + GetByte(pFrame, USBIDX_PAYLOAD + 2);
                clientNosepiece.address = GetByte(pFrame, USBIDX_PAYLOAD + 3);
                clientNosepiece.pid = GetByte(pFrame, USBIDX_PAYLOAD + 4);
                if (!isValidMode(clientNosepiece.mode) || clientNosepiece.address == 0) {
                    dataLen = 7;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = clientNosepiece.mode;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = (clientNosepiece.interval & 0xff00) >> 8;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = clientNosepiece.interval & 0xff;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = clientNosepiece.address;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 4) = clientNosepiece.pid;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 5) = (ERR_INVALID_PARAM & 0xff00) >> 8;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 6) = ERR_INVALID_PARAM & 0xff;
                 } else {
                    if (clientNosepiece.mode > 0) {
                        if (addMonClient(eMON_NOSEPIECE, &clientNosepiece) == false)
                            return;     // todo: add error message here
                    } else {
                        if (delMonClient(eMON_NOSEPIECE, &clientNosepiece) == false)
                            return;
                    }
                    if (GetByte(pFrame, USBIDX_CMDCL) == 0x1B)
                        return;
                    dataLen = 5;
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD) = clientNosepiece.mode;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = (clientNosepiece.interval & 0xff00) >> 8;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = clientNosepiece.interval & 0xff;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = clientNosepiece.address;
                    GetByte(usbtxmsg, USBIDX_PAYLOAD + 4) = clientNosepiece.pid;
                }
                break;
            } else if (GetByte(pFrame, USBIDX_CMDCL) == 0x15) {
                usbGetClient(eMON_NOSEPIECE, pFrame, eMON_MODE_MULTI);
                return;
            } else if (GetByte(pFrame, USBIDX_CMDCL) == 0x18) {
                usbGetClient(eMON_NOSEPIECE, pFrame, eMON_MODE_LEGACY);
                return;
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




