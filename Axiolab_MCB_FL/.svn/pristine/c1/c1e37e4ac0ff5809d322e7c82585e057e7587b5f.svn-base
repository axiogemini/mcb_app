/*
 * Descriptor.c
 *
 *  Created on: 2016?10?10?
 *      Author: ZCTIAXIA
 */

#include "include.h"

const Uint16 *
SearchMMap(Uint16 DescId, Uint16 DescEntry)
{
	const Uint16 (*p_arr)[3] ;
	Uint16 dep;
	switch (DescId) {
		
	case DESC_MCB:
		p_arr = MMAP_MCB;
		dep = MEMDEP_MCB;
		//p_arr = (Uint16 (*)[3])MMAP_MCB;
		break;
	case DESC_XYZ:
		p_arr = MMAP_XYZ;
		dep = MEMDEP_XYZ;
		break;
	case DESC_LIGHTCTRL:
		p_arr = MMAP_LIGHTCTRL;
		dep = MEMDEP_LIGHTCTRL;
		break;
	case DESC_RFLM:
		p_arr = MMAP_RFLM;
		dep = MEMDEP_RFLM;
		break;
	case DESC_TMLM:
		p_arr = MMAP_TMLM;
		dep = MEMDEP_TMLM;
		break;
	case DESC_USRSETTING:
		p_arr = MMAP_USRSETTING;
		dep = MEMDEP_USRSETTING;
		break;
	case DESC_ELEMENTS_NP:
		p_arr = MMAP_ELEMENTS_NP;
		dep = MEMDEP_ELEMENTS_NP;
		break;
		
	case DESC_ELEMENTS_REF1:
	case DESC_ELEMENTS_REF2:
	case DESC_ELEMENTS_REF3:
	case DESC_ELEMENTS_REF4:
	case DESC_ELEMENTS_REF5:
	case DESC_ELEMENTS_REF6:
		p_arr = MMAP_ELEMENTS_REF;
		dep = MEMDEP_ELEMENTS_REF;
		break;
		
	default:
		break;
	}

	return BinarySearch(p_arr, dep, DescEntry);
}

uint8_t
SearchLength(Uint16 Format)
{
	switch (Format) {
	case 1:
		return MAX_TEXT_LEN;
	case 2:
		return 1;
	case 3:
		return 1;
	case 4:
		return 2;
	case 5:
		return 2;
	case 6:
		return 4;
	case 7:
		return 4;
	case 16:
		return 4;
	case 17:
		return 8;
	default:
		return 0;
	}
}

TASKSTATUS
RwDescTaskMgr(DescTask *descobj)
{
	UsbTxTask usbtx;
	uint8_t usbtxmsg[USBFRAME_MAXLEN];
	uint8_t err_code[2] = {0};
	if (descobj->pSem->Value == 1) {	// leilc: eeprom has done the job
		FreeSem(descobj->pSem->Index);
		if (descobj->NeedAnswer) {
			if (GetByte(descobj->pCmdFrame, USBIDX_SUBID) == CSN_WRITE_ENTRY) {
				usbtx.pBuf = usbtxmsg;
				MakeupAnswerBios(descobj->pCmdFrame, &usbtx, 2, &err_code[0]);
				StartUsbTxTask(&usbtx);
				
			} else if (GetByte(descobj->pCmdFrame, USBIDX_SUBID) == CSN_READ_ENTRY) {
				AnswerWithData(descobj->pCmdFrame, descobj->Len, descobj->pWorkSpace, descobj->Format);
			}
		}
		free(descobj->pWorkSpace);
		descobj->MemProd--;
		ASSERT(descobj->MemProd == 1);
		free(descobj->pCmdFrame);
		descobj->MemProd--;
		free(descobj);
		return TASKSTS_FINISH;
	}else if(descobj->pSem->Value == 0)
		return TASKSTS_ONGOING;
	return TASKSTS_WRONG;
}

void
StartDescTask(DescTask *descobj)
{
	Task *pNewTask;

	pNewTask = CreateTask(NORMAL);
	pNewTask->Name = DESCRW;
	pNewTask->Status = TASKSTS_ONGOING;
	pNewTask->pfn = (TASKSTATUS (*)(void *))RwDescTaskMgr;
	pNewTask->pTaskObj = descobj;
}

void
DescInitRead(Uint16 Id, Uint16 Entry, void *pData, Uint16 NumOfItems, DESCFORMAT Format)
{
	const Uint16 (*Index)[3];
	Uint16 RegAddr, length;
	Uint16 *buffer;
	EromTask *eromobj;
	uint8_t i;

	switch (Format) {
	case DESCFMT_TEXT:
		length = NumOfItems * 20;
		buffer = malloc(length);
		break;
	case DESCFMT_UBYTE:
	case DESCFMT_BYTE:
		length = NumOfItems * 1;
		buffer = malloc(length);
		break;
	case DESCFMT_USHORT:
	case DESCFMT_SHORT:
		length = NumOfItems * 2;
		buffer = malloc(length);
		break;
	case DESCFMT_ULONG:
	case DESCFMT_LONG:
	case DESCFMT_FLOAT:
		length = NumOfItems * 4;
		buffer = malloc(length);
		break;
	case DESCFMT_DOUBLE:
		length = NumOfItems * 8;
		buffer = malloc(length);
		break;
	default : return;
	}
	Index = (const Uint16(*)[3])SearchMMap(Id, Entry);
	if (Id >= 0x1501 && Id <= 0x1506)
		RegAddr = (*Index)[1] + (Id - 0x1501)*512 + ADDR_ELEMENTS_REF;
	else
		RegAddr = (*Index)[1];
	eromobj = malloc(sizeof(EromTask));
	eromobj->MemProd = 1;
	eromobj->DevAddr = 0x50;
	eromobj->RegAddr = RegAddr;
	eromobj->DataLen = length;
	eromobj->Event = EROM_TOREAD;
	eromobj->Status = EROM_WAITING;
	eromobj->pBuf = buffer;
	eromobj->pData = eromobj->pBuf;
	eromobj->pSem = CreateSem();
	do {
		EepromTaskMgr(eromobj);
	} while (eromobj->pSem->Value == 0);
	FreeSem(eromobj->pSem->Index);

	switch (Format) {
	case DESCFMT_UBYTE:
	case DESCFMT_BYTE:
		for (i=0; i < NumOfItems; i++)
			*((uint8_t *)pData +i) = buffer[i];
		break;
	case DESCFMT_USHORT:
	case DESCFMT_SHORT:
		for (i=0; i < NumOfItems; i++)
			*((Uint16 *)pData +i) = buffer[2*i] << 8 | buffer[2*i+1];
		break;
	case DESCFMT_ULONG:
	case DESCFMT_LONG:
	case DESCFMT_FLOAT:
		for (i=0; i < NumOfItems; i++)
			*((Uint32 *)pData +i) = (Uint32)buffer[4*i] << 24 | (Uint32)buffer[4*i+1] << 16 | (Uint32)buffer[4*i+2] << 8 | (Uint32)buffer[4*i+3];
		break;
	case DESCFMT_DOUBLE:
		for (i=0; i < NumOfItems; i++)
			*((double *)pData +i) = (Uint64)buffer[8*i] << 56 | (Uint64)buffer[8*i+1] << 48 | (Uint64)buffer[8*i+2] << 40 | (Uint64)buffer[8*i+3] << 32 |
									(Uint64)buffer[8*i+4] << 24 | (Uint64)buffer[8*i+5] << 16 | (Uint64)buffer[8*i+6] << 8 | (Uint64)buffer[8*i+7];
		break;
	default : return;
	}
}

void
WriteDesc(Uint16 Id, Uint16 Entry, void *pData, Uint16 NumOfItems, DESCFORMAT Format)
{
	const Uint16 (*Index)[3];
	Uint16 RegAddr, length;
	Uint16 *buffer;
	DescTask *descobj;
	uint8_t i;

	i = 0;   // check if there is sem available before desctask malloc
	while (Sem[i++].Used) {
		if ((int16)i > MAX_TASK_NUM)
			return;
	}

	switch (Format) {
	case DESCFMT_TEXT:
		length = NumOfItems * 20;
		buffer = malloc(length);
		for (i=0; i < NumOfItems; i++)
			buffer[i] = *((uint8_t *)pData +i);
		break;
	case DESCFMT_UBYTE:
	case DESCFMT_BYTE:
		length = NumOfItems * 1;
		buffer = malloc(length);
		for (i=0; i < NumOfItems; i++)
			buffer[i] = *((uint8_t *)pData +i);
		break;
	case DESCFMT_USHORT:
	case DESCFMT_SHORT:
		length = NumOfItems * 2;
		buffer = malloc(length);
		for (i=0; i < NumOfItems; i++) {
			buffer[2*i] = *((Uint16 *)pData+i) >> 8;
			buffer[2*i+1] = *((Uint16 *)pData+i);
		}
		break;
	case DESCFMT_ULONG:
	case DESCFMT_LONG:
	case DESCFMT_FLOAT:
		length = NumOfItems * 4;
		buffer = malloc(length);
		for (i=0; i < NumOfItems; i++) {
			buffer[4*i] = (*((Uint32 *)pData+i) >> 24) & 0xff;
			buffer[4*i+1] = (*((Uint32 *)pData+i) >> 16) & 0xff;
			buffer[4*i+2] = (*((Uint32 *)pData+i) >> 8) & 0xff;
			buffer[4*i+3] = *((Uint32 *)pData+i) & 0xff;
		}
		break;
	case DESCFMT_DOUBLE:
		length = NumOfItems * 8;
		buffer = malloc(length);
		for (i=0; i < NumOfItems; i++) {
			buffer[8*i]   = (*((Uint64 *)pData+i) >> 56) & 0xff;
			buffer[8*i+1] = (*((Uint64 *)pData+i) >> 48) & 0xff;
			buffer[8*i+2] = (*((Uint64 *)pData+i) >> 40) & 0xff;
			buffer[8*i+3] = (*((Uint64 *)pData+i) >> 32) & 0xff;
			buffer[8*i+4] = (*((Uint64 *)pData+i) >> 24) & 0xff;
			buffer[8*i+5] = (*((Uint64 *)pData+i) >> 16) & 0xff;
			buffer[8*i+6] = (*((Uint64 *)pData+i) >> 8) & 0xff;
			buffer[8*i+7] = (*((Uint64 *)pData+i)) & 0xff;
		};
		break;
	default : return;
	}
	Index = (const Uint16(*)[3])SearchMMap(Id, Entry);
	if (Id >= 0x1501 && Id <= 0x1506)
		RegAddr = (*Index)[1] + (Id - 0x1501)*512 + ADDR_ELEMENTS_REF;
	else
		RegAddr = (*Index)[1];
	descobj = malloc(sizeof(DescTask));
	descobj->MemProd = 1;
	descobj->DescEntry = Entry;
	descobj->DescId = Id;
	descobj->Format = Format;
	descobj->Len = length;
	descobj->NeedAnswer = false;
	descobj->pBuf = pData;
	descobj->pCmdFrame = NULL;
	descobj->pWorkSpace = buffer;
	descobj->MemProd++;
	descobj->pSem = CreateSem();

	StartDescTask(descobj);
	StartEromTask(RegAddr, length, buffer, 0, descobj->pSem);
}

void
UsbDescCmdHandler(const Uint16 *pFrame)
{
	uint8_t i = 0;
	const Uint16 (*index)[3];
	DescTask *descobj;
	Uint16 regaddr;
	Bool Rw;
	float *pFloat;
	Uint16 dataLen;

	i = 0;   // check if there is sem available before desctask malloc
	while (Sem[i++].Used) {
		if (i > MAX_TASK_NUM)
			return;
	}

	if (GetByte(pFrame,USBIDX_SUBID) == CSN_LOCK_DESC || GetByte(pFrame,USBIDX_SUBID) == CSN_UPDATE_DESC) {
	        // dummy function, always return OK
	        SendFrameACK((uint8_t *)pFrame);
	        return;
	}

	descobj = malloc(sizeof(DescTask));
	descobj->MemProd = 1;
	descobj->pCmdFrame = malloc(GetByte(pFrame, USBIDX_LEN) + 9);
	descobj->MemProd++;
	if (GetByte(pFrame, USBIDX_CMDCL) == 0x18 || GetByte(pFrame, USBIDX_CMDCL) == 0x19)
		descobj->NeedAnswer = true;
	for (i=0; i<GetByte(pFrame, USBIDX_LEN) + 9; i++)
		*(descobj->pCmdFrame +i) = *(pFrame +i);
	descobj->DescId = GetByte(pFrame, USBIDX_DESC_ID0)<<8 | GetByte(pFrame, USBIDX_DESC_ID1);
	descobj->DescEntry = GetByte(pFrame, USBIDX_DESC_ENTRY);
	index = (const Uint16(*)[3])SearchMMap(descobj->DescId, descobj->DescEntry);

	if (descobj->DescId >= 0x1501 && descobj->DescId <= 0x1506)
		regaddr = (*index)[1] + (descobj->DescId - 0x1501)*512 + ADDR_ELEMENTS_REF;
	else
		regaddr = (*index)[1];

	descobj->Format = (*index)[2];

	if (GetByte(pFrame,USBIDX_SUBID) == CSN_READ_ENTRY || GetByte(pFrame,USBIDX_SUBID) == CSN_READ_DESC) { // read transaction
		Rw = 1;
		descobj->Len = SearchLength((*index)[2]);		// +1: space for DataFormat
		descobj->pWorkSpace = malloc(descobj->Len);
		descobj->MemProd++;
	} else if (GetByte(pFrame,USBIDX_SUBID) == CSN_WRITE_ENTRY) {  // write transaction
		Rw = 0;
		descobj->Len = GetByte(pFrame, USBIDX_LEN) - CAN29_DESC_OVERHEAD;

		// for text, we need add a '\0' after the text
		if (GetByte(pFrame,USBIDX_DESC_FORMAT) == DESCFMT_TEXT)
			descobj->pWorkSpace = malloc(descobj->Len + 1);
		else
			descobj->pWorkSpace = malloc(descobj->Len);
		
		descobj->MemProd++;
		for (i = 0; i < descobj->Len; i++)
			*(descobj->pWorkSpace + i) = *(pFrame + USBIDX_DESC_DATA + i);

		if (GetByte(pFrame,USBIDX_DESC_FORMAT) == DESCFMT_TEXT) {
				*(descobj->pWorkSpace + descobj->Len) = 0;
			descobj->Len++;
		}

		// update the global parameters
		switch (descobj->DescId) {
        case 0x1410:
            switch (descobj->DescEntry) {
	            case 0x12:		// magnification of nosepiece 1
	                pFloat = (float *)(pFrame + USBIDX_DESC_DATA);
	                np_mag[0] = *pFloat;
	                break;
	            case 0x22:
	                pFloat = (float *)(pFrame + USBIDX_DESC_DATA);
	                np_mag[1] = *pFloat;
	                break;
	            case 0x32:
	                pFloat = (float *)(pFrame + USBIDX_DESC_DATA);
	                np_mag[2] = *pFloat;
	                break;
	            case 0x42:
	                pFloat = (float *)(pFrame + USBIDX_DESC_DATA);
	                np_mag[3] = *pFloat;
	                break;
	            case 0x52:
	                pFloat = (float *)(pFrame + USBIDX_DESC_DATA);
	                np_mag[4] = *pFloat;
	                break;
	            case 0x62:
	                pFloat = (float *)(pFrame + USBIDX_DESC_DATA);
	                np_mag[5] = *pFloat;
	                break;
	            default:
	                break;
            }
            break;

		case 0x1300:
			switch (descobj->DescEntry) {
				case 0xAB:
					g_Device.parfocalEnable = pFrame[USBIDX_DESC_DATA];
			}
			break;
			
        default:
            break;
		}
	}

# if 0
	if (GetByte(pFrame, USBIDX_CMDCL) == 0x19) { //set if need answer via usb to pc
//		descobj->NeedAnswer = true;
//		descobj->pCmdFrame = malloc(GetByte(pFrame, USBIDX_LEN) + 9);
		descobj->MemProd++;
		for (i=0; i<GetByte(pFrame, USBIDX_LEN) + 9; i++)
			*(descobj->pCmdFrame +i) = *(pFrame +i);
	}
#endif
	descobj->pSem = CreateSem();
	StartDescTask(descobj);
	StartEromTask(regaddr, descobj->Len, descobj->pWorkSpace, Rw, descobj->pSem);
}
