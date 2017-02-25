/*
 * CmdHandler.h
 *
 *  Created on: 2017?1?10?
 *      Author: zctiaxia
 */

#ifndef SRC_CMDHANDLER_H_
#define SRC_CMDHANDLER_H_

typedef struct {
    uint8_t Target;
    uint8_t Src;
    uint8_t DataLen;
    uint8_t CmdCl;
    uint8_t CmdNum;
    uint8_t Buffer[255]; // begin from pid, subid, devid....
} MsgFrame;

void DLD_Handler(uint8_t *pFrame);

#endif /* SRC_CMDHANDLER_H_ */
