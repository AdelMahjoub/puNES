/*
 * mapper_Taito.h
 *
 *  Created on: 17/lug/2011
 *      Author: fhorse
 */

#ifndef MAPPER_TAITO_H_
#define MAPPER_TAITO_H_

#include "common.h"

enum {
	TC0190FMC,
	TC0190FMCPAL16R4,
	X1005A,
	X1005B,
	X1017,
	BAD_INES_FLINJ = 100,
	X1005_NO_BAT = 101
};

struct _taito_X1005 {
	BYTE ram[0x80];
	BYTE enable;
} taito_X1005;
struct _taito_X1017 {
	BYTE chr[6];
	BYTE control;
} taito_X1017;

void map_init_Taito(BYTE model);

void extcl_cpu_wr_mem_Taito_TC0190FMC(WORD address, BYTE value);

void extcl_cpu_wr_mem_Taito_TC0190FMCPAL16R4(WORD address, BYTE value);
void extcl_ppu_000_to_34x_Taito_TC0190FMCPAL16R4(void);
void extcl_ppu_000_to_255_Taito_TC0190FMCPAL16R4(void);
void extcl_ppu_256_to_319_Taito_TC0190FMCPAL16R4(void);
void extcl_ppu_320_to_34x_Taito_TC0190FMCPAL16R4(void);

void extcl_cpu_wr_mem_Taito_X1005(WORD address, BYTE value);
BYTE extcl_cpu_rd_mem_Taito_X1005(WORD address, BYTE openbus, BYTE before);
BYTE extcl_save_mapper_Taito_X1005(BYTE mode, BYTE slot, FILE *fp);
void extcl_battery_io_Taito_X1005(BYTE mode, FILE *fp);

void extcl_cpu_wr_mem_Taito_X1017(WORD address, BYTE value);
BYTE extcl_save_mapper_Taito_X1017(BYTE mode, BYTE slot, FILE *fp);

#endif /* MAPPER_TAITO_H_ */
