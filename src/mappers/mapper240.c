/*
 * mapper240.c
 *
 *  Created on: 24/mar/2012
 *      Author: fhorse
 */

#include "mappers.h"
#include "mem_map.h"

WORD prgRom32kMax, chrRom8kMax;

void map_init_240(void) {
	prgRom32kMax = (info.prg_rom_16k_count >> 1) - 1;
	chrRom8kMax = (info.chr_rom_4k_count >> 1) - 1;

	EXTCL_CPU_WR_MEM(240);

	info.mapper_extend_wr = TRUE;
}
void extcl_cpu_wr_mem_240(WORD address, BYTE value) {
	if ((address >= 0x4020) && (address < 0x6000)) {
		DBWORD bank;
		BYTE save = value;

		value >>= 4;
		control_bank(prgRom32kMax)
		map_prg_rom_8k(4, 0, value);
		map_prg_rom_8k_update();

		value = save;
		control_bank(chrRom8kMax)
		bank = value << 13;
		chr.bank_1k[0] = &chr.data[bank];
		chr.bank_1k[1] = &chr.data[bank | 0x0400];
		chr.bank_1k[2] = &chr.data[bank | 0x0800];
		chr.bank_1k[3] = &chr.data[bank | 0x0C00];
		chr.bank_1k[4] = &chr.data[bank | 0x1000];
		chr.bank_1k[5] = &chr.data[bank | 0x1400];
		chr.bank_1k[6] = &chr.data[bank | 0x1800];
		chr.bank_1k[7] = &chr.data[bank | 0x1C00];
	}
}
