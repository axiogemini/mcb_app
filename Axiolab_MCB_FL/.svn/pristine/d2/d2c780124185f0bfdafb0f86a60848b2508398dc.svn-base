/*
 * Adc.c
 *
 *  Created on: 2016Äê11ÔÂ8ÈÕ
 *      Author: zctiaxia
 */

#include "include.h"

void
ADC_Init(void)
{
    InitAdc();
    AdcOffsetSelfCal();

	EALLOW;
    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1;	//Enable non-overlap mode
    AdcRegs.ADCCTL1.bit.TEMPCONV  = 0; 		//DISconnect channel A5 internally to the temperature sensor

    AdcRegs.ADCSOC0CTL.bit.CHSEL  = 0x01; 		//Set SOC0 channel select to ADCINA1  -- for bulbmon
    AdcRegs.ADCSOC0CTL.bit.ACQPS  = 25; 		//Set SOC0 acquisition period to 26 ADCCLK
    AdcRegs.INTSEL1N2.bit.INT1SEL = 0; 		//Connect ADCINT1 to EOC0
    AdcRegs.INTSEL1N2.bit.INT1E  =  1; 		//Enable ADCINT1

    AdcRegs.ADCSOC1CTL.bit.CHSEL  = 0x0a; 		//Set SOC1 channel select to ADCINB2 -- for ledvf
    AdcRegs.ADCSOC1CTL.bit.ACQPS  = 25; 		//Set SOC1 acquisition period to 26 ADCCLK
    AdcRegs.INTSEL1N2.bit.INT2SEL = 1; 		//Connect ADCINT2 to EOC1
    AdcRegs.INTSEL1N2.bit.INT2E  =  1; 		//Enable ADCINT2

 // Set the flash OTP wait-states to minimum. This is important
 // for the performance of the temperature conversion function.
    FlashRegs.FOTPWAIT.bit.OTPWAIT = 1;

    EDIS;
}

static int
ADC_SampleOnce(uint8_t SocNum) // Num from 0 ~ 15
{
    int value = 0;
    int socbit = 1 << SocNum;
    volatile Uint16 *p_Result = &AdcResult.ADCRESULT0;

	AdcRegs.ADCSOCFRC1.all = socbit;

    //Wait for end of conversion.
    while((AdcRegs.ADCINTFLG.all & socbit) == 0){}  //Wait for ADCINT1
    AdcRegs.ADCINTFLGCLR.all &= socbit;        //Clear ADCINT1

    //Get temp sensor sample result from SOC0
    value = *(p_Result + SocNum);
    return value;
}

float
ReadBulbOut(void)
{
    return (float)ADC_SampleOnce(0) /4095 *3.3 *15.3l / 3.3l;
}

float
ReadLedVoltage(void)
{
    return (float)ADC_SampleOnce(1) / 4095 * 3.3 * 15.3l / 3.3l;
}














