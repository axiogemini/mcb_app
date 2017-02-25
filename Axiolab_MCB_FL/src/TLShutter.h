/*
 * TLShutter.h
 *
 *  Created on: 2017?2?21?
 *      Author: ZCTIAXIA
 */

#ifndef SRC_TLSHUTTER_H_
#define SRC_TLSHUTTER_H_

typedef enum {
    kShutterOn,
    kShutterOff
} eShutterDir;

typedef enum {
    kShutter_sts00,
    kShutter_sts01,
    kShutter_sts02,
    kShutter_sts03,
    kShutter_sts04,
    kShutter_sts05,
    kShutter_sts06
} eShutterStatus;

void TLShutterSet(void);
void ShutterMgr(void);

#endif /* SRC_TLSHUTTER_H_ */
