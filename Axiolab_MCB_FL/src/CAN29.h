/*
 * CAN29.h
 *
 *  Created on: 2016Äê6ÔÂ30ÈÕ
 *      Author: zctiaxia
 */

#ifndef SRC_CAN29_H_
#define SRC_CAN29_H_

#include "TaskMgr.h"

#define WRONGPOSVAL  0

extern Uint32 g_MailBoxBusy;
#define MailBoxIsBusy(Index)	((g_MailBoxBusy >> Index) & 0x01)  // index 0 ~ 31
#define SetMailBoxBusy(Index)	g_MailBoxBusy |= ((Uint32)1 << Index)
#define ClearMailBoxBusy(Index)	g_MailBoxBusy &= ~((Uint32)1 << Index)
#define CheckMailBoxRMP(Index)  ((ECanaRegs.CANRMP.all >> Index) & 0x01)  // index 0 ~ 31
#define ClearMailBoxRMP(Index)		ECanaRegs.CANRMP.all &= ((Uint32)1 << Index)

// leilc: static allocation for simple until necessary to use getPID() instead
enum {
	PID_GENERAL = 1,
	PID_HAND_WHEEL_MONITOR,
	PID_Z_MOVE = 8
};


// Define CAN NODE
#define CANNODE_PC					0x10
#define CANNODE_REFLECTOR			0x01
#define CANNODE_NOSEPIECE			0x02
#define CANNODE_MOTORZ				0x0f
#define CANNODE_MCB					0x19
#define CANNODE_KEY					0x1c
#define CANNODE_RLTL				0x1f
#define CANNODE_Z_KNOB_ENCODER		0x25
#define CANNODE_MOTORX  			0x26
#define CANNODE_MOTORY  			0x27
#define CANNODE_HBO					0x28
#define CANNODE_LIGHTCTRL			0x29
#define CANNODE_HXP					0x36
#define CANNODE_COLI				0x50
#define CANNODE_CAM					0x54
#define CANNODE_ALL					0xff

// Define Device ID
#define DEVID_REFLECTOR				0x01  //Changer
#define DEVID_NP					0x02  //Changer if MOTORIZED, convey to Z-axis board
#define DEVID_Z_AXIS				0x0F  //Axis
//#define DEVID_TURRET				0x14  //Changer
#define DEVID_SYSTEM				0x15  //BIOS
#define DEVID_KEYS					0x1C  //Key
#define DEVID_RLTL_SW				0x1F  //Changer
//#define DEVID_XY_WHEEL			0x25  //Axis
#define DEVID_X_AXIS				0x26  //Axis
#define DEVID_Y_AXIS				0x27  //Axis
#define DEVID_LAMPLED				0x29  //Servo
#define DEVID_SLIDER				0x30  //changer

// Define Desc ID
#define DESC_MCB 					0x1000
#define DESC_CAM					0x2000
#define DESC_XYZ					0x1100
#define DESC_LIGHTCTRL				0x1200
#define DESC_RFLM					0x1201
#define DESC_TMLM					0x1202
#define DESC_USRSETTING				0x1300
#define DESC_ELEMENTS_NP   			0x1410
#define DESC_ELEMENTS_REF1			0x1501
#define DESC_ELEMENTS_REF2			0x1502
#define DESC_ELEMENTS_REF3			0x1503
#define DESC_ELEMENTS_REF4			0x1504
#define DESC_ELEMENTS_REF5			0x1505
#define DESC_ELEMENTS_REF6			0x1506






// Define Error Code
#define ERR_CMD_NOT_IMPLEMENTED		0x0001
#define ERR_INVALID_DEVID			0x0009
#define ERR_INVALID_PARAM           0x0010
#define ERR_DEV_LOCKED				0x0014
#define ERR_DEV_NOT_LOCKED			0x0024
#define ERR_DESC_IS_LOCKED			0x00AD
#define ERR_DESC_NOT_LOCKED			0x00AE


#define CN_SYSTEM					0x02	// chapter 2.2 "system commands"
#define CN_DLD						0x0e
#define CN_DESC						0x15
#define CN_CHANGER  				0x50
#define CN_SERVO    				0x51
#define CN_AXIS						0x5f
#define CN_GEMINI					0xA0	// Gemini private

// subId for changer
#define CHG_SUBID_MON				0x1f

typedef enum {
	CHG_BIT_EXIST,
	CHG_BIT_ENCODED,
	CHG_BIT_MOTORIZED,
	CHG_BIT_SCALING,
	CHG_BIT_HAS_KEY,
	CHG_BIT_OBSOLETE1,
	CHG_BIT_OBSOLETE2,
	CHG_BIT_HAS_ACR,
	CHG_BIT_MOT_ENABLED,
	CHG_BIT_INITED,
	CHG_BIT_MOVING,
	CHG_BIT_VALID_POS,
	CHG_BIT_HAS_TRIGGER
} CHANGER_STATUS;

typedef enum {
	SVO_BIT_EXIST,
	SVO_BIT_ENCODED,
	SVO_BIT_MOTORIZED,
	SVO_BIT_SCALING,
	SVO_BIT_HAS_KEY,
	SVO_BIT_OBSOLETE1,
	SVO_BIT_OBSOLETE2,
	SVO_BIT_OBSOLETE3,
	SVO_BIT_MOT_ENABLED,
	SVO_BIT_INITED,
	SVO_BIT_MOVING,
	SVO_BIT_VALID_POS,
} SERVO_STATUS;

struct C29ID_FIELD {
	Uint16      CmdNum		:8;   // 7:0
	Uint16      Src			:8;   // 15:8
	Uint16      Dest		:8;   // 23:16
	Uint16      CmdClass	:5;   // 29:24
};

typedef union  {
	Uint32				all;
	struct C29ID_FIELD	field;
} C29ID;

enum C29TXOBJ_STS {
	CANTX_WAITING,
	CANTX_SENDING,
	CANTX_FINISH
};

typedef struct {
	enum C29TXOBJ_STS Status;
	uint8_t		MBoxIndex;  //mailbox index from 0 to 31
	C29ID	  	ID;	   // for ID
	uint8_t		Len;	   // payload length
	uint8_t 	*pBuf;   // buffer for payload
	uint8_t		*pData;  // ptr to payload buffer
	uint8_t 	MemProd;
} C29TxObj;

typedef struct {
	C29ID	ID;
	uint8_t *pBuf;
	uint8_t *pData;
	uint8_t Len;
	uint8_t NumOfSegs;
	tSemiphore *puCount;  //sem for can2usb task, return length for multi msg
	uint8_t MBoxIndex;
	uint8_t MemProd;
	Uint32	SysTick;
	uint8_t TimeoutCount;
} C29RxObj;

typedef struct {
	uint8_t *pBuf;
	uint8_t Len;
	tSemiphore *uCount;  //sem for c29rx task
	uint8_t MBoxIndex;
	int8_t  MemProd;
	uint8_t TimeOutTimer;
} CAN2UsbTask;

struct STSFIELD {
	Uint16	IsPresent	:1; //0
	Uint16	IsInit		:1; //1
	Uint16	IsCoded		:1; //2
	Uint16	IsMotored	:1; //3
	Uint16	HasButton	:1; //4
	Uint16	IsSettled	:1; //5
	Uint16	IsLocked	:1; //6
	Uint16	HasACR		:1; //7
	Uint16	HasTrig		:1; //8
	Uint16	Calibrated	:1; //9
	Uint16	InWorkPos	:1; //10
	Uint16	InLoadPos	:1; //11
	Uint16	InLowSwtLim :1; //12
	Uint16	InUpSwLim	:1; //13
	Uint16	InLowHwLim	:1; //14
	Uint16	InUpHwLim	:1; //15
	Uint16	InImmersion	:1; //16
	Uint16				:1; //17
	Uint16	LampLedOn	:1; //18
	Uint16				:1; //19
	Uint16	HBOLevel1	:1; //20
	Uint16	HBOLevel2	:1; //21
	Uint16				:1; //22
	Uint16				:1; //23
	Uint16				:1; //24
	Uint16				:1; //25
	Uint16				:1; //26
	Uint16				:1; //27
	Uint16				:1; //28
	Uint16	CLSigOK		:1; //29
	Uint16	CLSensorOK	:1; //30
	Uint16	CLActive	:1;	//31
};

// Define device status bit
#define PRESENT		0x01
#define INIT		0x02
#define CODED		0x04
#define	MOTORED		0x08
#define	HASBUT		0x10
#define SETTLED		0x20
#define	LOCKED		0x40
#define	HASACR		0x80
#define HASTRIG		0x100
#define	CAL			0x200
#define INWORKPOS	0x400
#define INLOADPOS	0x800
#define INLOWSWLIM	0x1000
#define INUPSWLIM	0x2000
#define INLOHWLIM	0x4000
#define INUPHWLIM	0x8000
#define INIMMERSION 0x10000
#define LAMPLEDON	0x40000
#define HBOLV1		0x100000
#define HBOLV2		0x200000
#define CLSIGOK		0x20000000
#define CLSENSOROK	0x40000000
#define CLACTIVE	0x80000000

typedef union {
	Uint32				all;
	struct STSFIELD		field;
} DevStatus;

extern const uint8_t MCB2Axis[][];

extern void AnswerBios_MCB(uint8_t *pframe);
extern uint8_t *SearchTable(void *pTable, uint8_t Value, uint8_t Dim, uint8_t Depth);
extern void C29Msg2Usb(C29RxObj *pc29obj, uint8_t *pFrame);
extern void StartECanTxTask(C29ID Id, uint8_t *Payload, uint8_t Len, uint8_t Mailbox);
extern void StartECANRxTask(Uint32 MsgId, uint8_t MBoxIndex, uint8_t *buffer, uint8_t NumOfSegs, tSemiphore *puCount);
extern void StartCAN2UsbTask(C29ID Id, uint8_t MBoxIndex, Uint16 Len);
extern void monitorHandwheel(_Bool on);

#endif /* SRC_CAN29_H_ */
