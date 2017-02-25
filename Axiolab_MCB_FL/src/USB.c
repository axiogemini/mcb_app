/*
 * USB.c
 *
 *  Created on: 2016Äê3ÔÂ13ÈÕ
 *      Author: ZCTIAXIA
 */

#include <ctype.h>
#include "include.h"

#define TOTALPOSITION 25000000


void
USB_Init(void)
{
    // Enable Device Mode
    EALLOW;
    HWREG(USBMODESEL) = USBMODESEL_DEV;
    HWREG(USB0_BASE + USB_O_GPCS) = USBGPCS_DEV;
    //Enable PHY
    GpioCtrlRegs.GPACTRL2.bit.USB0IOEN = 1;
    EDIS;
}

void
USB_Bulk_Init(void)
{
    if (g_Device.Serie == Axiolab)
        *(uint16_t*)(&g_sBulkDevice.ui16PID) = 0x6000;
    else if(g_Device.Serie == Axioscope)
        *(uint16_t*)(&g_sBulkDevice.ui16PID) = 0x6001;
    else
        *(uint16_t*)(&g_sBulkDevice.ui16PID) = 0xbbbb;

    USBBufferInit(&g_sTxBuffer);
    USBBufferInit(&g_sRxBuffer);
    USBDBulkInit(0, &g_sBulkDevice);

	UsbRingBufInit();
    // Wait for initial configuration to complete.
    //UARTprintf("Waiting for host...\n");
}

void
Usb_Updater(void)
{
#if FW_UPDATE
    USBDCDTerm(0);
    CpuTimer0Regs.TCR.all = 0x0000; // Use write-only instruction to set TSS bit = 0
    CpuTimer1Regs.TCR.all = 0x0000; // Use write-only instruction to set TSS bit = 0
    EALLOW;
    DINT;
    IER = 0;
    IFR = 0;
    PieCtrlRegs.PIEIER1.bit.INTx4 = 0;      // Enable PIE Group 1 INT4  XINT
    PieCtrlRegs.PIEIER1.bit.INTx7 = 0;      // Enable PIE Group 1 INT7  TINT0
    DELAY_US(5L);
    PieCtrlRegs.PIEIER3.bit.INTx1 = 0;      // Enable PIE Group 3 INT1  PWM1
    PieCtrlRegs.PIEIER3.bit.INTx2 = 0;      // Enable PIE Group 3 INT2  PWM2
    PieCtrlRegs.PIEIER3.bit.INTx4 = 0;      // Enable PIE Group 3 INT4  PWM4
    PieCtrlRegs.PIEIER3.bit.INTx6 = 0;      // Enable PIE Group 3 INT4  PWM6
    DELAY_US(5L);
    PieCtrlRegs.PIEIER8.bit.INTx1 = 0;      // Enable PIE Group 8 INT1  I2CA NONE FIFO INT
    DELAY_US(5L);
    IER = 0;
    IFR = 0;
    PieCtrlRegs.PIEACK.all = 0x0fff;
    Usb0Regs.USBPOWER.bit.SOFT_CONN = 0;
    *(Uint32*)pAppSig = FW_UPDATE;
    GpioCtrlRegs.GPACTRL2.bit.USB0IOEN = 0;
    SysCtrlRegs.PCLKCR3.bit.USB0ENCLK = 0;
    DisablePeripherals();
    EDIS;

    if((unsigned long)pBootEntry != 0xFFFFFFFF && (unsigned long)pBootEntry != 0x00000000)
        ((void (*)(void))pBootEntry)();
#endif
}

/* ****************************************************************************
 * Description:	directly translate one USB frame to C29 format. if corresponding C29
 * 				payload is longer than 8, the message automatically fill in C29 chain
 * 				info.
 * pC29TxObj: pointer to C29TxObj which present the CAN29 message.
 * pFrame: 	  pointer to one usb frame.
 * ****************************************************************************/
void
UsbMsg2Can29(C29TxObj *pC29TxObj, uint8_t *pFrame)
{
	uint8_t i;
	((C29TxObj*)pC29TxObj)->Len = GetByte(pFrame, USBIDX_LEN) > 8 ? GetByte(pFrame, USBIDX_LEN) + 4 : GetByte(pFrame, USBIDX_LEN); // +4 bytes: 0x00 CN 0x00 NumOfSeg
	((C29TxObj*)pC29TxObj)->pBuf = malloc(((C29TxObj*)pC29TxObj)->Len);
	((C29TxObj*)pC29TxObj)->pData = ((C29TxObj*)pC29TxObj)->pBuf;
	((C29TxObj*)pC29TxObj)->ID.field.CmdNum = GetByte(pFrame, USBIDX_CMDNUM);
	((C29TxObj*)pC29TxObj)->ID.field.CmdClass = GetByte(pFrame, USBIDX_CMDCL);
	((C29TxObj*)pC29TxObj)->ID.field.Dest = GetByte(pFrame, USBIDX_TARGET);
	((C29TxObj*)pC29TxObj)->ID.field.Src = GetByte(pFrame, USBIDX_SRC);
	((C29TxObj*)pC29TxObj)->MBoxIndex = 28;
	((C29TxObj*)pC29TxObj)->Status = CANTX_WAITING;
	if (GetByte(pFrame, USBIDX_LEN) <= 8) {
		for (i=0; i<GetByte(pFrame, USBIDX_LEN); i++)
			*(((C29TxObj*)pC29TxObj)->pBuf+i) = GetByte(pFrame, USBIDX_PID +i);
	}else {// length > 8
		Uint8 length = GetByte(pFrame, USBIDX_LEN) + 4;
		
		*(((C29TxObj*)pC29TxObj)->pBuf+0) = 0x00;
		*(((C29TxObj*)pC29TxObj)->pBuf+1) = GetByte(pFrame, USBIDX_CMDNUM);
		*(((C29TxObj*)pC29TxObj)->pBuf+2) = 0x00;
		if (length % 8)
			*((pC29TxObj)->pBuf+3) = (length >> 3) + 1;
		else
			*((pC29TxObj)->pBuf+3) = length >> 3;
		for (i=0; i < GetByte(pFrame, USBIDX_LEN) ; i++)
			*(((C29TxObj*)pC29TxObj)->pBuf+i+4) = GetByte(pFrame, USBIDX_PID +i);
	}
}

void
SendFrameACK(uint8_t *pFrame)
{
	UsbTxTask UsbTxTask;
	uint8_t usbtxmsg[USBFRAME_MAXLEN];
	uint8_t i;

	UsbTxTask.Len = GetByte(pFrame, USBIDX_LEN) + 9;
	UsbTxTask.pBuf = usbtxmsg;
	for (i=0; i< UsbTxTask.Len; i++) {
		usbtxmsg[i] = *(pFrame +i);
	}
	GetByte(usbtxmsg, USBIDX_TARGET) = GetByte(pFrame, USBIDX_SRC);
	GetByte(usbtxmsg, USBIDX_SRC) = GetByte(pFrame, USBIDX_TARGET);
	GetByte(usbtxmsg, USBIDX_CMDCL) = GetByte(pFrame, USBIDX_CMDCL) & 0x0F;
	StartUsbTxTask(&UsbTxTask);
}

void
AnswerBios_MCB(uint8_t *pFrame)
{
	UsbTxTask usbtx;
	uint8_t i, usbtxmsg[USBFRAME_MAXLEN];
	uint8_t string_len;

	usbtxmsg[0] = 0x10;
	usbtxmsg[1] = 0x02;
	GetByte(usbtxmsg, USBIDX_TARGET) = GetByte(pFrame, USBIDX_SRC);
	GetByte(usbtxmsg, USBIDX_SRC) = CANNODE_MCB;
	GetByte(usbtxmsg, USBIDX_CMDCL) = 0x08;
	GetByte(usbtxmsg, USBIDX_CMDNUM) = GetByte(pFrame, USBIDX_CMDNUM);
	GetByte(usbtxmsg, USBIDX_PID) = GetByte(pFrame, USBIDX_PID);
	GetByte(usbtxmsg, USBIDX_SUBID) = GetByte(pFrame, USBIDX_SUBID);
//	GetByte(usbtxmsg, USBIDX_DEVID) = GetByte(pFrame, USBIDX_DEVID);  // leilc: the BIOS commands does not have deviceID
	// leilc: what's this?
	if (GetByte(usbtxmsg, USBIDX_SUBID) == FIND_CAN_ADDR) {
		GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD) = CANNODE_MCB;
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
		case 0x06:	// bios date
        case 0x08:	// app date
            string_len = strlen(AppDate);
            for (i=0; i<string_len; i++) {
                GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD + i) = *(AppDate+i);
            }
            break;
		case 0x0a:	// bios name
        case 0x09:	// app name
            if (g_Device.Serie == Axiolab) {
                string_len = strlen(AppName_Axiolab);
                for (i=0; i<string_len; i++) {
                    GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD + i) = *(AppName_Axiolab +i);
                }
                break;
            }else if (g_Device.Serie >= Axioscope) {
                string_len = strlen(AppName_Axioscope);
                for (i=0; i<string_len; i++) {
                    GetByte(usbtxmsg, USBIDX_BIOS_PAYLOAD + i) = *(AppName_Axioscope +i);
                }
                break;
            }
        default :
            // need return ERR message here later than simple return
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

/* ****************************************************************************
 * Description: makeup anwer for arbitary commands with data
 * pFrame:  device command frame
 * usbtx:	UsbTxTask struct to be sent
 * Len:		data payload length 
 * pData:	pointer to answer data
 * ****************************************************************************/
void
MakeupAnswerBios(uint8_t *pFrame, UsbTxTask *usbtx, uint8_t Len, void *pData)
{
	int8_t i;

	usbtx->Len = Len + CAN29_BIOS_OVERHEAD + USB_PKT_OVERHEAD;
	for (i = Len - 1; i >= 0; i--)
		GetByte(usbtx->pBuf, USBIDX_BIOS_PAYLOAD + i) = *((uint8_t *)pData + i);		// the DataFormat is the first byte of data
	*(usbtx->pBuf +0) = 0x10;
	*(usbtx->pBuf +1) = 0x02;
	GetByte(usbtx->pBuf, USBIDX_TARGET) = GetByte(pFrame, USBIDX_SRC);
	GetByte(usbtx->pBuf, USBIDX_SRC) = GetByte(pFrame, USBIDX_TARGET);
	GetByte(usbtx->pBuf, USBIDX_LEN) = Len + CAN29_BIOS_OVERHEAD;
	// actually this is wrong. for 0x15 it need rework
	GetByte(usbtx->pBuf, USBIDX_CMDCL) = GetByte(pFrame, USBIDX_CMDCL) == 0x18 ? 0x08 : 0x09;
	GetByte(usbtx->pBuf, USBIDX_CMDNUM) = GetByte(pFrame, USBIDX_CMDNUM);
	GetByte(usbtx->pBuf, USBIDX_PID) = GetByte(pFrame, USBIDX_PID);
	GetByte(usbtx->pBuf, USBIDX_SUBID) = GetByte(pFrame, USBIDX_SUBID);
	GetByte(usbtx->pBuf, USBIDX_BIOS_PAYLOAD + Len) = 0x10;
	GetByte(usbtx->pBuf, USBIDX_BIOS_PAYLOAD + Len + 1) = 0x03;
}


/* ****************************************************************************
 * Description: makeup anwer for Descriptor commands with data
 * pFrame:  device command frame
 * usbtx:	UsbTxTask struct to be sent
 * Len:		data payload length WITH DataFormat and Data
 * pData:	pointer to answer data
 * ****************************************************************************/
void
MakeupAnswerDesc(uint8_t *pFrame, UsbTxTask *usbtx, uint8_t Len, void *pData, uint8_t format)
{
	int8_t i;
	int8_t dataLen;
	
	// recalculate the data len for text entries to remove the non-ascii tail
	if (format == DESCFMT_TEXT) {
	    for (i=0; i<MAX_TEXT_LEN; i++) {
			if (isascii(*((uint8_t *)pData + i)) == 0 || (*((uint8_t *)pData + i)==0))
				break;
	    }
	    dataLen = i;
	} else
		dataLen = Len;
	usbtx->Len = dataLen + CAN29_DESC_OVERHEAD + USB_PKT_OVERHEAD;
	for (i = dataLen - 1; i >= 0; i--)
		GetByte(usbtx->pBuf, USBIDX_DESC_DATA + i) = *((uint8_t *)pData + i);		// the DataFormat is the first byte of data
	*(usbtx->pBuf +0) = 0x10;
	*(usbtx->pBuf +1) = 0x02;
	GetByte(usbtx->pBuf, USBIDX_TARGET) = GetByte(pFrame, USBIDX_SRC);
	GetByte(usbtx->pBuf, USBIDX_SRC) = GetByte(pFrame, USBIDX_TARGET);
	GetByte(usbtx->pBuf, USBIDX_LEN) = dataLen + CAN29_DESC_OVERHEAD;
	// actually this is wrong. for 0x15 it need rework
	GetByte(usbtx->pBuf, USBIDX_CMDCL) = GetByte(pFrame, USBIDX_CMDCL) == 0x18 ? 0x08 : 0x09;
	GetByte(usbtx->pBuf, USBIDX_CMDNUM) = GetByte(pFrame, USBIDX_CMDNUM);
	GetByte(usbtx->pBuf, USBIDX_PID) = GetByte(pFrame, USBIDX_PID);
	GetByte(usbtx->pBuf, USBIDX_SUBID) = GetByte(pFrame, USBIDX_SUBID);
	GetByte(usbtx->pBuf, USBIDX_DESC_ID0) = GetByte(pFrame, USBIDX_DESC_ID0);
	GetByte(usbtx->pBuf, USBIDX_DESC_ID1) = GetByte(pFrame, USBIDX_DESC_ID1);
	GetByte(usbtx->pBuf, USBIDX_DESC_ENTRY) = GetByte(pFrame, USBIDX_DESC_ENTRY);
	GetByte(usbtx->pBuf, USBIDX_DESC_FORMAT) = format;
	GetByte(usbtx->pBuf, USBIDX_DESC_DATA + dataLen) = 0x10;
	GetByte(usbtx->pBuf, USBIDX_DESC_DATA + dataLen + 1) = 0x03;
}

void
AnswerWithData(uint8_t *pFrame, uint8_t Len, uint8_t *pData, uint8_t format)
{
	UsbTxTask usbtx;
	uint8_t usbtxmsg[BULK_BUFFER_SIZE];

	usbtx.pBuf = usbtxmsg;
	MakeupAnswerDesc(pFrame, &usbtx, Len, pData, format);
	StartUsbTxTask(&usbtx);
}

static void
BroadCastFindDev(uint8_t *pFrame)
{
	C29ID Id;
	uint8_t payload[4];

	Id.all = 0x18ff19A0;
	payload[0] = 0x00;
	payload[1] = 0xFE;
	payload[2] = GetByte(pFrame, USBIDX_DEVID);

	StartECanTxTask(Id, payload, 3, 29);
}

static void
SystemHandler(uint8_t *pFrame)
{
	switch (GetByte(pFrame, USBIDX_SUBID)) {
	case 0x10:	// monitor state
		break;
	default : break;
	}
}

//10 02 target src lengh cl cn pid subid devid [...data...] 10 03
//10 02 target src lengh cl cn pid subid descid1 descid2 entry format [...data...] 10 03

uint8_t
UsbFrameProcess(UsbRingBuf *pRing)
{
	C29ID Id;
	uint8_t txBuffer [MAX_LEN_USB2CAN_ROUTER] = {0};	// risk here if very long message
	uint8_t i;
	uint8_t *pFrame;
	int16 ulLoop = pRing->WriteIndex;
    uint8_t buffer[BULK_BUFFER_SIZE] = {0};

    while(ulLoop--) {
        buffer[i] = pRing->Buffer[pRing->ReadIndex++];
        if (buffer[i] == 0x10) {
            if(pRing->Buffer[pRing->ReadIndex] == 0x10) { // 0x10 0x10
                pRing->ReadIndex++;
                ulLoop--;
            }else if (pRing->Buffer[pRing->ReadIndex] == 0x0D) { // 0x10 0x0d
                buffer[i] = 0x0D;
                pRing->ReadIndex++;
                ulLoop--;
            }else if (pRing->Buffer[pRing->ReadIndex] != 0x02 && pRing->Buffer[pRing->ReadIndex] != 0x03) {
                return pRing->WriteIndex;   //msg has error, directly return
            }
        }
        i = (++i == BULK_BUFFER_SIZE) ? 0 : i; // prevent overflow
    }
    pFrame = &buffer[0];

	if (GetByte(pFrame, USBIDX_CMDNUM) == CN_DESC) {		       		// descriptor
		if (GetByte(pFrame, USBIDX_TARGET) != CANNODE_MCB) {  			// X,Y,Z, MotorNP, descriptor
			Id.field.CmdClass = GetByte(pFrame, USBIDX_CMDCL);
			Id.field.CmdNum = GetByte(pFrame, USBIDX_CMDNUM);
			Id.field.Dest = GetByte(pFrame, USBIDX_TARGET);
			Id.field.Src = GetByte(pFrame, USBIDX_SRC);
			for (i=0; i< GetByte(pFrame, USBIDX_LEN); i++) {
				txBuffer[i] = GetByte(pFrame, USBIDX_PID+i);
			}
			StartECanTxTask(Id, txBuffer, i, 28);							// convey descriptor to X,Y,Z axis
		}else {
			UsbDescCmdHandler(pFrame);
		}
	} else {
		switch (GetByte(pFrame, USBIDX_CMDNUM)) {
        case CN_MESSAGE:												// intended fall through
        case CN_SYSTEM:
            switch (GetByte(pFrame, USBIDX_TARGET)) {
            case CANNODE_MCB:
                AnswerBios_MCB(pFrame);
                break;
            case CANNODE_REFLECTOR:
                AnswerBios_Reflector(pFrame);
                break;
            case CANNODE_NOSEPIECE:
                AnswerBios_Nosepiece(pFrame);
                break;
            case CANNODE_RLTL:
                AnswerBios_RLTL(pFrame);
                break;
            case CANNODE_LIGHTCTRL:
                AnswerBios_Lightctrl(pFrame);
                break;
            case CANNODE_ALL:
                AnswerBios_MCB(pFrame);
                AnswerBios_Reflector(pFrame);
                AnswerBios_Nosepiece(pFrame);
                AnswerBios_RLTL(pFrame);
                AnswerBios_Lightctrl(pFrame);
                // intended fall through, no break
            default:
                Id.field.CmdClass = GetByte(pFrame, USBIDX_CMDCL);
                Id.field.CmdNum = GetByte(pFrame, USBIDX_CMDNUM);
                Id.field.Dest = GetByte(pFrame, USBIDX_TARGET);
                Id.field.Src = GetByte(pFrame, USBIDX_SRC);

                for (i=0; i< GetByte(pFrame, USBIDX_LEN); i++)
                    txBuffer[i] = GetByte(pFrame, USBIDX_PID+i);

                StartECanTxTask(Id, txBuffer, i, 28);// convey direct command
                break;
            }
            break;
			
        case CN_CHANGER:
            switch (GetByte(pFrame, USBIDX_TARGET)) {
            case CANNODE_REFLECTOR:
                ChangerHandler_Reflector(pFrame);
                break;
            case CANNODE_NOSEPIECE:
                ChangerHandler_Nosepiece(pFrame);
                break;
            case CANNODE_RLTL:
                ChangerHandler_RLTL(pFrame);
                break;
            case CANNODE_ALL:
                ChangerHandler_Reflector(pFrame);
                ChangerHandler_Nosepiece(pFrame);
                ChangerHandler_RLTL(pFrame);
                // intended fall through
            default:
                Id.field.CmdClass = GetByte(pFrame, USBIDX_CMDCL);
                Id.field.CmdNum = GetByte(pFrame, USBIDX_CMDNUM);
                Id.field.Dest = GetByte(pFrame, USBIDX_TARGET);
                Id.field.Src = GetByte(pFrame, USBIDX_SRC);
                for (i=0; i< GetByte(pFrame, USBIDX_LEN); i++)
                    txBuffer[i] = GetByte(pFrame, USBIDX_PID+i);
                StartECanTxTask(Id, txBuffer, i, 28);// convey direct command
                break;
            }
            break;
			
        case CN_SERVO:
            if (GetByte(pFrame, USBIDX_TARGET) == CANNODE_LIGHTCTRL) {
                servoHandler_Lightctrl(pFrame);
			} else {	// currently we have the focus-Z knob encoder (CanAddr: 0x25)
				Id.field.CmdClass = GetByte(pFrame, USBIDX_CMDCL);
	            Id.field.CmdNum = GetByte(pFrame, USBIDX_CMDNUM);
	            Id.field.Dest = GetByte(pFrame, USBIDX_TARGET);
	            Id.field.Src = GetByte(pFrame, USBIDX_SRC);
	            for (i=0; i< GetByte(pFrame, USBIDX_LEN); i++)
                txBuffer[i] = GetByte(pFrame, USBIDX_PID+i);
            	StartECanTxTask(Id, txBuffer, i, 28);// convey direct command
			}				
            break;
			
        case CN_AXIS:
            // since MCB has no axis small nodes, forward to CAN bus
            Id.field.CmdClass = GetByte(pFrame, USBIDX_CMDCL);
            Id.field.CmdNum = GetByte(pFrame, USBIDX_CMDNUM);
            Id.field.Dest = GetByte(pFrame, USBIDX_TARGET);
            Id.field.Src = GetByte(pFrame, USBIDX_SRC);
            for (i=0; i< GetByte(pFrame, USBIDX_LEN); i++)
                txBuffer[i] = GetByte(pFrame, USBIDX_PID+i);
            StartECanTxTask(Id, txBuffer, i, 28);// convey direct command
            break;
			
        case CN_GEMINI:
            if (GetByte(pFrame, USBIDX_TARGET) == CANNODE_MCB)
                SystemHandler(pFrame);
            break;
			
        case CN_DLD:
			if (GetByte(pFrame, USBIDX_TARGET) == CANNODE_MCB)
            	DLD_Handler(pFrame);
			else {
				Id.field.CmdClass = GetByte(pFrame, USBIDX_CMDCL);
	            Id.field.CmdNum = GetByte(pFrame, USBIDX_CMDNUM);
	            Id.field.Dest = GetByte(pFrame, USBIDX_TARGET);
	            Id.field.Src = GetByte(pFrame, USBIDX_SRC);
	            for (i=0; i< GetByte(pFrame, USBIDX_LEN); i++)
                	txBuffer[i] = GetByte(pFrame, USBIDX_PID+i);
            	StartECanTxTask(Id, txBuffer, i, 28);// convey direct command
			}
            break;
			
        default : 
			break;
		}
	} 
	return GetByte(pFrame, USBIDX_LEN) + 9;
}

TASKSTATUS
UsbTxTaskMgr(UsbTxTask *pusbobj)
{
    uint32_t ulSpace, ulWriteIndex;
    tUSBRingBufObject sTxRing;
    uint8_t i;

    USBBufferInfoGet(&g_sTxBuffer, &sTxRing);
    ulSpace = USBBufferSpaceAvailable(&g_sTxBuffer);
    if (ulSpace < pusbobj->Len)
    	return TASKSTS_ONGOING;
    ulWriteIndex = sTxRing.ui32WriteIndex;
    for (i=0; i<pusbobj->Len; i++) {
    	g_pui8USBTxBuffer[ulWriteIndex++] = *(pusbobj->pBuf + i);
    	ulWriteIndex = (ulWriteIndex == BULK_BUFFER_SIZE) ? 0 : ulWriteIndex;
    }
    USBBufferDataWritten(&g_sTxBuffer, pusbobj->Len);
    free(pusbobj->pBuf);
    pusbobj->MemProd--;
    ASSERT(pusbobj->MemProd == 1);
    free(pusbobj);
    return TASKSTS_FINISH;
}

/* *****************************************************************************
 * Description: handle DLE for original frame, and send the new frame via usb
 * pData: original frame
 * *****************************************************************************/
void
StartUsbTxTask(UsbTxTask *pData)
{
	Task *pNewTask;
	UsbTxTask *pusbobj;
	uint8_t WriteIndex, ReadIndex;
	uint8_t usbtxmsg[BULK_BUFFER_SIZE];

	pNewTask = CreateTask(NORMAL);
	pNewTask->Name = USBTRANSMIT;
	pNewTask->Status = TASKSTS_ONGOING;
	pNewTask->pfn = (TASKSTATUS (*)(void *))UsbTxTaskMgr;
	if (*pData->pBuf != 0x10 || *(pData->pBuf +1) != 0x02)
		return;
	pusbobj = malloc(sizeof(UsbTxTask));
	pusbobj->MemProd = 1;
	pNewTask->pTaskObj = pusbobj;

	WriteIndex = 0;
	for (ReadIndex=0; ReadIndex < pData->Len; ReadIndex++) {
		usbtxmsg[WriteIndex] = *(pData->pBuf +ReadIndex);
		//if (usbtxmsg[WriteIndex] == 0x10 &&
				//(*(pData->pBuf+ReadIndex +1) != 0x02 && *(pData->pBuf+ReadIndex +1) != 0x03) ) {
		if (usbtxmsg[WriteIndex] == 0x10 && ReadIndex != 0 && ReadIndex != pData->Len-2) {
			usbtxmsg[++WriteIndex] = 0x10;
		}
		if (usbtxmsg[WriteIndex] == 0x0D) {
			usbtxmsg[WriteIndex] = 0x10;
			usbtxmsg[++WriteIndex] = 0x0D;
		}
		WriteIndex++;
	}
	pusbobj->pBuf = malloc(WriteIndex);
	pusbobj->MemProd++;
	pusbobj->Len = WriteIndex;
	for (ReadIndex =0; ReadIndex <WriteIndex; ReadIndex++) {
		*(pusbobj->pBuf +ReadIndex) = usbtxmsg[ReadIndex];
	}
}

void
AppCheck(void)
{
#if FW_UPDATE
    if (*(uint32_t *)pAppSig != FWBL)
        return;
    *(uint32_t *)pAppSig = FWAPP;
    uint8_t buffer[] = {0x10, 0x02, 0x10, 0x10, 0x19, 04, 0x09, 0x0e, 0x10, 0x10, 0x25, 0x00, 0x00, 0x10, 0x03};
    uint8_t len = 15;
    uint32_t ulWriteIndex;
    tUSBRingBufObject sTxRing;
    uint8_t i;
    USBBufferInfoGet(&g_sTxBuffer, &sTxRing);
    ulWriteIndex = sTxRing.ui32WriteIndex;
    for (i = 0; i < len; i++) {
       g_pui8USBTxBuffer[ulWriteIndex++] = *(buffer + i);
       ulWriteIndex = (ulWriteIndex == BULK_BUFFER_SIZE) ? 0 : ulWriteIndex;
    }
    USBBufferDataWritten(&g_sTxBuffer, len);
#endif
}



