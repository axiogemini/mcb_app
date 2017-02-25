/*
 * PWM5.c
 *
 *  Created on: 2015Äê7ÔÂ27ÈÕ
 *      Author: xiatian
 */

#include "include.h"

static _Bool g_PwmTimeRunning[7] = {0,0,0,0,0,0,0};

void EPwmTimerInit()
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;          // Stop all the TB clocks
    EDIS;

    // Disable Sync
    EPwm1Regs.TBCTL.bit.SYNCOSEL = 11;              // Pass through

    // Initally disable Free/Soft Bits
    EPwm1Regs.TBCTL.bit.FREE_SOFT = 0;
    EPwm1Regs.TBCTL.bit.PHSEN = 0;
    //EPwm1Regs.TBCTL.bit.PRDLD = 1;
    EPwm1Regs.TBPRD = 0xE4E0;                       // Set up PWM1 Period
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;      // Count up mode
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 6;              // TBCLK = SYSCLKOUT / 12 / 128  TBCLK = SYSCLKOUT / (HSPCLKDIV*2 * 2^CLKDIV)
    EPwm1Regs.TBCTL.bit.CLKDIV = 7;
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        // Select INT on PRD event
    EPwm1Regs.ETSEL.bit.INTEN = PWM_INT_ENABLE;     // enable INT
    EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;             // Generate INT on 1st event
    EPwm1Regs.TBCTR = 0x0000;                       // Clear timer counter

    EPwm2Regs.TBCTL.bit.SYNCOSEL = 11;              // Pass through
    EPwm2Regs.TBCTL.bit.FREE_SOFT = 0;
    EPwm2Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;
    EPwm2Regs.TBPRD = PWM2_TIMER_TBPRD;             // Set up PWM2 Period
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;      // Count up mode
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 5;              // TBCLK = SYSCLKOUT / 10 / 128 = 1/70312.5
    EPwm2Regs.TBCTL.bit.CLKDIV = 7;
    EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        // Select INT on PRD event
    EPwm2Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;    // disable INT
    EPwm2Regs.ETPS.bit.INTPRD = ET_DISABLE;         // Generate INT on 1st event
    EPwm2Regs.TBCTR = 0x0000;                       // Clear timer counter

    EPwm3Regs.TBCTL.bit.SYNCOSEL = 11;              // Pass through
    EPwm3Regs.TBCTL.bit.FREE_SOFT = 0;
    EPwm3Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;
    EPwm3Regs.TBPRD = PWM3_TIMER_TBPRD;             // Set up PWM2 Period
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;      // Count up mode
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = 5;              // TBCLK = SYSCLKOUT
    EPwm3Regs.TBCTL.bit.CLKDIV = 3;
    EPwm3Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        // Select INT on PRD event
    EPwm3Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;    // disable INT
    EPwm3Regs.ETPS.bit.INTPRD = ET_DISABLE;             // Generate INT on 1st event
    EPwm3Regs.TBCTR = 0x0000;                       // Clear timer counter

    EPwm4Regs.TBCTL.bit.SYNCOSEL = 11;              // Pass through
    EPwm4Regs.TBCTL.bit.FREE_SOFT = 0;
    EPwm4Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;
    EPwm4Regs.TBPRD = PWM4_TIMER_TBPRD;             // Set up PWM4 Period
    EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;      // Count up mode
    EPwm4Regs.TBCTL.bit.HSPCLKDIV = 5;              // TBCLK = SYSCLKOUT
    EPwm4Regs.TBCTL.bit.CLKDIV = 3;
    EPwm4Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        // Select INT on PRD event
    EPwm4Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;    // disable INT
    EPwm4Regs.ETPS.bit.INTPRD = ET_DISABLE;         // Generate INT on 1st event
    EPwm4Regs.TBCTR = 0x0000;                       // Clear timer counter

    EPwm5Regs.TBCTL.bit.SYNCOSEL = 11;              // Pass through
    EPwm5Regs.TBCTL.bit.FREE_SOFT = 0;
    EPwm5Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;
    EPwm5Regs.TBPRD = PWM4_TIMER_TBPRD;             // Set up PWM4 Period
    EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;      // Count up mode
    EPwm5Regs.TBCTL.bit.HSPCLKDIV = 5;              // TBCLK = SYSCLKOUT
    EPwm5Regs.TBCTL.bit.CLKDIV = 3;
    EPwm5Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        // Select INT on PRD event
    EPwm5Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;    // disable INT
    EPwm5Regs.ETPS.bit.INTPRD = ET_1ST;         // Generate INT on 1st event
    EPwm5Regs.TBCTR = 0x0000;                       // Clear timer counter

    EPwm6Regs.TBCTL.bit.SYNCOSEL = 11;              // Pass through
    EPwm6Regs.TBCTL.bit.FREE_SOFT = 0;
    EPwm6Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;
    EPwm6Regs.TBPRD = 0x0040;                       // Set up PWM6 Period
    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;      // Count up mode
    EPwm6Regs.TBCTL.bit.HSPCLKDIV = 5;              // TBCLK = SYSCLKOUT
    EPwm6Regs.TBCTL.bit.CLKDIV = 3;
    EPwm6Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        // Select INT on PRD event
    EPwm6Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;    // disable INT
    EPwm6Regs.ETPS.bit.INTPRD = ET_DISABLE;         // Generate INT on 1st event
    EPwm6Regs.TBCTR = 0x0000;                       // Clear timer counter

   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;           // Start all the timers synced
   EDIS;
}

// Interrupt routines uses in this example:
__interrupt void epwm1_timer_isr(void)
{
    //EPwm1Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;      one shot INT

    // TODO application
    GpioDataRegs.GPBTOGGLE.bit.GPIO56 = 1;

    EPwm1Regs.ETCLR.bit.INT = 1;  // Clears the ETFLG[INT] flag bit and enable further interrupts pulses to be generated
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

__interrupt void epwm2_timer_isr(void)
{
    EPwm2Regs.ETPS.bit.INTPRD = ET_DISABLE;
    EPwm2Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;    //one shot INT
    EPwm2Regs.ETCLR.bit.INT = 1;  // Clears the ETFLG[INT] flag bit and enable further interrupts pulses to be generated
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    g_PwmTimeRunning[2] = false;
}

__interrupt void epwm3_timer_isr(void)
{
    EPwm3Regs.ETPS.bit.INTPRD = ET_DISABLE;
    EPwm3Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;    //one shot INT
    EPwm3Regs.ETCLR.bit.INT = 1;  // Clears the ETFLG[INT] flag bit and enable further interrupts pulses to be generated
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    g_PwmTimeRunning[3] = false;
}

__interrupt void epwm4_timer_isr(void)
{
    EPwm4Regs.ETPS.bit.INTPRD = ET_DISABLE;
    EPwm4Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;    //one shot INT
    EPwm4Regs.ETCLR.bit.INT = 1;  // Clears the ETFLG[INT] flag bit and enable further interrupts pulses to be generated
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    g_PwmTimeRunning[4] = false;
}

__interrupt void epwm5_timer_isr(void)
{
    EPwm5Regs.ETPS.bit.INTPRD = ET_DISABLE;
    EPwm5Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;    //one shot INT
    EPwm5Regs.ETCLR.bit.INT = 1;  // Clears the ETFLG[INT] flag bit and enable further interrupts pulses to be generated
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    g_PwmTimeRunning[5] = false;
}

__interrupt void epwm6_timer_isr(void)
{
    EPwm6Regs.ETPS.bit.INTPRD = ET_DISABLE;
    EPwm6Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;    //one shot INT
    EPwm6Regs.ETCLR.bit.INT = 1;  // Clears the ETFLG[INT] flag bit and enable further interrupts pulses to be generated
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    g_PwmTimeRunning[6] = false;
}

void EPwm_RegisterISR(void)
{
    EALLOW;
    PieVectTable.EPWM1_INT = &epwm1_timer_isr;
    PieVectTable.EPWM2_INT = &epwm2_timer_isr;
    PieVectTable.EPWM3_INT = &epwm3_timer_isr;
    PieVectTable.EPWM4_INT = &epwm4_timer_isr;
    PieVectTable.EPWM5_INT = &epwm5_timer_isr;
    PieVectTable.EPWM6_INT = &epwm6_timer_isr;
    EDIS;
}

void EPwmTimerStart(Uint8 PwmIndex, Uint32 usecond)
{
    volatile struct EPWM_REGS *epwmreg = &EPwm1Regs + PwmIndex - 1;
    epwmreg->ETPS.bit.INTPRD = ET_1ST;
    epwmreg->TBPRD = (PwmIndex == 2) ? usecond * 0.0703125 : usecond * 1.125;  // 90 may change depend on pwm module clock
    epwmreg->TBCTR = 0x0000;
    epwmreg->ETSEL.bit.INTEN = PWM_INT_ENABLE;
    g_PwmTimeRunning[PwmIndex] = true;
}

bool EPwmTimerisOver(Uint8 PwmIndex)
{
    return !g_PwmTimeRunning[PwmIndex];
}

void EPwmTimerCancel(Uint8 PwmIndex)
{
    volatile struct EPWM_REGS *epwmreg = &EPwm1Regs + PwmIndex - 1;
    epwmreg->TBCTR = 0x0000;
    epwmreg->ETSEL.bit.INTEN = PWM_INT_DISABLE;
}

/*void EPwm8_Init(void)
{
    volatile struct EPWM_REGS *epwmreg;
    epwmreg = &EPwm8Regs;
    // Time-base clock synchronization close first
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    epwmreg->TBCTL.bit.PRDLD = TB_SHADOW;       // Enable shadow mode

    // Initialization Time
    // = = = = = = = = = = = = = = = = = = = = = = = =
    epwmreg->TBPRD = 1024;                  // Period = 3125 TBCLK counts
    epwmreg->CMPA.half.CMPA = 0;                // Compare A = 350 TBCLK counts
    epwmreg->CMPB = 0000;                       // Compare B = 200 TBCLK counts
    epwmreg->TBPHS.half.TBPHS = 0;          // Set Phase register to zero
    epwmreg->TBCTR = 0;                         // clear TB counter
    epwmreg->TBCTL.bit.CTRMODE = TB_COUNT_UP;
    epwmreg->TBCTL.bit.PHSEN = TB_DISABLE;  // Phase loading disabled
    epwmreg->TBCTL.bit.PRDLD = TB_SHADOW;
    epwmreg->TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    epwmreg->TBCTL.bit.HSPCLKDIV = 5;       //TB_DIV1;  // TBCLK = SYSCLKOUT / 80
    epwmreg->TBCTL.bit.CLKDIV = 3;          //TB_DIV4;
    epwmreg->CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    epwmreg->CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    epwmreg->CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on TBCTR = Zero
    epwmreg->CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on TBCTR = Zero
    epwmreg->AQCTLA.bit.ZRO = AQ_SET;
    epwmreg->AQCTLA.bit.CAU = AQ_CLEAR;
    epwmreg->AQCTLB.bit.ZRO = AQ_CLEAR;
    epwmreg->AQCTLB.bit.CBU = AQ_CLEAR;

    epwmreg->DBCTL.bit.HALFCYCLE = 0;
    epwmreg->DBCTL.bit.OUT_MODE = DB_DISABLE;//DB_FULL_ENABLE;
    epwmreg->DBCTL.bit.POLSEL = DB_ACTV_HIC;
    epwmreg->DBCTL.bit.IN_MODE = DBA_ALL;
    epwmreg->DBRED = 120;
    epwmreg->DBFED = 120;


    epwmreg->ETSEL.bit.INTSEL = ET_CTR_ZERO;
    epwmreg->ETSEL.bit.INTEN = 1;
    epwmreg->ETPS.bit.INTPRD = ET_1ST;
    //
    // Run Time
    // = = = = = = = = = = = = = = = = = = = = = = = =
    //EPwm5Regs.CMPA.half.CMPA = Duty1A; // adjust duty for output EPWM1A
    //EPwm5Regs.CMPB = Duty1B; // adjust duty for output EPWM1B

    // Time-base clock synchronization start
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}*/
