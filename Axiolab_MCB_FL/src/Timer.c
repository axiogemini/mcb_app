/*
 * Timer.c
 *
 *  Created on: 2015?4?25?
 *      Author: zctiaxia
 */


#include "include.h"


// Prototype statements for functions found within this file.
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);


//---------------------------------------------------------------------------
// Timer_RegisterIsr:
//---------------------------------------------------------------------------
// This function register ISR for timer 0 1 2
//
void Timer_RegisterIsr(void)
{
	// Interrupts that are used in this example are re-mapped to
	// ISR functions found within this file.
	EALLOW;  // This is needed to write to EALLOW protected registers
	PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.TINT1 = &cpu_timer1_isr;
	//PieVectTable.TINT2 = &cpu_timer2_isr;
	EDIS;    // This is needed to disable write to EALLOW protected registers
}

//---------------------------------------------------------------------------
// Timer_Init:
//---------------------------------------------------------------------------
// This function setup period of timer and start all timer
//
void Timer_Init(void)
{
	// Configure CPU-Timer 0, 1, and 2 to interrupt every second:
	// 90MHz CPU Freq, 1 second Period (in uSeconds)

	ConfigCpuTimer(&CpuTimer0, 90, TIMER0_INTERVAL_US);  // 0.1s
	ConfigCpuTimer(&CpuTimer1, 90, TIMER1_INTERVAL_US);
	//ConfigCpuTimer(&CpuTimer2, 90, 1);

	// To ensure precise timing, use write-only instructions to write to the entire register. Therefore, if any
	// of the configuration bits are changed in ConfigCpuTimer and InitCpuTimers (in F2806x_CpuTimers.h), the
	// below settings must also be updated.

	CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
	CpuTimer1Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
	//CpuTimer2Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
}


__interrupt void cpu_timer0_isr(void)
{
	CpuTimer0.InterruptCount++;

	GpioDataRegs.GPATOGGLE.bit.GPIO2 = 1;
	//GpioDataRegs.GPBTOGGLE.bit.GPIO56 = 1;



	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void cpu_timer1_isr(void)
{
	CpuTimer1.InterruptCount++;
	// The CPU acknowledges the interrupt.

	EDIS;
}

/*
__interrupt void cpu_timer2_isr(void)
{
	EALLOW;
	CpuTimer2.InterruptCount++;
	// The CPU acknowledges the interrupt.
	EDIS;
}*/
