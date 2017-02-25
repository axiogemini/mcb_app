/*
 * ECAN.c
 *
 *  Created on: 2015.4.27
 *      Author: xiatian
 */

#include "include.h"

void ECAN_Config(void);

void ECAN_MBoxSend(Uint32 MBoxIndex)
{
	uint32_t shadow;

	shadow = ECanaRegs.CANTRS.all;
	shadow |= (Uint32)1 << MBoxIndex;
	ECanaRegs.CANTRS.all = shadow;  // Set TRS for mailbox[index]
	//while (ECanaRegs.CANTA.all != MBoxIndex) {}  // Wait for all TAn bits to be set..
	//ECanaRegs.CANTA.all = MBoxIndex;   // Clear all TAn
}

void ECAN_SendMail(Uint16 mboxindex, C29ID id, uint8_t *pData, uint8_t length)  //mbox_index from 0 to 31
{
	volatile struct MBOX *pMBox;
	uint32_t shadow;

	pMBox = &ECanaMboxes.MBOX0 + mboxindex;

	ECanaRegs.CANME.all = ~((Uint32)1 << mboxindex); //~(1 << mboxindex); default 16 bit, shift overflow
	pMBox->MSGID.field.CmdNum = id.field.CmdNum;
	pMBox->MSGID.field.Src = id.field.Src;
	pMBox->MSGID.field.Dest =id.field.Dest;
	pMBox->MSGID.field.CmdClass = id.field.CmdClass;
	pMBox->MSGCTRL.bit.DLC = length;

	pMBox->MDL.byte.BYTE0 = *(pData+0);
	pMBox->MDL.byte.BYTE1 = *(pData+1);
	pMBox->MDL.byte.BYTE2 = *(pData+2);
	pMBox->MDL.byte.BYTE3 = *(pData+3);
	pMBox->MDH.byte.BYTE4 = *(pData+4);
	pMBox->MDH.byte.BYTE5 = *(pData+5);
	pMBox->MDH.byte.BYTE6 = *(pData+6);
	pMBox->MDH.byte.BYTE7 = *(pData+7);

	ECanaRegs.CANME.all = CAN_MAILBOX_EN_MASK;
	//ECAN_MBoxSend(1 << mboxindex);   // send MBox[Index]
	shadow = ECanaRegs.CANTRS.all;
	shadow |= (Uint32)1 << mboxindex;
	ECanaRegs.CANTRS.all = shadow;
}

void ECAN_Init(void)
{
	/* Create a shadow register structure for the CAN control registers. This is
 needed, since only 32-bit access is allowed to these registers. 16-bit access
 to these registers could potentially corrupt the register contents or return
 false data. */
	struct ECAN_REGS ECanaShadow;

	EALLOW;     // EALLOW enables access to protected bits

/* Configure eCAN RX and TX pins for CAN operation using eCAN regs*/
	ECanaShadow.CANTIOC.all = ECanaRegs.CANTIOC.all;
	ECanaShadow.CANTIOC.bit.TXFUNC = 1;
	ECanaRegs.CANTIOC.all = ECanaShadow.CANTIOC.all;

	ECanaShadow.CANRIOC.all = ECanaRegs.CANRIOC.all;
	ECanaShadow.CANRIOC.bit.RXFUNC = 1;
	ECanaRegs.CANRIOC.all = ECanaShadow.CANRIOC.all;

/* Configure eCAN for HECC mode - (reqd to access mailboxes 16 thru 31) */
	                              // HECC mode also enables time-stamping feature
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.SCB = 1;
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

/* Initialize all bits of 'Message Control Register' to zero */
// Some bits of MSGCTRL register come up in an unknown state. For proper operation,
// all bits (including reserved bits) of MSGCTRL must be initialized to zero
	ECanaMboxes.MBOX0.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX1.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX2.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX3.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX4.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX5.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX6.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX7.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX8.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX9.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX10.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX11.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX12.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX13.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX14.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX15.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX16.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX17.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX18.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX19.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX20.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX21.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX22.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX23.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX24.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX25.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX26.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX27.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX28.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX29.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX30.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX31.MSGCTRL.all = 0x00000000;

// TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
//  as a matter of precaution.

	ECanaRegs.CANTA.all = 0xFFFFFFFF;   /* Clear all TAn bits */

	ECanaRegs.CANRMP.all = 0xFFFFFFFF;  /* Clear all RMPn bits */

	ECanaRegs.CANGIF0.all = 0xFFFFFFFF; /* Clear all interrupt flag bits */
	ECanaRegs.CANGIF1.all = 0xFFFFFFFF;

/* Configure bit timing parameters for eCANA*/

	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR = 1 ;            // Set CCR = 1
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

	// Wait until the CPU has been granted permission to change the configuration registers
	do
	{
	  ECanaShadow.CANES.all = ECanaRegs.CANES.all;
	} while(ECanaShadow.CANES.bit.CCE != 1 );       // Wait for CCE bit to be set..

	ECanaShadow.CANBTC.all = 0;

/* The following block is for 80 MHz SYSCLKOUT. (40 MHz CAN module clock Bit rate = 1 Mbps
	   See Note at end of file. */

	ECanaShadow.CANBTC.bit.BRPREG = 29;
	ECanaShadow.CANBTC.bit.TSEG1REG = 10;
	ECanaShadow.CANBTC.bit.TSEG2REG = 2;

	ECanaShadow.CANBTC.bit.SAM = 1;
	ECanaRegs.CANBTC.all = ECanaShadow.CANBTC.all;

	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR = 0 ;            // Set CCR = 0
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

	ECanaRegs.CANOPC.all = 0xffff0000;

// Wait until the CPU no longer has permission to change the configuration registers
	do
	{
		ECanaShadow.CANES.all = ECanaRegs.CANES.all;
	} while (ECanaShadow.CANES.bit.CCE != 0 );       // Wait for CCE bit to be  cleared..

/* Disable all Mailboxes  */
	ECanaRegs.CANME.all = 0;        // Required before writing the MSGIDs

	EDIS;

	ECAN_Config();

	initSliceMsg();
}

void ECAN_Config(void)
{
	struct ECAN_REGS ECanaShadow;
	volatile struct MBOX *Mailbox;
	volatile union CANLAM_REG *LAMreg;

	Uint16 i;

	for (i = 0; i< 32; i++)
	{
		Mailbox = &ECanaMboxes.MBOX0 + i;

		Mailbox->MSGID.all = 0x00000000;
		Mailbox->MDH.all = 0x00000000;
		Mailbox->MDL.all = 0x00000000;
		// Bit31, STD/EXT ID flag, when AMI is '1', IDE is ingored and can be overwritten
		Mailbox->MSGID.bit.IDE = 1;
		// Bit30, 1: Enable Acceptance mask 0: Disable
		Mailbox->MSGID.bit.AME = 1;
		// Bit29, Auto Answer mode, 1: auto answer   0: normal transmit mode
		Mailbox->MSGID.bit.AAM = 0;
	}

	/* RECEIVE mailboxes MBOX 0 - 27 */

	// MBOX 0 : actually mailbox 0-2 have the same rule to catch any CAN message missed from mailbox 3-27
	ECanaMboxes.MBOX0.MSGID.field.CmdNum = 0;
	ECanaMboxes.MBOX0.MSGID.field.Src = 0x00;
	ECanaMboxes.MBOX0.MSGID.field.Dest = 0x10;
	ECanaMboxes.MBOX0.MSGID.field.CmdClass = 0x00;
	// MBOX 1 : target is pc
	ECanaMboxes.MBOX1.MSGID.field.CmdNum = 0;
	ECanaMboxes.MBOX1.MSGID.field.Src = 0x00;
	ECanaMboxes.MBOX1.MSGID.field.Dest = 0x11;
	ECanaMboxes.MBOX1.MSGID.field.CmdClass = 0;
	// MBOX 2 : target is cam
	ECanaMboxes.MBOX2.MSGID.field.CmdNum = 0;
	ECanaMboxes.MBOX2.MSGID.field.Src = 0x00;
	ECanaMboxes.MBOX2.MSGID.field.Dest = CANNODE_CAM;
	ECanaMboxes.MBOX2.MSGID.field.CmdClass = 0;
	// MBOX 3 : focus-z answer to mcb
	ECanaMboxes.MBOX3.MSGID.field.CmdNum = 0x5f;
	ECanaMboxes.MBOX3.MSGID.field.Src = CANNODE_MOTORZ;
	ECanaMboxes.MBOX3.MSGID.field.Dest = 0x19;
	ECanaMboxes.MBOX3.MSGID.field.CmdClass = 0x08;
	ECanaMboxes.MBOX3.MSGID.bit.AME = 0;  // mask not used
	// MBOX 4 : motor-x answer to mcb
	ECanaMboxes.MBOX4.MSGID.field.CmdNum = 0x5f;
	ECanaMboxes.MBOX4.MSGID.field.Src = CANNODE_MOTORX;
	ECanaMboxes.MBOX4.MSGID.field.Dest = 0x19;
	ECanaMboxes.MBOX4.MSGID.field.CmdClass = 0x08;
	ECanaMboxes.MBOX4.MSGID.bit.AME = 0;  // mask not used
	// MBOX 5 : motor-y answer to mcb
	ECanaMboxes.MBOX5.MSGID.field.CmdNum = 0x5f;
	ECanaMboxes.MBOX5.MSGID.field.Src = CANNODE_MOTORY;
	ECanaMboxes.MBOX5.MSGID.field.Dest = 0x19;
	ECanaMboxes.MBOX5.MSGID.field.CmdClass = 0x08;
	ECanaMboxes.MBOX5.MSGID.bit.AME = 0;  // mask not used
	// MBOX 6 : notification of axis
	ECanaMboxes.MBOX6.MSGID.field.CmdNum = 0x5f;
	ECanaMboxes.MBOX6.MSGID.field.Src = 0x00;
	ECanaMboxes.MBOX6.MSGID.field.Dest = 0x19;
	ECanaMboxes.MBOX6.MSGID.field.CmdClass = 0x07;
	// MBOX 7 : notification of Z knob
	ECanaMboxes.MBOX7.MSGID.field.CmdNum = 0x5f;
	ECanaMboxes.MBOX7.MSGID.field.Src = CANNODE_Z_KNOB_ENCODER;
	ECanaMboxes.MBOX7.MSGID.field.Dest = 0x19;
	ECanaMboxes.MBOX7.MSGID.field.CmdClass = 0x07;
	ECanaMboxes.MBOX7.MSGID.bit.AME = 0;  // mask not used
	// MBOX 8 : focus-z reply after finish to mcb
	ECanaMboxes.MBOX8.MSGID.field.CmdNum = 0x5f;
	ECanaMboxes.MBOX8.MSGID.field.Src = CANNODE_MOTORZ;
	ECanaMboxes.MBOX8.MSGID.field.Dest = 0x19;
	ECanaMboxes.MBOX8.MSGID.field.CmdClass = 0x09;
	ECanaMboxes.MBOX8.MSGID.bit.AME = 0;  // mask not used

	/* TRANSMIT mailboxes MBOX 28 - 31 */
	// MBOX 28 - Derectly convey
	ECanaMboxes.MBOX28.MSGID.field.CmdNum = 31;
	ECanaMboxes.MBOX28.MSGID.field.Src = 31;
	ECanaMboxes.MBOX28.MSGID.field.Dest = 31;
	ECanaMboxes.MBOX28.MSGID.field.CmdClass = 31;
	// MBOX 29 - Translate convey
	ECanaMboxes.MBOX29.MSGID.field.CmdNum = 31;
	ECanaMboxes.MBOX29.MSGID.field.Src = 31;
	ECanaMboxes.MBOX29.MSGID.field.Dest = 31;
	ECanaMboxes.MBOX29.MSGID.field.CmdClass = 31;
	// MBOX 30 - Autonomic msg
	ECanaMboxes.MBOX30.MSGID.field.CmdNum = 31;
	ECanaMboxes.MBOX30.MSGID.field.Src = 31;
	ECanaMboxes.MBOX30.MSGID.field.Dest = 31;
	ECanaMboxes.MBOX30.MSGID.field.CmdClass = 31;
	// MBOX 31 - ACK answer
	ECanaMboxes.MBOX31.MSGID.field.CmdNum = 31;
	ECanaMboxes.MBOX31.MSGID.field.Src = 31;
	ECanaMboxes.MBOX31.MSGID.field.Dest = 31;
	ECanaMboxes.MBOX31.MSGID.field.CmdClass = 31;


	/* Local Acceptance Mask set */
	for (i = 0; i < 9; i++)
	{
		LAMreg = &ECanaLAMRegs.LAM0 + i;

		// ID Mask Setting, both STD and EXT ID can be accepted
		LAMreg->bit.LAMI = 1;	// When '1': Standard and extended frames can be received.
								// When '0': The identifier extension bit stored in the mailbox determines which messages shall be received
		if (i<3) {
			// for the dst address 0x10, 0x11 and 0x54, receive arbitary message
//			LAMreg->bit.LAM_H = 0xff00;
//			LAMreg->bit.LAM_L = 0xffff;

			// leilc: receive any CAN message missed from the rules of other mailbox
			LAMreg->bit.LAM_H = 0xffff;
			LAMreg->bit.LAM_L = 0xffff;

		} else {
			// Set mask bit, For corresponding bit, 1 is don't care
			LAMreg->bit.LAM_H = 0x0000;
			LAMreg->bit.LAM_L = 0xff00;
		}
	}

    // Configure Mailboxes 0-27 as Rx, 28-31 as Tx
    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
	ECanaRegs.CANMD.all = 0x0FFFFFFF;

    // Enable all Mailboxes */
    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
	ECanaRegs.CANME.all = 0xFFFFFFFF;

    // Specify how many bits will be sent/received
	ECanaMboxes.MBOX28.MSGCTRL.bit.DLC = 8;  // Version info
	ECanaMboxes.MBOX29.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX30.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX31.MSGCTRL.bit.DLC = 8;

	EALLOW;
	ECanaRegs.CANMIM.all = 0x00000000;  // interrupt mask
	ECanaRegs.CANMIL.all = 0x00000000;
	ECanaRegs.CANGIM.all = 0x00000000;
	ECanaRegs.CANGIM.bit.GIL = 0;
	ECanaRegs.CANGIM.bit.I0EN = 1;
	ECanaRegs.CANMIM.bit.MIM0 = 1;
	EDIS;

    // Configure the eCAN for non self test mode
    // Enable the enhanced features of the eCAN.
	EALLOW;
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.STM = 0;		// Configure CAN for non-self-test mode
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
	EDIS;
}


static slice_msg sliceMsg[MAX_CHAIN_MSG_SENDER] = {0};	

void initSliceMsg(void)
{
	memset((void *)&sliceMsg, 0, sizeof(sliceMsg));
}

Uint8 getSliceMsg(slice_msg **p)
{
	int i;
	for (i=0; i<MAX_CHAIN_MSG_SENDER; i++) {
		if (sliceMsg[i].src == 0) {
			*p = &sliceMsg[i];
			return i;
		}
	}
	return NO_SLICE_MSG_SPACE;
}

Uint8 matchSliceMsg(CAN_msg can29, slice_msg **p)
{
	int i;
	for (i=0; i<MAX_CHAIN_MSG_SENDER; i++) {
		if (sliceMsg[i].src == CANID_SOURCE(can29) &&
			sliceMsg[i].dest == CANID_DEST(can29) &&
			sliceMsg[i].cmdCls== CANID_CLASS(can29) &&
			sliceMsg[i].cmdNum== CANID_CMDNUM(can29)) {
				*p = &sliceMsg[i];
				return i;
		}
	}
	return NO_SLICE_MSG_MATCH;
}

void freeSliceMsg(slice_msg *p)
{
	memset((void *)p, 0, sizeof(slice_msg));
}

void
ECAN_Listen(void)
{
	uint8_t index = 0, payload[8] = {0};

	// Check if any received message
	if (ECanaRegs.CANRMP.all != 0){
		// MBOX 0-2 : destination to 0x10, 0x11, 0x54, need to be send by USB port
		int rcvIdx;
		CAN_msg can29;
		volatile struct MBOX *pMBOX;

		// redirect the message with target address of 0x10, 0x11, 0x54 to USB port
		for (rcvIdx=2; rcvIdx>0; rcvIdx--)
		{
			slice_msg *pSlice;

			if (ECanaRegs.CANRMP.all & 1<<rcvIdx) {
				ECanaRegs.CANRMP.all = 1<<rcvIdx;

				pMBOX = &ECanaMboxes.MBOX0 + rcvIdx;

				if (pMBOX->MSGID.field.Dest != 0x10 && pMBOX->MSGID.field.Dest != 0x11 && pMBOX->MSGID.field.Dest != 0x54)
					return;

				UsbTxTask usbtx;

				can29.id = pMBOX->MSGID.all;
				can29.data[0] = pMBOX->MDL.byte.BYTE0;
				can29.data[1] = pMBOX->MDL.byte.BYTE1;
				can29.data[2] = pMBOX->MDL.byte.BYTE2;
				can29.data[3] = pMBOX->MDL.byte.BYTE3;
				can29.data[4] = pMBOX->MDH.byte.BYTE4;
				can29.data[5] = pMBOX->MDH.byte.BYTE5;
				can29.data[6] = pMBOX->MDH.byte.BYTE6;
				can29.data[7] = pMBOX->MDH.byte.BYTE7;
				can29.len = pMBOX->MSGCTRL.bit.DLC;

				if (CANID_CMDNUM(can29) == 0) {										// command number = 0, sliced message
					if (getSliceMsg(&pSlice) == NO_SLICE_MSG_SPACE)
						return;
					pSlice->segment = can29.data[3];
					if (pSlice->segment <= 1)
						return;														// should have more than one segment
					if (pSlice->segment > MAX_SLICE_NUM)
						return;
					pSlice->dle = 0x10;
					pSlice->stx = 0x02;
					pSlice->dest = CANID_DEST(can29);
					pSlice->src = CANID_SOURCE(can29);
					pSlice->dataLen = 0;											// a new start!
					pSlice->cmdCls = CANID_CLASS(can29);
					pSlice->cmdNum = can29.data[1];

					pSlice->data[pSlice->dataLen++] = can29.data[4];
					pSlice->data[pSlice->dataLen++] = can29.data[5];
					pSlice->data[pSlice->dataLen++] = can29.data[6];
					pSlice->data[pSlice->dataLen++] = can29.data[7];
					pSlice->segment--;

				} else {
					int i;
					if (matchSliceMsg(can29, &pSlice) != NO_SLICE_MSG_MATCH) {
						// rest part of the chained message
						for (i=0; i<can29.len; i++)
							pSlice->data[pSlice->dataLen++] = can29.data[i];
						pSlice->segment--;
						if (pSlice->segment == 0)	{
							pSlice->data[pSlice->dataLen++] = 0x10;
							pSlice->data[pSlice->dataLen++] = 0x03;
							pSlice->dataLen -= 2;									// excluding 0x10 and 0x03
							usbtx.Len = pSlice->dataLen + USB_PKT_OVERHEAD;						
							usbtx.pBuf = (void *)pSlice;
							StartUsbTxTask(&usbtx);
							freeSliceMsg(pSlice);
						}
					} else {
						int i;
						// just redirect to USB
						Uint8 usbtxmsg[17];
						usbtx.pBuf = (void *)&usbtxmsg[0];
						usbtx.Len = can29.len + USB_PKT_OVERHEAD;
						usbtxmsg[0] = 0x10;
						usbtxmsg[1] = 0x02;
						usbtxmsg[2] = pMBOX->MSGID.field.Dest;
						usbtxmsg[3] = pMBOX->MSGID.field.Src;
						usbtxmsg[4] = can29.len;									// length of can29 message
						usbtxmsg[5] = pMBOX->MSGID.field.CmdClass;
						usbtxmsg[6] = pMBOX->MSGID.field.CmdNum;
						for (i=7; i<7+can29.len; i++)
							usbtxmsg[i] = can29.data[i-7];
						usbtxmsg[i++] = 0x10;
						usbtxmsg[i++] = 0x03;
						StartUsbTxTask(&usbtx);
					}
				}
				pMBOX = NULL;
			}
		}
		// MBOX 3 : focus-z answer to MCB
		if (ECanaRegs.CANRMP.bit.RMP3 == 1) {
			*(payload   ) = ECanaMboxes.MBOX3.MDL.byte.BYTE0;
			*(payload +1) = ECanaMboxes.MBOX3.MDL.byte.BYTE1;
			*(payload +2) = ECanaMboxes.MBOX3.MDL.byte.BYTE2;
			*(payload +3) = ECanaMboxes.MBOX3.MDL.byte.BYTE3;
			*(payload +4) = ECanaMboxes.MBOX3.MDH.byte.BYTE4;
			*(payload +5) = ECanaMboxes.MBOX3.MDH.byte.BYTE5;
			*(payload +6) = ECanaMboxes.MBOX3.MDH.byte.BYTE6;
			*(payload +7) = ECanaMboxes.MBOX3.MDH.byte.BYTE7;
			g_Device.MotorZ.Position = (int32)payload[3] << 24 | (int32)payload[4] << 16 | (int32)payload[5] << 8 | (int32)payload[6];

			// leilc: keep the original code untouched
			index = FindSem(ECanaMboxes.MBOX3.MDL.byte.BYTE0);
			if (index < MAX_PID && Sem[index].pBuf != NULL) {
				*(Sem[index].pBuf   ) = ECanaMboxes.MBOX3.MDL.byte.BYTE0;
				*(Sem[index].pBuf +1) = ECanaMboxes.MBOX3.MDL.byte.BYTE1;
				*(Sem[index].pBuf +2) = ECanaMboxes.MBOX3.MDL.byte.BYTE2;
				*(Sem[index].pBuf +3) = ECanaMboxes.MBOX3.MDL.byte.BYTE3;
				*(Sem[index].pBuf +4) = ECanaMboxes.MBOX3.MDH.byte.BYTE4;
				*(Sem[index].pBuf +5) = ECanaMboxes.MBOX3.MDH.byte.BYTE5;
				*(Sem[index].pBuf +6) = ECanaMboxes.MBOX3.MDH.byte.BYTE6;
				*(Sem[index].pBuf +7) = ECanaMboxes.MBOX3.MDH.byte.BYTE7;
				Sem[index].Filled = true;
			}
			ECanaRegs.CANRMP.all = 0x00000008;
		}
		// notification of axis
		if (ECanaRegs.CANRMP.bit.RMP6 == 1) {
			*(payload   ) = ECanaMboxes.MBOX6.MDL.byte.BYTE0;
			*(payload +1) = ECanaMboxes.MBOX6.MDL.byte.BYTE1;
			*(payload +2) = ECanaMboxes.MBOX6.MDL.byte.BYTE2;
			*(payload +3) = ECanaMboxes.MBOX6.MDL.byte.BYTE3;
			*(payload +4) = ECanaMboxes.MBOX6.MDH.byte.BYTE4;
			*(payload +5) = ECanaMboxes.MBOX6.MDH.byte.BYTE5;
			*(payload +6) = ECanaMboxes.MBOX6.MDH.byte.BYTE6;
			*(payload +7) = ECanaMboxes.MBOX6.MDH.byte.BYTE7;
			if (payload[1] == 0x02) { // subid = 0x02, get Z-axis position each time it changes
				if (ECanaMboxes.MBOX6.MSGID.field.Src == CANNODE_MOTORZ) {
					g_Device.MotorZ.isValid = true;
					g_Device.MotorZ.Position = (int32)payload[3] << 24 | (int32)payload[4] << 16 | (int32)payload[5] << 8 | (int32)payload[6];
					g_Device.MotorZ.moving = false;
					if ((PfInitiated & 0x01) == 1 && g_Device.NosePiece.Settled)
						;
				}else if (ECanaMboxes.MBOX6.MSGID.field.Src == CANNODE_MOTORX) {
					g_Device.MotorX.Position = (int32)payload[3] << 24 | (int32)payload[4] << 16 | (int32)payload[5] << 8 | (int32)payload[6];
					if ((PfInitiated & 0x02) == 1 && g_Device.NosePiece.Settled)
						g_Device.MotorX.PrevParIndex = g_Device.NosePiece.Position;
				}else if (ECanaMboxes.MBOX6.MSGID.field.Src == CANNODE_MOTORY) {
					g_Device.MotorY.Position = (int32)payload[3] << 24 | (int32)payload[4] << 16 | (int32)payload[5] << 8 | (int32)payload[6];
					if ((PfInitiated & 0x04) == 1 && g_Device.NosePiece.Settled)
						g_Device.MotorY.PrevParIndex = g_Device.NosePiece.Position;
				}
			}
			ECanaRegs.CANRMP.all = 0x00000040;
		}
		//notification of servo
		if (ECanaRegs.CANRMP.bit.RMP7 == 1) {
			*(payload   ) = ECanaMboxes.MBOX7.MDL.byte.BYTE0;
			*(payload +1) = ECanaMboxes.MBOX7.MDL.byte.BYTE1;
			*(payload +2) = ECanaMboxes.MBOX7.MDL.byte.BYTE2;
			*(payload +3) = ECanaMboxes.MBOX7.MDL.byte.BYTE3;
			*(payload +4) = ECanaMboxes.MBOX7.MDH.byte.BYTE4;
			*(payload +5) = ECanaMboxes.MBOX7.MDH.byte.BYTE5;
			*(payload +6) = ECanaMboxes.MBOX7.MDH.byte.BYTE6;
			*(payload +7) = ECanaMboxes.MBOX7.MDH.byte.BYTE7;
			g_Device.HandWheelCode = (int32)payload[3] << 24 | (int32)payload[4] << 16 | (int32)payload[5] << 8 | (int32)payload[6];
			ECanaRegs.CANRMP.all = 0x00000080;
		}
		// MBOX 8 : focus-z reply after finish to MCB
		if (ECanaRegs.CANRMP.bit.RMP8 == 1) {
			*(payload   ) = ECanaMboxes.MBOX8.MDL.byte.BYTE0;
			*(payload +1) = ECanaMboxes.MBOX8.MDL.byte.BYTE1;
			*(payload +2) = ECanaMboxes.MBOX8.MDL.byte.BYTE2;
			*(payload +3) = ECanaMboxes.MBOX8.MDL.byte.BYTE3;
			*(payload +4) = ECanaMboxes.MBOX8.MDH.byte.BYTE4;
			*(payload +5) = ECanaMboxes.MBOX8.MDH.byte.BYTE5;
			*(payload +6) = ECanaMboxes.MBOX8.MDH.byte.BYTE6;
			*(payload +7) = ECanaMboxes.MBOX8.MDH.byte.BYTE7;
			if (payload[1] == PID_Z_MOVE)
//				g_Device.MotorZ.moving = false; // Adam has not implemented the reply after finish yet
			ECanaRegs.CANRMP.all = (1<<8);
		}
	}
}


