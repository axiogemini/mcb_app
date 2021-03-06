/*
 * USB.h
 *
 *  Created on: 2016��3��13��
 *      Author: ZCTIAXIA
 */

#ifndef SRC_USB_H_
#define SRC_USB_H_

#define USBFRAME_MAXLEN 			64

#define MAX_LEN_USB2CAN_ROUTER		256

// define each index position in usb frame
#define USBIDX_TARGET 	      		2
#define USBIDX_SRC		      		3
#define USBIDX_LEN		      		4
#define USBIDX_CMDCL	      		5
#define USBIDX_CMDNUM	      		6
#define USBIDX_PID		      		7
#define USBIDX_SUBID	      		8
#define USBIDX_DEVID	      		9
#define USBIDX_DESC_ID0	      		9
#define USBIDX_BIOS_PAYLOAD   		9        // pay attention that BIOS commands has no devID so the payload is one byte earlier!
#define USBIDX_DESC_ID1	      		10
#define USBIDX_PAYLOAD        		10
#define USBIDX_DESC_ENTRY	      	11
#define USBIDX_DESC_FORMAT	   		12
#define USBIDX_DESC_DATA      		13

#define USB_DATA_FORMAT_WIDTH		1


// define command number
#define CN_MESSAGE	0x01	// chapter 2.1 "message handler" of ZIS_Spec_BiosCommands_1_0.pdf

typedef enum {
	FIND_CAN_ADDR = 1
}eMsgSubId;

// define command sub number
#define CSN_READ_DESC		0x01
#define CSN_READ_ENTRY		0x04
#define CSN_WRITE_ENTRY		0x10


// need rework the following commands, they're not really supported but dummy ones
#define CSN_LOCK_DESC		0x20
#define CSN_UPDATE_DESC		0x1f

typedef struct {
	uint8_t Len; // length of the total frame including 10 02 target src len cl ...10 03
	uint8_t	*pBuf;
	int8_t  MemProd;
} UsbTxTask;

#define GetByte(ptr,index)		*(ptr+index)

#define GetShort(ptr, index)    (Uint16)(*((uint8_t *)ptr+index)) << 8 | \
                                (Uint16)(*((uint8_t *)ptr+index + 1))

#define GetLong(ptr,index)      (Uint32)(*((uint8_t *)ptr+index)) << 24 | \
                                (Uint32)(*((uint8_t *)ptr+index + 1)) << 16 | \
                                (Uint32)(*((uint8_t *)ptr+index + 2)) << 8 | \
                                (Uint32)(*((uint8_t *)ptr+index + 3))

#define WriteByte(ptr, offset, value)   *((uint8_t *)ptr + offset) = value & 0xff

#define WriteShort(ptr, offset, value)  *((uint8_t *)ptr + offset) = value >> 8; \
                                        *((uint8_t *)ptr + offset + 1) = value & 0xff

#define WriteLong(ptr, offset, value)   *((uint8_t *)ptr + offset)     = ((Uint32)value >> 24)       ; \
                                        *((uint8_t *)ptr + offset + 1) = ((Uint32)value >> 16) & 0xff; \
                                        *((uint8_t *)ptr + offset + 2) = ((Uint32)value >>  8) & 0xff; \
                                        *((uint8_t *)ptr + offset + 3) = ((Uint32)value) & 0xff


typedef struct {
	uint8_t WriteIndex;
	uint8_t ReadIndex;
	uint8_t Buffer[BULK_BUFFER_SIZE];
} UsbRingBuf;

extern void USB_Init(void);
void USB_Bulk_Init(void);
//extern uint8_t UsbFrameProcess(Uint16 *pData);
uint8_t UsbFrameProcess(UsbRingBuf *pRing);

extern void AnswerWithData(uint8_t *pFrame, uint8_t Len, uint8_t *pData, uint8_t format);
extern void StartUsbTxTask(UsbTxTask *pData);
extern void Usb_Updater(void);
extern void AppCheck(void);
extern void SendFrameACK(uint8_t *pFrame);
extern void MakeupAnswerBios(uint8_t *pFrame, UsbTxTask *usbtx, uint8_t Len, void *pData);
extern void MakeupAnswerDesc(uint8_t *pFrame, UsbTxTask *usbtx, uint8_t Len, void *pData, uint8_t format);




#endif /* SRC_USB_H_ */
