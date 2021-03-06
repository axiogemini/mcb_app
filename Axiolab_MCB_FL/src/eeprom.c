 /*
 * eeprom.c
 *
 *  Created on: 2016?6?26?
 *      Author: xiatian
 */

#include "include.h"

static EromTask *g_pEeprom;

void
EromThread_Reset(void)
{
    g_pEeprom->Status = EROM_IDLE;
    g_pEeprom->Event = EROM_IDLE;
    g_pEeprom->DevAddr = EEPROM_I2CADDR;
    free(g_pEeprom->pBuf);
    g_pEeprom->RegAddr = 0;
    g_pEeprom->DataLen = 0;
}

static Bool
Eeprom_Write(Erom_Ops *erom_ops)
{
    if (g_I2cMsg.Status != I2C_MSGSTAT_INACTIVE)
        return false;

    g_I2cMsg.Status = I2C_MSGSTAT_SEND_WITHSTOP;
    g_I2cMsg.DevAddr = erom_ops->DevAddr;  // if no bank
    g_I2cMsg.Len = erom_ops->Len;
    g_I2cMsg.RegAddr.all = erom_ops->RegAddr;
    g_I2cMsg.LongRegAddrFlag = true;
    g_I2cMsg.pBuf = erom_ops->pBuf;
    return true;
}

static void
StartInterval()
{
    EPwmTimerStart(6,5500);
}

static bool
CheckInterval()
{
    return EPwmTimerisOver(6);
}

static Bool
Eeprom_Read(Erom_Ops *erom_ops)
{
    if (g_I2cMsg.Status != I2C_MSGSTAT_INACTIVE)
        return false;

    g_I2cMsg.Status = I2C_MSGSTAT_SEND_NOSTOP;
    g_I2cMsg.DevAddr = erom_ops->DevAddr;
    g_I2cMsg.Len = erom_ops->Len;
    g_I2cMsg.RegAddr.all = erom_ops->RegAddr;
    g_I2cMsg.LongRegAddrFlag = true;
    g_I2cMsg.pBuf = erom_ops->pBuf;
    return true;
}

static void
StartOneEromWrite(EromTask *eromtask)
{
    Erom_Ops erom_ops;
    uint8_t length_lim;  // for bank limit

    erom_ops.DevAddr = eromtask->DevAddr;
    erom_ops.RegAddr = eromtask->RegAddr;
    length_lim = PAGESIZE - eromtask->RegAddr % PAGESIZE;
    erom_ops.Len = (eromtask->DataLen > length_lim) ? length_lim : eromtask->DataLen;
    erom_ops.pBuf = eromtask->pData;
    if (Eeprom_Write(&erom_ops) == true) {
        eromtask->pData = (uint8_t*)eromtask->pData + erom_ops.Len;
        eromtask->RegAddr = eromtask->RegAddr + erom_ops.Len;
        eromtask->DataLen -= erom_ops.Len;
        StartInterval();
    }
}

static void
StartOneEromRead(EromTask *eromtask)
{
    Erom_Ops erom_ops;

    erom_ops.DevAddr = eromtask->DevAddr;
    erom_ops.RegAddr = eromtask->RegAddr;
    erom_ops.Len = eromtask->DataLen;
    erom_ops.pBuf = eromtask->pData;
    if (Eeprom_Read(&erom_ops) == true) {
        eromtask->pData = (Uint16*)eromtask->pData + eromtask->DataLen;
        eromtask->DataLen = 0;
    }
}

// execute EromThread(pEeprom)
// this server poll EepromThread status, check if need divde data to segments for writing within bank, and call I2c_Server() if thread is operation
static uint8_t
Eeprom_Thread(EromTask *pEeprom)
{
    if (pEeprom->Status == EROM_TOWRITE || pEeprom->Status == EROM_TOREAD) {
        if (g_I2cMsg.Status != I2C_MSGSTAT_INACTIVE)    // I2c busy, waiting until next time poll
            return pEeprom->Status;
        else
            return ++pEeprom->Status;
    }
    if (pEeprom->Status == EROM_WRITING || pEeprom->Status == EROM_READING) {
        if (g_I2cMsg.Status == I2C_MSGSTAT_INACTIVE) {  // I2c transaction finished
//          if (pEeprom->pData == pEeprom->pBuf + pEeprom->DataLen) {
            if (pEeprom->DataLen == 0) {
                I2cMsg_Reset();
                return (pEeprom->Status = EROM_FINISH);
            }else {
                if (pEeprom->Status == EROM_WRITING) {
                    if (CheckInterval()) {              // check if eeprom write after 5ms
                        StartOneEromWrite(pEeprom);
                    }
                }else if (pEeprom->Status == EROM_READING)
                    StartOneEromRead(pEeprom);
                return pEeprom->Status;
            }
        }else {                                         // I2c transaction running
            I2CA_Server();
            return pEeprom->Status;
        }
    }
    return EROM_WRONGSTS; //should not reach here
}

TASKSTATUS
EepromTaskMgr(EromTask *eromtask)  // decide whether need hook concrete eromtask to Eeprom_Thread
{
    uint8_t result;

    if (g_I2cApplication == 1 && eromtask->Status == EROM_WAITING)  // I2C is occupied
        return TASKSTS_ONGOING;
    if (g_I2cApplication == 0 && eromtask->Status == EROM_WAITING) { // I2C is standby so we can start task
        g_pEeprom = eromtask;
        g_I2cApplication = 1;
        eromtask->Status = eromtask->Event;
        return TASKSTS_ONGOING;
    }
    if (g_I2cApplication == 1 && eromtask->Status != EROM_WAITING) {
        result = Eeprom_Thread(eromtask);
        if (result == EROM_FINISH) {                                    // whole task is finished
            g_I2cApplication = 0;
            if (eromtask->pSem != NULL)
                eromtask->pSem->Value = 1;
            ASSERT(eromtask->MemProd == 1);
            free(eromtask);
            DELAY_US(5000);                         // minimum interval between two eeprom write
            return TASKSTS_FINISH;
        }else {   // task is ongoing
            g_I2cApplication = 1;
            return TASKSTS_ONGOING;
        }
    }
    return TASKSTS_WRONG;   //should not get here
}

void
StartEromTask(Uint16 RegAddr, Uint16 DataLen, Uint16 *pBuf, Bool Rw, tSemiphore *pSem) //Rw is 1:r 0:w. Sem is 1: finish 0: ongoing
{
    EromTask *eromobj;
    Task *pNewTask;

    pNewTask = CreateTask(NORMAL);
    eromobj = malloc(sizeof(EromTask));
    eromobj->MemProd = 1;
    eromobj->DevAddr = 0x50;
    eromobj->RegAddr = RegAddr;
    eromobj->DataLen = DataLen;
    eromobj->Event = Rw ? EROM_TOREAD : EROM_TOWRITE;
    eromobj->Status = EROM_WAITING;
    eromobj->pBuf = pBuf;
    eromobj->pData = eromobj->pBuf;
    eromobj->pSem = pSem;
    eromobj->pSem->Value = 0;

    pNewTask->Name = EEPROM;
    pNewTask->Status = TASKSTS_ONGOING;
    pNewTask->pfn = (TASKSTATUS (*)(void *))EepromTaskMgr;
    pNewTask->pTaskObj = eromobj;
}






















