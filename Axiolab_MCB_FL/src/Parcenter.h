/*
 * Parcenter.h
 *
 *  Created on: 2016?10?10?
 *      Author: ZCTIAXIA
 */

#ifndef SRC_PARCENTER_H_
#define SRC_PARCENTER_H_

typedef struct {
	int32 X[NP_NUM];
	int32 Y[NP_NUM];
	int32 Z[NP_NUM];
} tParcenterTable;

extern tParcenterTable PcTable;

extern uint8_t PfInitiated;  // 0 0 0 0 0 x y z
extern uint8_t PfClearAll;
extern void StartParcenterSaveTask(Uint16 DescID, Uint16 DescEntry);

#endif /* SRC_PARCENTER_H_ */
