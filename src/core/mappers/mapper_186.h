/*
 * mapper_186.h
 *
 *  Created on: 10/ott/2011
 *      Author: fhorse
 */

#ifndef MAPPER_186_H_
#define MAPPER_186_H_

#include "common.h"

struct _m186 {
	BYTE *prg_ram_bank2;
} m186;

void map_init_186(void);
void extcl_cpu_wr_mem_186(WORD address, BYTE value);
BYTE extcl_cpu_rd_mem_186(WORD address, BYTE openbus, BYTE before);
BYTE extcl_save_mapper_186(BYTE mode, BYTE slot, FILE *fp);

#endif /* MAPPER_186_H_ */
