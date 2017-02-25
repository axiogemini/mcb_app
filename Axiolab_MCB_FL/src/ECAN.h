/*
 * ECAN.h
 *
 *  Created on: 2015Äê4ÔÂ27ÈÕ
 *      Author: xiatian
 */


#ifndef ECAN_H_
#define ECAN_H_

#define MAX_SLICE_NUM				10
#define MAX_CHAIN_MSG_SENDER 		5				// max concurrent chain message sender
#define NO_SLICE_MSG_SPACE			255
#define NO_SLICE_MSG_MATCH			255
#define USB_PKT_OVERHEAD			9       		// STX, ETX: 4;  ID: 4;  length: 1
#define CAN29_OVERHEAD              3               // pid, subid, devid
#define CAN29_BIOS_OVERHEAD         2               // pid, subid
#define CAN29_DESC_OVERHEAD			6				// pid, subid, Uint16 DescrID, EntryNbr, DataFormat

#define CANID_CLASS(x)	((x.id >> 24) & 0x1F)
#define CANID_DEST(x)	((x.id >> 16) & 0XFF)
#define CANID_SOURCE(x)	((x.id >>  8) & 0xFF)
#define CANID_CMDNUM(x)	((x.id >>  0) & 0xFF)

#define CAN_MAILBOX_EN_MASK         (0xf00001ff)

// CAN message object structure
typedef struct {
	Uint32 id;										// 29 bit identifier
	Uint8 data[8];									// Data field
	Uint8 len;										// Length of data field in bytes 
} CAN_msg;

typedef struct {
	Uint8 dle;
	Uint8 stx;
	Uint8 dest;
	Uint8 src;
	Uint8 dataLen;
	Uint8 cmdCls;
	Uint8 cmdNum;
	Uint8 data[8*MAX_SLICE_NUM];
	Uint8 segment;
} slice_msg;

extern void ECAN_Init(void);
extern void ECAN_Config(void);
extern void ECAN_Listen(void);
void ECAN_SendMail(Uint16 mboxindex, C29ID id, uint8_t *pData, uint8_t length);
void initSliceMsg(void);

#endif /* ECAN_H_ */

