/*
 * mapper57.h
 *
 *  Created on: 22/apr/2012
 *      Author: fhorse
 */

#ifndef MAPPER57_H_
#define MAPPER57_H_

#include "common.h"

struct _m57 {
	BYTE reg[2];
} m57;

void mapInit_57(void);
void extclCpuWrMem_57(WORD address, BYTE value);
BYTE extclSaveMapper_57(BYTE mode, BYTE slot, FILE *fp);

#endif /* MAPPER57_H_ */
