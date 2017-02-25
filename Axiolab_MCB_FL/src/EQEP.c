/*
 * EQEP.c
 *
 *  Created on: 2015Äê4ÔÂ26ÈÕ
 *      Author: xiatian
 */



#include "include.h"


void EQEP1_RegisterIsr(void);
__interrupt void EQEP1_ISR(void);

// EQEP1 for X-Axis
// EQEP2 for Y-Axis
void EQEP_Init(void)
{
    EQep1Regs.QDECCTL.bit.QSRC = 0;      // QEP quadrature count mode
    EQep1Regs.QDECCTL.bit.XCR=0;        // 2x resolution (cnt falling and rising edges)

	EQep1Regs.QEPCTL.bit.FREE_SOFT = 2;
    EQep1Regs.QEPCTL.bit.PCRM = 1;       // PCRM=01 mode - QPOSCNT reset on maximum position
    EQep1Regs.QEPCTL.bit.IEI = 0;       // PCRM=00 mode - do nothing for QPOSCNT initiation on index event
    EQep1Regs.QEPCTL.bit.QPEN = 1;        // QEP enable

    EQep1Regs.QPOSMAX = 0xffffffff;

    EQep1Regs.QEINT.bit.IEL = 0;
}



