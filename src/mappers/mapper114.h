/*
 * mapper114.h
 *
 *  Created on: 8/ott/2011
 *      Author: fhorse
 */

#ifndef MAPPER114_H_
#define MAPPER114_H_

#include "common.h"

struct _m114 {
	BYTE prgRomSwitch;
	BYTE mmc3CtrlChange;
	WORD prgRomBank[4];
} m114;

void mapInit_114(void);
void extclCpuWrMem_114(WORD address, BYTE value);
BYTE extclSaveMapper_114(BYTE mode, BYTE slot, FILE *fp);

#endif /* MAPPER114_H_ */
