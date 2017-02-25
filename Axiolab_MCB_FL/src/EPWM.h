/*
 * PWM5.h
 *
 *  Created on: 2015Äê7ÔÂ27ÈÕ
 *      Author: zctiaxia
 */

#ifndef SOURCE_PWM5_H_
#define SOURCE_PWM5_H_

#define PWM_INT_ENABLE  1
#define PWM_INT_DISABLE 0
#define PWM1_TIMER_TBPRD   0xFFFF
#define PWM2_TIMER_TBPRD   0x1000
#define PWM3_TIMER_TBPRD   0x1000
#define PWM4_TIMER_TBPRD   0x1000
#define PWM5_TIMER_TBPRD   0x1000
#define PWM6_TIMER_TBPRD   0x1000

void EPwm_RegisterISR(void);
void EPwmTimerInit(void);
void EPwmTimerStart(Uint8 PwmIndex, Uint32 usecond);
void EPwmTimerCancel(Uint8 PwmIndex);
bool EPwmTimerisOver(Uint8 PwmIndex);

#endif /* SOURCE_PWM5_H_ */
