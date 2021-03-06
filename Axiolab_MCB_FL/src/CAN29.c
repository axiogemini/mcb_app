/*
 * CAN29.c
 *
 *  Created on: 2016��6��30��
 *      Author: zctiaxia
 */

#include "include.h"

Uint32 g_MailBoxBusy = 0;

const uint8_t MCB2Axis[30][2] = {
//		MCB				Axis
		0x00	,		0x05	,   // stop
		0x01	,		0x02	,	// move absolute unlock
		0x11	,		0x12	,	// move absolute lock
		0x02	,		0x03	,	// move relative unlock
		0x12	,		0x13	,	// move relative lock
		0x03	,		0x04	,	// move velocity unlock
		0x13	,		0x14	, 	// move velocity lock
		0x04	,		0x31	,	// get target postion
		0x05	,		0xFF	,	// get total position				    MCB function
		//0x06	,					// get accuracy
		0x07	,		0x01	,	// get status                           MCB function + Convey
		0x08	,		0xFF	,	// get device scaling					MCB function
		0x09	,		0xFF	,	// get scaling table					MCB function
		0x20	,		0x18	,	// find/read hw stop, 1st send upper
		0x21	,		0x19	,	// find/read hw stop, 2nd send lower
		0x22	,		0x08	,	// set/get upper sw limit
		0x23	,		0x09	,	// set/get lower sw limit
		0x24	,		0xFF	,	// set/get origin						MCB function
		0x25	,		0xFF	,	// get measurement distance				MCB function
		//0x26	,					// set working state
		//0x27	,					// get working state
		0x2B	,		0x21	,	// set/get trajectory velocity
		0x2C	,		0x22	,	// set/get trajectory acceleration
		0x60	,		0x90	,	// set/get trigger mode
		0x61	,		0x91	,	// get one from trigger position list
		0x68	,		0x98	,	// get whole trigger position list
		0x63	,		0x93	,	// get trigger positon list count
		0x64	,		0x94	,	// get trigger position list size
		0x62	,		0x92	,	// clear trigger position list
		0xA1	,		0x52	,	// wheel state
};


/* ********************************************************************************
 * Search one column to find element that match value. Return the position in talbe
 * that meet match value.
 *
 * pTable: 	pointer to the table for searching in
 * Value:  	value for search to match
 * Dim:		dimension of the table
 * Depth:	depth of the table
 * ********************************************************************************/
uint8_t *
SearchTable(void *pTable, uint8_t Value, uint8_t Dim, uint8_t Depth)
{
	uint8_t *ptr;
	uint8_t *address_range;

	address_range = (uint8_t *)pTable + Dim*Depth;
	for (ptr = pTable; ptr<= address_range ; ptr += Dim) {
		if (*ptr == Value)
			return ptr;
	}
	return NULL;
}

/* ********************************************************************
 * Directly translate CAN29 message to usb frame, remove CAN29 chain
 * message info. DLE is not processed.
 *
 * pc29obj: pointer to CAN29 message struct
 * pFrame:  pointer to usb frame
 * ********************************************************************/
void
C29Msg2Usb(C29RxObj *pc29obj, uint8_t *pFrame)
{
	uint8_t i;

	*(pFrame + 0) = 0x10;
	*(pFrame + 1) = 0x02;
	GetByte(pFrame, USBIDX_TARGET) = pc29obj->ID.field.Dest;
	GetByte(pFrame, USBIDX_SRC) = pc29obj->ID.field.Src;
	GetByte(pFrame, USBIDX_LEN) = pc29obj->Len;
	GetByte(pFrame, USBIDX_CMDCL) = pc29obj->ID.field.CmdClass;
	GetByte(pFrame, USBIDX_CMDNUM) = pc29obj->ID.field.CmdNum;

	for (i=0; i < pc29obj->Len; i++) {
		GetByte(pFrame, USBIDX_PID + i) = *(pc29obj->pBuf + i);
	}
	GetByte(pFrame, USBIDX_PID + i++) = 0x10;
	GetByte(pFrame, USBIDX_PID + i++) = 0x03;
}

TASKSTATUS
ECAN_TxTaskMgr(C29TxObj *pc29obj)
{
	if (pc29obj->Status == CANTX_WAITING) {
		if (MailBoxIsBusy(pc29obj->MBoxIndex))  // waiting for takeover this mailbox
			return TASKSTS_ONGOING;
		SetMailBoxBusy(pc29obj->MBoxIndex);  // get this mailbox
		pc29obj->Status = CANTX_SENDING;
		if (pc29obj->Len <= 8) {
			ECAN_SendMail(pc29obj->MBoxIndex, pc29obj->ID, pc29obj->pData, pc29obj->Len);
			pc29obj->pData += pc29obj->Len;
			pc29obj->Len = 0;
		}else {// length > 8
			C29ID id;
			id.field.CmdClass = pc29obj->ID.field.CmdClass;
			id.field.Dest = pc29obj->ID.field.Dest;
			id.field.Src = pc29obj->ID.field.Src;
			id.field.CmdNum = 0x00;
			ECAN_SendMail(pc29obj->MBoxIndex, id, pc29obj->pData, 8);
			pc29obj->pData += 8;
			pc29obj->Len -= 8;
		}
		return TASKSTS_ONGOING;
	}else if (pc29obj->Status == CANTX_SENDING) {  // already get control of the mailbox
		uint32_t shadow = (Uint32)1 << pc29obj->MBoxIndex;
		if ((ECanaRegs.CANTA.all & shadow) == 0)
			return TASKSTS_ONGOING;
		ECanaRegs.CANTA.all = shadow;  // clear corresponding TAn bit
		if (pc29obj->Len == 0) {
			pc29obj->Status = CANTX_FINISH;
		}else {
			if (pc29obj->Len <= 8) {
				ECAN_SendMail(pc29obj->MBoxIndex, pc29obj->ID, pc29obj->pData, pc29obj->Len);
				pc29obj->pData += pc29obj->Len;
				pc29obj->Len = 0;
			}else if (pc29obj->Len > 8) {
				ECAN_SendMail(pc29obj->MBoxIndex, pc29obj->ID, pc29obj->pData, 8);
				pc29obj->pData += 8;
				pc29obj->Len -= 8;
			}
		}
		return TASKSTS_ONGOING;
	}else if (pc29obj->Status == CANTX_FINISH) {
		ClearMailBoxBusy(pc29obj->MBoxIndex);
		free(pc29obj->pBuf);
		free(pc29obj);
		return TASKSTS_FINISH;
	}
	return TASKSTS_WRONG;
}


/* ********************************************************************************
 * Len : Payload length
 * ********************************************************************************/
void
StartECanTxTask(C29ID Id, uint8_t *Payload, uint8_t Len, uint8_t MBoxIndex)
{
	uint8_t i;
	C29TxObj *c29tx;
	c29tx = malloc(sizeof(C29TxObj));
	Task *pnewtask;

	c29tx->ID.all = Id.all;
	c29tx->Len = Len > 8 ? Len + 4 : Len; // +4 bytes: 0x00 CN 0x00 NumOfSeg
	c29tx->pBuf = malloc(c29tx->Len);
	c29tx->pData = c29tx->pBuf;
	c29tx->MBoxIndex = MBoxIndex;
	c29tx->Status = CANTX_WAITING;
	if (Len <= 8) {
		for (i=0; i < Len; i++)
			*(c29tx->pBuf+i) = *(Payload + i);
	}else {// length > 8
		*(c29tx->pBuf+0) = 0x00;
		*(c29tx->pBuf+1) = Id.field.CmdNum;
		*(c29tx->pBuf+2) = 0x00;
		if ((Len+4) % 8)
			*(c29tx->pBuf+3) = (Len >> 3) + 1;
		else
			*(c29tx->pBuf+3) = (Len >> 3);
		for (i=0; i < Len + 4 ; i++)
			*(c29tx->pBuf+i+4) = *(Payload + i);
	}

	pnewtask = CreateTask(NORMAL);
	pnewtask->Name = CANTX;
	pnewtask->Status = TASKSTS_ONGOING;
	pnewtask->pfn = (TASKSTATUS (*)(void *))ECAN_TxTaskMgr;
	pnewtask->pTaskObj = c29tx;
}

TASKSTATUS
ECAN_RxTaskMgr(C29RxObj *canrxobj)
{
	volatile struct MBOX *pMBox;
	pMBox = &ECanaMboxes.MBOX0 + canrxobj->MBoxIndex;

	if (GetSysTick0() != canrxobj->SysTick) {
		canrxobj->TimeoutCount++;
		canrxobj->SysTick = GetSysTick0();
		if (canrxobj->TimeoutCount >= 100) {
			ClearMailBoxBusy(canrxobj->MBoxIndex);
			ASSERT(canrxobj->MemProd == 1);
			free(canrxobj);
			return TASKSTS_TIMEOUT;
		}
	}
	if (canrxobj->puCount->Value == 244) { // task terminated
		ClearMailBoxBusy(canrxobj->MBoxIndex);
		FreeSem(canrxobj->puCount->Index); //need freesem here but not application because sem is used to term the mail rx task
		ASSERT(canrxobj->MemProd == 1);
		free(canrxobj);
		return TASKSTS_WRONG;
	}
	if (canrxobj->pBuf == NULL) { //buffer is not assigned
		ClearMailBoxBusy(canrxobj->MBoxIndex);
		ASSERT(canrxobj->MemProd == 1);
		free(canrxobj);
		return TASKSTS_WRONG;
	}

	if (!CheckMailBoxRMP(canrxobj->MBoxIndex)) //wait for mail
		return TASKSTS_ONGOING;

	*(canrxobj->pData +0) = pMBox->MDL.byte.BYTE0;
	*(canrxobj->pData +1) = pMBox->MDL.byte.BYTE1;
	*(canrxobj->pData +2) = pMBox->MDL.byte.BYTE2;
	*(canrxobj->pData +3) = pMBox->MDL.byte.BYTE3;
	*(canrxobj->pData +4) = pMBox->MDH.byte.BYTE4;
	*(canrxobj->pData +5) = pMBox->MDH.byte.BYTE5;
	*(canrxobj->pData +6) = pMBox->MDH.byte.BYTE6;
	*(canrxobj->pData +7) = pMBox->MDH.byte.BYTE7;
	canrxobj->pData += pMBox->MSGCTRL.bit.DLC;
	canrxobj->NumOfSegs--;
	ClearMailBoxRMP(canrxobj->MBoxIndex);
	if (canrxobj->NumOfSegs == 0) {
		if (canrxobj->puCount)
			canrxobj->puCount->Value = canrxobj->pData - canrxobj->pBuf;
		ClearMailBoxBusy(canrxobj->MBoxIndex);
		ECanaRegs.CANME.all &= ~((Uint32)1 << canrxobj->MBoxIndex); //~(1 << mboxindex); default 16 bit, shift overflow
		pMBox->MSGID.all = 0x80000000; //ide:1, ame:0, aam:0
		ECanaRegs.CANME.all = CAN_MAILBOX_EN_MASK;
		ASSERT(canrxobj->MemProd == 1);
		free(canrxobj);
		return TASKSTS_FINISH;
	}
	return TASKSTS_ONGOING;
}

/* ********************************************************************************
 * Description:	Start recevie CAN msg. upper layer assign buffer space
 * MsgId:		CAN ID
 * MBoxIndex:	mailbox index
 * buffer:		buffer to store data, space is assigned by upper layer, should be
 * 				8xN bytes to prevent overflow.
 * Len:			data length for payload, not include CAN ID
 * *puCount:	when receive is finished, fill actual payload data length to *puCount
 * ********************************************************************************/
void
StartECANRxTask(Uint32 MsgId, uint8_t MBoxIndex, uint8_t *buffer, uint8_t NumOfSegs, tSemiphore *puCount)
{
	C29RxObj *canrxobj;
	Task *pnewtask;

	canrxobj = malloc(sizeof(C29RxObj));
	canrxobj->MemProd = 1;
	canrxobj->ID.all = MsgId;
	canrxobj->NumOfSegs = NumOfSegs;
	canrxobj->puCount = puCount;
	canrxobj->pBuf = canrxobj->pData = buffer;
	canrxobj->MBoxIndex = MBoxIndex;
	SetMailBoxBusy(MBoxIndex);
	ECanaRegs.CANME.all &= ~((Uint32)1 << canrxobj->MBoxIndex); //~(1 << mboxindex); default 16 bit, shift overflow
	((volatile struct MBOX *)(&ECanaMboxes.MBOX0 + canrxobj->MBoxIndex))->MSGID.all = MsgId | 0x80000000; //ide:1, ame:0, aam:0
	ECanaRegs.CANME.all = CAN_MAILBOX_EN_MASK;;
	canrxobj->SysTick = GetSysTick0();
	canrxobj->TimeoutCount = 0;

	pnewtask = CreateTask(PRIOR);
	pnewtask->Name = CANRX;
	pnewtask->Status = TASKSTS_ONGOING;
	pnewtask->pfn = (TASKSTATUS (*)(void *))ECAN_RxTaskMgr;
	pnewtask->pTaskObj = canrxobj;
}

TASKSTATUS
ECAN2UsbTaskMgr(CAN2UsbTask *can2usbobj)
{
	if (can2usbobj->uCount->Value == 0)  // haven't received complete msg
		return TASKSTS_ONGOING;

	can2usbobj->Len = can2usbobj->uCount->Value +4+9;
	*(can2usbobj->pBuf + can2usbobj->Len -2) = 0x10;
	*(can2usbobj->pBuf + can2usbobj->Len -1) = 0x03;
	GetByte(can2usbobj->pBuf, USBIDX_LEN) = can2usbobj->uCount->Value +4;
	FreeSem(can2usbobj->uCount->Index);

	UsbTxTask usbtxobj;

	usbtxobj.Len = can2usbobj->Len; // 10 02 target src length cl cn payload 10 03
	usbtxobj.pBuf = can2usbobj->pBuf;
	StartUsbTxTask(&usbtxobj);
	ClearMailBoxBusy(can2usbobj->MBoxIndex);
	free(can2usbobj->pBuf);
	can2usbobj->MemProd--;
    ASSERT(can2usbobj->MemProd == 1);
    free(can2usbobj);
	return TASKSTS_FINISH;
}

/* ********************************************************************************
 * Description: convey single CAN msg or multi reply with one integrated msg via
 * 				usb
 * Id:  		CAN msg ID
 * MBoxIndex:	index number for the mailbox that receive the '1st' frame
 * Len: 		C29 msg payload length
 * ********************************************************************************/
void
StartCAN2UsbTask(C29ID Id, uint8_t MBoxIndex, Uint16 NumOfSegs)
{
	uint8_t *buffer;
	Task *pnewtask;
	CAN2UsbTask *can2usbobj;
	volatile struct MBOX *pmbox;
	uint8_t index;

	index = 0;   // check if there is sem available before desctask malloc
	while (Sem[index++].Used) {
		if ((int16)index > MAX_TASK_NUM)
			return;
	}
	// find idle mailbox, return if no available
	index = 15;
	while (MailBoxIsBusy(index)) {
		index++;
		if (index == 28) // no idle mailbox
			return;
	}

	can2usbobj = malloc(sizeof(CAN2UsbTask));
	can2usbobj->MemProd = 1;
	can2usbobj->Len = NumOfSegs * 8;

	//assign buffer
	buffer = malloc(can2usbobj->Len +9 +8); //additional 8 for anti-overflow when copy MDL,MDH to buffer
	can2usbobj->pBuf = buffer;
	can2usbobj->MemProd++;

	// pre store the 1st frame msg data to buffer
	*buffer = 0x10;
	*(buffer +1) = 0x02;
	if (Id.field.Dest == CANNODE_PC || Id.field.Dest == CANNODE_CAM) {
		GetByte(buffer, USBIDX_TARGET) = Id.field.Dest;
		GetByte(buffer, USBIDX_SRC) = Id.field.Src;
	}else if (Id.field.Dest == CANNODE_MCB) {
		GetByte(buffer, USBIDX_TARGET) = 0x10;
		GetByte(buffer, USBIDX_SRC) = 0x19;
	}
	GetByte(buffer, USBIDX_LEN) = 0; //todo need fill in when rx finish
	GetByte(buffer, USBIDX_CMDCL) = Id.field.CmdClass;
	GetByte(buffer, USBIDX_CMDNUM) = Id.field.CmdNum;
	pmbox = &ECanaMboxes.MBOX0 + MBoxIndex;
	GetByte(buffer, USBIDX_PID) = pmbox->MDH.byte.BYTE4;
	GetByte(buffer, USBIDX_SUBID) = pmbox->MDH.byte.BYTE5;
	GetByte(buffer, USBIDX_DEVID) = pmbox->MDH.byte.BYTE6;
	GetByte(buffer, USBIDX_DEVID + 1) = pmbox->MDH.byte.BYTE7;

	// use idle mailbox and set msgid
	pmbox = &ECanaMboxes.MBOX0 + index;
	ECanaRegs.CANME.all = ~((Uint32)1 << index); //~(1 << mboxindex); default 16 bit, shift overflow
	pmbox->MSGID.all = Id.all | 0x80000000;
	ECanaRegs.CANME.all = CAN_MAILBOX_EN_MASK;
	can2usbobj->MBoxIndex = index;

	can2usbobj->uCount = CreateSem();

	pnewtask = CreateTask(PRIOR);
	pnewtask->Name = ECAN2USB;
	pnewtask->Status = TASKSTS_ONGOING;
	pnewtask->pTaskObj = can2usbobj;
	pnewtask->pfn = (TASKSTATUS (*)(void *))ECAN2UsbTaskMgr;

	SetMailBoxBusy(MBoxIndex);
	StartECANRxTask(Id.all,
					index,
					buffer+7+4, 	// 10 02 target src len cl cn payload(Len=4 for 1st msg) 10 03,, buffer+7+4: begin at 5th payload
					NumOfSegs-1, 		// 1st segment (00 subid 00 numofsegs + 4 bytes payload) are already stored
					can2usbobj->uCount);
}

void
FindDevice(uint8_t Devid)
{
	C29ID c29id;
	uint8_t payload[8] = {0};
	uint8_t pid;
//	tSemiphore *psem;

//	if ((psem = CreateSem()) == NULL)
//		return;
	pid = GetPid();
	FreePid(pid);
	pid = 0x01;
//	psem->pBuf = malloc(8); //8xN to prevent overflow
//	psem->Pid = pid;

	c29id.all = 0x1800195f;
	c29id.field.Dest = Devid;
	payload[0] = GetPid();
	payload[1] = 0x01;
	payload[2] = Devid;
	StartECanTxTask(c29id, payload, 3, 30);
}

// here start can29 commands

void monitorHandwheel(_Bool on)
{
	C29ID c29id;
	uint8_t payload[8] = {0};
	uint8_t pid;
	uint8_t mode;
	static _Bool prevOn = false;

	if(prevOn == on)
		return;

//	pid = GetPid();
	pid = PID_GENERAL;
	mode = (on == true) ? 0x02 : 0x00;
	c29id.all = 0x1925195f;						// CanAddr of Z knob: 0x25
	payload[0] = pid; //pid
	payload[1] = 0x1f; //subid
	payload[2] = 0; //devid
	payload[3] = mode; //mode
	payload[4] = 0x00; //interval_h
	payload[5] = 0x64; //interval_l
	payload[6] = CANNODE_MCB; //client address
	payload[7] = 0x01; //pid
	StartECanTxTask(c29id, payload, 8, 30);

	prevOn = on;
}





