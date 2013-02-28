/*
 * mapper52.h
 *
 *  Created on: 21/apr/2012
 *      Author: fhorse
 */

#ifndef MAPPER52_H_
#define MAPPER52_H_

#include "common.h"

struct _m52 {
	BYTE disabled;
	BYTE reg;
	WORD prg_map[4];
	WORD chr_map[8];
} m52;

void map_init_52(void);
void extcl_cpu_wr_mem_52(WORD address, BYTE value);
BYTE extcl_save_mapper_52(BYTE mode, BYTE slot, FILE *fp);

#endif /* MAPPER52_H_ */
