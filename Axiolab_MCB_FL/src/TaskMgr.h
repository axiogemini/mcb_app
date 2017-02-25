/*
 * CAN29.h
 *
 *  Created on: 2016?6?7?
 *      Author: ZCTIAXIA
 */

#ifndef SRC_TASKMGR_H_
#define SRC_TASKMGR_H_

#include "F2806x_ECan.h"

// Define Task Priority
#define NORMAL 0
#define PRIOR  1

#define BUFSIZE 64
#define MAX_TASK_NUM 32
#define MAX_PID 15

enum TASKNAME {
	NOTASK,
	MainCtrl,
	EEPROM,
	IO6424,
	AD5293_BULB,
	MAX5422_RL,
	MAX5422_HAL,
	USBTRANSMIT,
	TRIGGERLIST,
	DEVLOCK,
	DESCRW,
	CANTX,
	CANRX,
	ECAN2USB,
	SAVEPARCENTER
};

typedef enum {
	TASKSTS_IDLE,
	TASKSTS_ONGOING,
	TASKSTS_FINISH,
	TASKSTS_TIMEOUT,
	TASKSTS_WRONG
} TASKSTATUS;

struct TaskStruct {
	enum TASKNAME   Name;
	TASKSTATUS Status;
	struct TaskStruct *Self;
	struct TaskStruct *link;
	uint8_t Pid;
	TASKSTATUS (*pfn)(void *);
	void *pTaskObj;  // CAN message length
};

typedef struct TaskStruct Task;

typedef struct {
	Uint16 Size;
	Uint16 ReadIndex;
	Uint16 WriteIndex;
	Uint16 RingBuf[BUFSIZE];
}tRingBufObj;

typedef struct {
	uint8_t Index;
	uint8_t Pid;
	bool 	Used;
	bool	Filled;
	uint8_t	Value;
	uint8_t *pBuf;
} tSemiphore;

typedef struct {
	uint8_t Num;
	bool 	Used;
} tPid;

extern Task TaskNode[];
extern tSemiphore Sem[];
extern tSemiphore* CreateSem(void);
extern uint8_t FindSem(uint8_t pid);
extern void FreeSem(uint8_t Index);
extern uint8_t GetPid(void);
extern void FreePid(uint8_t Num);
extern void TaskListInit(void);
extern Task* CreateTask(Uint16);
extern bool DeleteTask(Task *p_Task);
extern void TaskPoll(void);


#endif /* SRC_TASKMGR_H_ */
