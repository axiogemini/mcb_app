/*
 * monitoring.h
 *
 *  Created on: 2016Äê12ÔÂ23ÈÕ
 *      Author: zcleicai
 */

#ifndef SRC_MONITORING_H_
#define SRC_MONITORING_H_

#define MONITOR_SUBID_DATA              0x02
#define MONITOR_SUBID_STATUS            0x01


#define         MAX_MONITOR_CLIENTS     5

typedef struct {
    Uint8       devId;
    Uint8       mode;
    Uint8       address;
    Uint8       pid;
    Uint16      interval;
    Uint32      lastTxTime;
}tMonClient;

typedef struct {
    bool        bDataChanged;
    bool        bStatusChanged;
    // leilc: the following two reserved for motorized elements
    bool        bMoveStarted;
    bool        bMoveCompleted;
	Uint8		deviceType;
    Uint8       nodeAddr;
    Uint8       cmdNr;
    Uint8       numOfClients;
    // leilc: for the servo and changer device the monitored data and status are Uint16
    // for simple just hard coded the data and status here as Uint16
    // if need support other device than these, please check if need modification
    Uint16 *    pData;
    Uint16 *    pStatus;
    bool *      pStable;            // for changer device
    Uint16      prevData;
    Uint16      prevStatus;

    tMonClient  client[MAX_MONITOR_CLIENTS];
}tDevMonitor;

enum {
    eMON_REFLECTOR,
    eMON_NOSEPIECE,
    eMON_RLTL,
    eMON_LIGHTCTRL,
    NUM_MONITOR_DEV
};

enum {
	eMON_DEV_CHANGER,
	eMON_DEV_SERVO,
	eMON_DEV_AXIS
};

enum {
    eMON_MODE_MULTI,
    eMON_MODE_LEGACY
};

void initMonitor(void);
bool addMonClient(Uint8 monDevIndex, tMonClient *pClient);
bool delMonClient(Uint8 monDevIndex, tMonClient *pClient);
bool isValidMode(Uint8 mode);
bool usbGetClient(Uint8 monDevIndex, uint8_t *pFrame, Int8 mode);

void monitorExec(void);
void sendMonDataUint16(Uint8 devIndex, Uint8 clientIndex, Uint8 subId);



#endif /* SRC_MONITORING_H_ */
