/*
 * mapper_74x138x161.c
 *
 *  Created on: 16/set/2011
 *      Author: fhorse
 */

#include "mappers.h"
#include "mem_map.h"

WORD prg_rom_32k_max, chr_rom_8k_max;

void map_init_74x138x161(void) {
	prg_rom_32k_max = (info.prg_rom_16k_count >> 1) - 1;
	chr_rom_8k_max = info.chr_rom_8k_count - 1;

	EXTCL_CPU_WR_MEM(74x138x161);

	info.mapper_extend_wr = TRUE;

	if (info.reset >= HARD) {
		map_prg_rom_8k(4, 0, 0);
	}
}
void extcl_cpu_wr_mem_74x138x161(WORD address, BYTE value) {
	const BYTE save = value;
	DBWORD bank;

	if (address >= 0x8000) {
		return;
	}

	control_bank_with_AND(0x03, prg_rom_32k_max)
	map_prg_rom_8k(4, 0, value);
	map_prg_rom_8k_update();

	value = save >> 2;
	control_bank(chr_rom_8k_max)
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
