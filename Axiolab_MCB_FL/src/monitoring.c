/*
 * monitoring.c
 *
 *  Created on: 2016Äê12ÔÂ23ÈÕ
 *      Author: zcleicai
 */


#include <stdio.h>
#include "include.h"
#include "maincontrol.h"
#include "usb.h"
#include "can29.h"
#include "monitoring.h"

#define NEW_FUNC 1

tDevMonitor MonDevice[NUM_MONITOR_DEV];

static bool alwaysStable;

void initMonitor(void)
{
    alwaysStable = true;

    memset((void *)&MonDevice[0], 0, sizeof(tDevMonitor)*NUM_MONITOR_DEV);

    MonDevice[eMON_REFLECTOR].nodeAddr = CANNODE_REFLECTOR;
    MonDevice[eMON_REFLECTOR].cmdNr = CN_CHANGER;
    MonDevice[eMON_REFLECTOR].pData = (void *)&g_Device.Reflector.Position;
    MonDevice[eMON_REFLECTOR].pStatus = (void *)&g_Device.Reflector.status;
    MonDevice[eMON_REFLECTOR].pStable = (void *)&g_Device.Reflector.Settled;
	MonDevice[eMON_REFLECTOR].deviceType = eMON_DEV_CHANGER;

    MonDevice[eMON_NOSEPIECE].nodeAddr = CANNODE_NOSEPIECE;
    MonDevice[eMON_NOSEPIECE].cmdNr = CN_CHANGER;
    MonDevice[eMON_NOSEPIECE].pData = (void *)&g_Device.NosePiece.Position;
    MonDevice[eMON_NOSEPIECE].pStatus = (void *)&g_Device.NosePiece.status;
    MonDevice[eMON_NOSEPIECE].pStable = (void *)&g_Device.NosePiece.Settled;
	MonDevice[eMON_NOSEPIECE].deviceType = eMON_DEV_CHANGER;

    MonDevice[eMON_RLTL].nodeAddr = CANNODE_RLTL;
    MonDevice[eMON_RLTL].cmdNr = CN_CHANGER;
    MonDevice[eMON_RLTL].pData = (void *)&g_Device.RLTLSw.Sw;
    MonDevice[eMON_RLTL].pStatus = (void *)&g_Device.RLTLSw.status;
    MonDevice[eMON_RLTL].pStable = (void *)&alwaysStable;       // digit changer than mechanical, always stable
    MonDevice[eMON_RLTL].deviceType = eMON_DEV_CHANGER;
    

    MonDevice[eMON_LIGHTCTRL].nodeAddr = CANNODE_LIGHTCTRL;
    MonDevice[eMON_LIGHTCTRL].cmdNr = CN_SERVO;
    MonDevice[eMON_LIGHTCTRL].pData = (void *)&g_Device.LampLed.pLed->Intensity_I;
    MonDevice[eMON_LIGHTCTRL].pStatus = (void *)&g_Device.LampLed.pLed->status;
	MonDevice[eMON_LIGHTCTRL].deviceType = eMON_DEV_SERVO;
}

bool addMonClient(Uint8 monDevIndex, tMonClient *pClient)
{
    int i;

    if (monDevIndex > NUM_MONITOR_DEV || pClient == NULL)
        return false;

    for (i=0; i<MAX_MONITOR_CLIENTS; i++) {                                     // if already has the client, only update the parameters
        if (MonDevice[monDevIndex].client[i].devId == pClient->devId &&
            MonDevice[monDevIndex].client[i].address == pClient->address) {
            MonDevice[monDevIndex].client[i].pid = pClient->pid;
            MonDevice[monDevIndex].client[i].interval = pClient->interval;
            MonDevice[monDevIndex].client[i].mode = pClient->mode;
            MonDevice[monDevIndex].bDataChanged = true;                         // first notification at set
            return true;
        }
    }

    if (MonDevice[monDevIndex].numOfClients >= MAX_MONITOR_CLIENTS)             // if no existing client, check if have room
        return false;

    for (i=0; i<MAX_MONITOR_CLIENTS; i++) {
        if (MonDevice[monDevIndex].client[i].mode == 0) {
            memcpy((void *)&MonDevice[monDevIndex].client[i], (void *)pClient, sizeof(tMonClient));
            MonDevice[monDevIndex].numOfClients++;
            MonDevice[monDevIndex].bDataChanged = true;
            return true;
        }
    }
    return true;
}

bool delMonClient(Uint8 monDevIndex, tMonClient *pClient)
{
    int i;

    if (MonDevice[monDevIndex].numOfClients == 0)
        return false;

    if (monDevIndex >= NUM_MONITOR_DEV || pClient == NULL)
        return false;

    for (i=0; i<MAX_MONITOR_CLIENTS; i++) {
        if (MonDevice[monDevIndex].client[i].devId == pClient->devId &&
            MonDevice[monDevIndex].client[i].address == pClient->address &&
            MonDevice[monDevIndex].client[i].pid == pClient->pid) {
            memset((void *)&MonDevice[monDevIndex].client[i], 0, sizeof(tMonClient));
            MonDevice[monDevIndex].numOfClients--;
            return true;
        }
    }
    return false;
}

bool isValidMode(Uint8 mode)
{
    if ((mode > 0x04 && mode < 0x11) || mode > 0x14)
        return false;
    else
        return true;
}

bool usbGetClient(Uint8 monDevIndex, uint8_t *pFrame, Int8 mode)
{
    int i, j;
    UsbTxTask usbtx;
    Uint8 usbtxmsg[USBFRAME_MAXLEN];
    Uint8 dataLen;
    tMonClient client;
    bool isDone;
    j = 0;
    isDone = false;
    for (i=MAX_MONITOR_CLIENTS; i>0; ) {    // strange! the TI compilor does not work with --i correctly...
        i--;
        if (monDevIndex >= NUM_MONITOR_DEV || pFrame == NULL)
            return false;

        memcpy((void *)&client, (void *)&MonDevice[monDevIndex].client[i], sizeof(tMonClient));

        if (client.mode != 0) {
            if (mode == eMON_MODE_MULTI) {
                if (++j == MonDevice[monDevIndex].numOfClients) {
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
                    isDone = true;
                } else
                    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x05;
            } else if (mode == eMON_MODE_LEGACY) {
                GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
                isDone = true;
            } else
                return false;

            dataLen = 5;
            usbtxmsg[0] = 0x10;
            usbtxmsg[1] = 0x02;
            GetByte(usbtxmsg, USBIDX_TARGET) = GetByte(pFrame, USBIDX_SRC);
            GetByte(usbtxmsg, USBIDX_SRC) = GetByte(pFrame, USBIDX_TARGET);
            GetByte(usbtxmsg, USBIDX_CMDNUM) = GetByte(pFrame, USBIDX_CMDNUM);
            GetByte(usbtxmsg, USBIDX_PID) = GetByte(pFrame, USBIDX_PID);
            GetByte(usbtxmsg, USBIDX_SUBID) = GetByte(pFrame, USBIDX_SUBID);
            GetByte(usbtxmsg, USBIDX_DEVID) = client.devId;
            GetByte(usbtxmsg, USBIDX_PAYLOAD) = client.mode;
            GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = (client.interval & 0xff00) >> 8;
            GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = client.interval & 0xff;
            GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = client.address;
            GetByte(usbtxmsg, USBIDX_PAYLOAD + 4) = client.pid;
            GetByte(usbtxmsg, USBIDX_PAYLOAD + dataLen) = 0x10;
            GetByte(usbtxmsg, USBIDX_PAYLOAD + dataLen + 1) = 0x03;
            usbtx.Len = dataLen + USB_PKT_OVERHEAD + CAN29_OVERHEAD;
            GetByte(usbtxmsg, USBIDX_LEN) = dataLen + CAN29_OVERHEAD;
            usbtx.pBuf = usbtxmsg;
            StartUsbTxTask(&usbtx);
            if (isDone == true)
                return true;
        }
    }

    // if not monitored
    if (mode == eMON_MODE_MULTI) {
        GetByte(usbtxmsg, USBIDX_CMDCL) = 0x09;
    } else if (mode == eMON_MODE_LEGACY) {
        GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
    } else
        return false;

    dataLen = 5;
    usbtxmsg[0] = 0x10;
    usbtxmsg[1] = 0x02;
    GetByte(usbtxmsg, USBIDX_TARGET) = GetByte(pFrame, USBIDX_SRC);
    GetByte(usbtxmsg, USBIDX_SRC) = GetByte(pFrame, USBIDX_TARGET);
    GetByte(usbtxmsg, USBIDX_CMDNUM) = GetByte(pFrame, USBIDX_CMDNUM);
    GetByte(usbtxmsg, USBIDX_PID) = GetByte(pFrame, USBIDX_PID);
    GetByte(usbtxmsg, USBIDX_SUBID) = GetByte(pFrame, USBIDX_SUBID);
    GetByte(usbtxmsg, USBIDX_DEVID) = client.devId;
    GetByte(usbtxmsg, USBIDX_PAYLOAD) = 0x00;
    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = 0x00;
    GetByte(usbtxmsg, USBIDX_PAYLOAD + 2) = 0x00;
    GetByte(usbtxmsg, USBIDX_PAYLOAD + 3) = 0x00;
    GetByte(usbtxmsg, USBIDX_PAYLOAD + 4) = 0x00;
    GetByte(usbtxmsg, USBIDX_PAYLOAD + dataLen) = 0x10;
    GetByte(usbtxmsg, USBIDX_PAYLOAD + dataLen + 1) = 0x03;
    usbtx.Len = dataLen + USB_PKT_OVERHEAD + CAN29_OVERHEAD;
    GetByte(usbtxmsg, USBIDX_LEN) = dataLen + CAN29_OVERHEAD;
    usbtx.pBuf = usbtxmsg;
    StartUsbTxTask(&usbtx);
    return true;
}

void monitorExec(void)
{
    int i, j;
    UsbTxTask usbtx;
    Uint8 usbtxmsg[USBFRAME_MAXLEN];
    Uint8 dataLen;

    // perhaps later we need add a "needUpdate" attribute to the monitored device
    MonDevice[eMON_LIGHTCTRL].pData = (void *)&g_Device.LampLed.pLed->Intensity_I;
    MonDevice[eMON_LIGHTCTRL].pStatus = (void *)&g_Device.LampLed.pLed->status;

    for (i=0; i<NUM_MONITOR_DEV; i++) {

        if (MonDevice[i].numOfClients == 0)
            continue;

        for (j=0; j<MAX_MONITOR_CLIENTS; j++) {

            if (MonDevice[i].client[j].mode == 0)
                continue;

            if (*MonDevice[i].pData != MonDevice[i].prevData) {
                MonDevice[i].bDataChanged = true;
				if (MonDevice[i].deviceType == eMON_DEV_CHANGER) {
					// for a changer, each time leave a valid position, the status should be ON_MOVING
					if (MonDevice[i].prevData != 0) {
						*MonDevice[i].pStatus |= (1 << CHG_BIT_MOVING);
						*MonDevice[i].pStatus &= ~(1 << CHG_BIT_VALID_POS);
					} else if ((MonDevice[i].prevData == 0) && (*MonDevice[i].pData != 0)) {
						*MonDevice[i].pStatus &= ~(1 << CHG_BIT_MOVING);
						*MonDevice[i].pStatus |= (1 << CHG_BIT_VALID_POS);
					}
				} else if (MonDevice[i].deviceType == eMON_DEV_SERVO) {
					*MonDevice[i].pStatus |= (1 << SVO_BIT_MOVING);
					*MonDevice[i].pStatus &= ~(1 << SVO_BIT_VALID_POS);
				}
            } else {	// the data not change
            	MonDevice[i].bDataChanged = false;
            	if (MonDevice[i].deviceType == eMON_DEV_CHANGER && *MonDevice[i].pData != 0) {
	            	*MonDevice[i].pStatus &= ~(1 << CHG_BIT_MOVING);
					*MonDevice[i].pStatus |= (1 << CHG_BIT_VALID_POS);
            	} else if (MonDevice[i].deviceType == eMON_DEV_SERVO) {
	            	*MonDevice[i].pStatus &= ~(1 << SVO_BIT_MOVING);
					*MonDevice[i].pStatus |= (1 << SVO_BIT_VALID_POS);
            	}
            }


            if (*MonDevice[i].pStatus != MonDevice[i].prevStatus)
                MonDevice[i].bStatusChanged = true;
            else
                MonDevice[i].bStatusChanged = false;

            switch (MonDevice[i].client[j].mode) {

                case 0x02:
                    if (MonDevice[i].bDataChanged != true)
                        break;
                    // intended fall through to case 0x01

                case 0x01:
                    if (GetSysTick0() - MonDevice[i].client[j].lastTxTime > MSEC100 * (MonDevice[i].client[j].interval/100)) {

						sendMonDataUint16(i, j, MONITOR_SUBID_DATA);
                    }
                    break;

                case 0x03:
                    // for changer only notify when it's stable, for motorized NOT support speed control yet
                    if (MonDevice[i].cmdNr == CN_CHANGER) {
                        if (*MonDevice[i].pData == 0 || *MonDevice[i].pStable == false)
                        break;
                    }
                    // intended fall through

                case 0x04:
                    if (MonDevice[i].bDataChanged == true) {
						sendMonDataUint16(i, j, MONITOR_SUBID_DATA);
                        MonDevice[i].client[j].lastTxTime = GetSysTick0();
                    }
                    break;

                case 0x12:
                    if (MonDevice[i].bDataChanged != true && MonDevice[i].bStatusChanged != true)
                        break;
                    
                case 0x11:	// intended fall through to case 0x11
                	// leilc:
                	// for changer, it does not affected with the interval. At each status change:
                	// 	VALID -> 0: status comes first with 0x400 bit, POS latter
                	// 	VALID1 -> VALID2: first status with 0x400, then POS, then status with 0x800
                	// 	0 -> VALID: POS first, status latter with 0x800
                	if (GetSysTick0() - MonDevice[i].client[j].lastTxTime > MSEC100 * (MonDevice[i].client[j].interval/100)) {
						if (MonDevice[i].client[j].mode == 0x11) {
							sendMonDataUint16(i, j, MONITOR_SUBID_DATA);
							MonDevice[i].client[j].lastTxTime = GetSysTick0();
						}
                    }
					if (MonDevice[i].deviceType == eMON_DEV_CHANGER) {
						if (MonDevice[i].bDataChanged == true) {
							if (((*MonDevice[i].pData == 0) && (MonDevice[i].prevData != 0)) ||
								((*MonDevice[i].pData != 0) && (MonDevice[i].prevData != 0))) {
								// leave a valid position, should send status first, POS afterwards
								sendMonDataUint16(i, j, MONITOR_SUBID_STATUS);
								sendMonDataUint16(i, j, MONITOR_SUBID_DATA);
							} else if ((*MonDevice[i].pData != 0) && (MonDevice[i].prevData == 0)) {
								// from 0 to valid position, need pos first then status with 0x800
								sendMonDataUint16(i, j, MONITOR_SUBID_DATA);
								sendMonDataUint16(i, j, MONITOR_SUBID_STATUS);
							} else
								;
						} else {		// in case valid pos1 to pos2, skipped 0, need an additional status here
							if (MonDevice[i].bStatusChanged == true) {
								sendMonDataUint16(i, j, MONITOR_SUBID_STATUS);
							}
						}
					} else if (MonDevice[i].deviceType == eMON_DEV_SERVO) {
						// for the servo, both pos change and status change, indicates start to move
						if ((MonDevice[i].bDataChanged == true) && (MonDevice[i].bStatusChanged == true)) {
							// status first, POS afterwards
							sendMonDataUint16(i, j, MONITOR_SUBID_STATUS);
							sendMonDataUint16(i, j, MONITOR_SUBID_DATA);
							MonDevice[i].client[j].lastTxTime = GetSysTick0();
						} else if ((MonDevice[i].bDataChanged == true) && (MonDevice[i].bStatusChanged == false)) {
							if (GetSysTick0() - MonDevice[i].client[j].lastTxTime > MSEC100 * (MonDevice[i].client[j].interval/100)) {
								sendMonDataUint16(i, j, MONITOR_SUBID_DATA);
							}
						} else if ((MonDevice[i].bDataChanged == false) && (MonDevice[i].bStatusChanged == true)) {
							sendMonDataUint16(i, j, MONITOR_SUBID_STATUS);
						}							
					}				
                    break;

                case 0x13:
                    // intended fall through

                case 0x14:
                    if (MonDevice[i].deviceType == eMON_DEV_CHANGER) {
						if (MonDevice[i].bDataChanged == true) {
							if (((*MonDevice[i].pData == 0) && (MonDevice[i].prevData != 0)) ||
								((*MonDevice[i].pData != 0) && (MonDevice[i].prevData != 0))) {
								// leave a valid position, should send status first, POS afterwards
								sendMonDataUint16(i, j, MONITOR_SUBID_STATUS);
								// for changer only notify when it's stable, for motorized NOT support speed control yet
			                    if ((MonDevice[i].cmdNr == CN_CHANGER) && (MonDevice[i].client[j].mode == 0x13)) {
			                        if (*MonDevice[i].pData == 0 || *MonDevice[i].pStable == false) {
			                        	break;
			                        }
			                    }
								sendMonDataUint16(i, j, MONITOR_SUBID_DATA);
							} else if ((*MonDevice[i].pData != 0) && (MonDevice[i].prevData == 0)) {
								// from 0 to valid position, need pos first then status with 0x800
								sendMonDataUint16(i, j, MONITOR_SUBID_DATA);
								sendMonDataUint16(i, j, MONITOR_SUBID_STATUS);
							} else
								;
						} else {		// moving -> stable
							if (MonDevice[i].bStatusChanged == true) {
								sendMonDataUint16(i, j, MONITOR_SUBID_STATUS);
							}
						}
					} else if (MonDevice[i].deviceType == eMON_DEV_SERVO) {
						// for the servo, both pos change and status change, indicates start to move
						if ((MonDevice[i].bDataChanged == true) && (MonDevice[i].bStatusChanged == true)) {
							// status first, POS afterwards
							sendMonDataUint16(i, j, MONITOR_SUBID_STATUS);
							sendMonDataUint16(i, j, MONITOR_SUBID_DATA);
							MonDevice[i].client[j].lastTxTime = GetSysTick0();
						} else if ((MonDevice[i].bDataChanged == true) && (MonDevice[i].bStatusChanged == false)) {
								sendMonDataUint16(i, j, MONITOR_SUBID_DATA);
						} else if ((MonDevice[i].bDataChanged == false) && (MonDevice[i].bStatusChanged == true)) {
							sendMonDataUint16(i, j, MONITOR_SUBID_STATUS);
						} else
							;
					}				
                    break;

                default:
                    break;
            }
            MonDevice[i].prevData = *MonDevice[i].pData;
            MonDevice[i].prevStatus = *MonDevice[i].pStatus;
        }
    }
}

void sendMonDataUint16(Uint8 devIndex, Uint8 clientIndex, Uint8 subId)
{
	UsbTxTask usbtx;
	Uint8 usbtxmsg[USBFRAME_MAXLEN];
	Uint8 dataLen;
	Uint16 data;

	dataLen = 2;				// send Uint16 data
    usbtxmsg[0] = 0x10;
    usbtxmsg[1] = 0x02;

	if (subId == MONITOR_SUBID_DATA)
		data = *MonDevice[devIndex].pData;
	else if (subId == MONITOR_SUBID_STATUS)
		data = *MonDevice[devIndex].pStatus;
	else
		return;
	
    GetByte(usbtxmsg, USBIDX_TARGET) = MonDevice[devIndex].client[clientIndex].address;
    GetByte(usbtxmsg, USBIDX_SRC) = MonDevice[devIndex].nodeAddr;
    GetByte(usbtxmsg, USBIDX_CMDCL) = 0x07;
    GetByte(usbtxmsg, USBIDX_CMDNUM) = MonDevice[devIndex].cmdNr;
    GetByte(usbtxmsg, USBIDX_PID) = MonDevice[devIndex].client[clientIndex].pid;
    GetByte(usbtxmsg, USBIDX_DEVID) = MonDevice[devIndex].client[clientIndex].devId;

	GetByte(usbtxmsg, USBIDX_SUBID) = subId;
    GetByte(usbtxmsg, USBIDX_PAYLOAD) = (data & 0xff00) >> 8;
    GetByte(usbtxmsg, USBIDX_PAYLOAD + 1) = data & 0xff;
    GetByte(usbtxmsg, USBIDX_PAYLOAD + dataLen) = 0x10;
    GetByte(usbtxmsg, USBIDX_PAYLOAD + dataLen + 1) = 0x03;
    usbtx.Len = dataLen + USB_PKT_OVERHEAD + CAN29_OVERHEAD;
    GetByte(usbtxmsg, USBIDX_LEN) = dataLen + CAN29_OVERHEAD;
	usbtx.pBuf = usbtxmsg;
	StartUsbTxTask(&usbtx);
}


