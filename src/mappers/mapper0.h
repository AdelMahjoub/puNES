/*
 * mapper0.h
 *
 *  Created on: 09/mag/2010
 *      Author: fhorse
 */

#ifndef MAPPER0_H_
#define MAPPER0_H_

#include "common.h"

enum  { UNKHORIZONTAL, UNKVERTICAL };

void map_init_0(void);
void extcl_cpu_wr_mem_0(WORD address, BYTE value);
BYTE extcl_cpu_rd_mem_0(WORD address, BYTE openbus, BYTE before);

#endif /* MAPPER0_H_ */
