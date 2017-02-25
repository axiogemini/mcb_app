/*
 * Timer.h
 *
 *  Created on: 2015?4?25?
 *      Author: zctiaxia
 */

#ifndef TIMER_H_
#define TIMER_H_

#define GetSysTick0() CpuTimer0.InterruptCount


// should rework this and auto calculate the others based on the TIMERx_INTERVAL_US
#define ONE_SECOND				1000000
#define ONE_MS                  1000
#define TIMER0_INTERVAL_US		50000				// 50ms
#define TIMER1_INTERVAL_US		100000				// 100ms
#define SEC3					60
#define SEC1					20
#define SEC10					200
#define MSEC600     			12
#define MSEC100                 2

extern void Timer_RegisterIsr(void);
extern void Timer_Init(void);

#endif /* TIMER_H_ */
