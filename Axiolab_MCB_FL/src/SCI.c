/*
 * SCI.c
 *
 *  Created on: 2016��3��13��
 *      Author: ZCTIAXIA
 */

#include "include.h"

void SCI_Init(void)
{
	UARTStdioInit(0);
	UARTprintf("\033[2JBulk device application\n");
}
