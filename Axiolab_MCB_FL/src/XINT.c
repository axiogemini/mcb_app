/*
 * XINT.c
 *
 *  Created on: 2016Äê3ÔÂ13ÈÕ
 *      Author: ZCTIAXIA
 */

#include "include.h"

__interrupt void xint1_isr(void)
{
	// TODO

	// Acknowledge this interrupt to get more from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

void XINT_RegisterISR(void)
{
    EALLOW;	// This is needed to write to EALLOW protected registers
    PieVectTable.XINT1 = &xint1_isr;
    EDIS;   // This is needed to disab
}
