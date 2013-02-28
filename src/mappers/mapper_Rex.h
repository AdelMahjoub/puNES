/*
 * mapper_Rex.h
 *
 *  Created on: 11/set/2011
 *      Author: fhorse
 */

#ifndef MAPPER_REX_H_
#define MAPPER_REX_H_

#include "common.h"

enum { DBZ = 2 };

struct _rex_dbz {
	WORD chr_rom_bank[8];
	BYTE chr_high;
} rex_dbz;

void map_init_Rex(BYTE model);
void extcl_cpu_wr_mem_Rex_dbz(WORD address, BYTE value);
BYTE extcl_cpu_rd_mem_Rex_dbz(WORD address, BYTE openbus, BYTE before);
BYTE extcl_save_mapper_Rex_dbz(BYTE mode, BYTE slot, FILE *fp);

#endif /* MAPPER_REX_H_ */
