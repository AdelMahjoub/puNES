/*
 * mapper_Ntdec.h
 *
 *  Created on: 26/set/2011
 *      Author: fhorse
 */

#ifndef MAPPER_NTDEC_H_
#define MAPPER_NTDEC_H_

#include "common.h"

enum { ASDER, FHERO };

struct _asder {
	BYTE address;
	BYTE reg[8];
} asder;

void map_init_Ntdec(BYTE model);

void extcl_cpu_wr_mem_Ntdec_asder(WORD address, BYTE value);
BYTE extcl_save_mapper_Ntdec_asder(BYTE mode, BYTE slot, FILE *fp);

void extcl_cpu_wr_mem_Ntdec_fhero(WORD address, BYTE value);

#endif /* MAPPER_NTDEC_H_ */
