/*
 * mapper178.h
 *
 *  Created on: 6/ott/2011
 *      Author: fhorse
 */

#ifndef MAPPER178_H_
#define MAPPER178_H_

#include "common.h"

enum {
	M178 = 2,
	XINGJI = 3
};

struct _m178 {
	BYTE reg;
} m178;

void mapInit_178(BYTE model);
void extcl_cpu_wr_mem_178(WORD address, BYTE value);
BYTE extcl_save_mapper_178(BYTE mode, BYTE slot, FILE *fp);

#endif /* MAPPER178_H_ */
