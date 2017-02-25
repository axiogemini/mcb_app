/*
 * Parcenter.c
 *
 *  Created on: 2016?10?10?
 *      Author: ZCTIAXIA
 */

#include "include.h"

uint8_t PfInitiated = 0;
uint8_t Parfocal_SetFlag = 0;
uint8_t ParcenterX_SetFlag = 0;
uint8_t ParcenterY_SetFlag = 0;
tParcenterTable PcTable;
uint8_t PfClearAll = 1;

TASKSTATUS
SaveParcenterMgr(DescTask *descobj)
{
	int32 parcenter;

	if (GetSysTick0() != descobj->SysTick) {
		descobj->TimeoutCount++;
		descobj->SysTick = GetSysTick0();
		if (descobj->TimeoutCount >= 50) { // 5s time-out
			FreeSem(descobj->pSem->Index);
			ASSERT(descobj->MemProd == 1);
			free(descobj);
			return TASKSTS_TIMEOUT;
		}
	}
	if(!descobj->pSem->Filled)
		return TASKSTS_ONGOING;

	//parcenter = *(int32*)((uint8_t*)descobj->pBuf +3);
	//TODO use sem buf
	parcenter =  (int32)(*((uint8_t*)descobj->pSem->pBuf +3)) << 24 | (int32)(*((uint8_t*)descobj->pSem->pBuf +4)) << 16 | (int32)(*((uint8_t*)descobj->pSem->pBuf +5)) << 8 | (int32)(*((uint8_t*)descobj->pSem->pBuf +6));
	parcenter = parcenter == -1 ? 0 : parcenter;
	if (descobj->DescId == DESC_XYZ) {
		if (descobj->DescEntry <= 0x46) {
			CurrentParfocal = parcenter;
			g_Device.MotorZ.Position = parcenter;
//			g_Device.MotorZ.Parcenter = parcenter;
		}else if (descobj->DescEntry <= 0x96) {
			CurrentParcenter_X = parcenter;
			g_Device.MotorX.Position = parcenter;
//			g_Device.MotorX.Parcenter = parcenter;
		}else if (descobj->DescEntry <= 0xa6) {
			CurrentParcenter_Y = parcenter;
			g_Device.MotorY.Position = parcenter;
//			g_Device.MotorY.Parcenter = parcenter;
		}
	}
	WriteDesc(DESC_XYZ, descobj->DescEntry, &parcenter, 1, DESCFMT_LONG);

	FreeSem(descobj->pSem->Index);

	ASSERT(descobj->MemProd == 1);
	free(descobj);
	return TASKSTS_FINISH;
}

/* ********************************************************************************
 * send one can29 message to X,Y or Z to get actual position, start a can29 rx task
 * to wait for answer, after get answer, save to corresponding descriptor according
 * to nosepiece position. give up if 10s time out
 * ********************************************************************************/
void
StartParcenterSaveTask(Uint16 DescID, Uint16 DescEntry)
{
	DescTask *desctask;
	Task *pNewTask;
	C29ID c29id;
	uint8_t payload[8] = {0};
	uint8_t pid;
	tSemiphore *psem;
	int i;

	//pid = GetPid();
	pid = 3;
	if (DescID != DESC_XYZ)
		return;
	if ((psem = CreateSem()) == NULL)
		return;
	if (DescEntry >= 0x41 && DescEntry <= 0x46) {
		// leilc: clear all the parfocal setting in the RAM at first set. No need touch the E2PROM coz only read once
		if(PfClearAll) {
			PfClearAll = 0;
			for(i=0; i<NP_NUM; i++)
					PcTable.Z[i] = -1;
		}
		c29id.all = 0x180f195f;
		payload[0] = pid;
		payload[1] = 0x02;
		payload[2] = 0x00;	// leilc: devId should be 0x00 when send to X/Y/Z drive
		StartECanTxTask(c29id, payload, 3, 30);
	}else if (DescEntry >= 0x91 && DescEntry <= 0x96) {
		c29id.all = 0x1826195f;
		payload[0] = pid;
		payload[1] = 0x02;
		payload[2] = 0x00;
		StartECanTxTask(c29id, payload, 3, 30);
	}else if (DescEntry >= 0xa1 && DescEntry <= 0xa6) {
		c29id.all = 0x1827195f;
		payload[0] = pid;
		payload[1] = 0x02;
		payload[2] = 0x00;
		StartECanTxTask(c29id, payload, 3, 30);
	}else {
		return;
	}

	psem->pBuf = malloc(8); //8xN to prevent overflow
	psem->Pid = pid;

	desctask = malloc(sizeof(DescTask));
	desctask->MemProd = 1;
	desctask->DescEntry = DescEntry;
	desctask->DescId = DescID;
	desctask->Format = DESCFMT_LONG;
	desctask->Len = 1;
	desctask->NeedAnswer = false;
	desctask->pSem = psem;
	desctask->pBuf = NULL;
	desctask->pCmdFrame = NULL;
	desctask->pWorkSpace = NULL;
	desctask->SysTick = GetSysTick0(); // real time
	desctask->TimeoutCount = 0;		   // timeout count

	pNewTask = CreateTask(NORMAL);
	pNewTask->Name = SAVEPARCENTER;
	pNewTask->Status = TASKSTS_ONGOING;
	pNewTask->pfn = (TASKSTATUS (*)(void *))SaveParcenterMgr;
	pNewTask->pTaskObj = desctask;
}

