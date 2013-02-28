/*
 * mapper_74x161x161x32.c
 *
 *  Created on: 19/set/2011
 *      Author: fhorse
 */

#include "mappers.h"
#include "mem_map.h"

WORD prg_rom_16k_max, chr_rom_8k_max;
BYTE type;

void map_init_74x161x161x32(BYTE model) {
	prg_rom_16k_max = info.prg_rom_16k_count - 1;
	chr_rom_8k_max = info.chr_rom_8k_count - 1;

	EXTCL_CPU_WR_MEM(74x161x161x32);

	type = model;
}
void extcl_cpu_wr_mem_74x161x161x32(WORD address, BYTE value) {
	/* bus conflict */
	const BYTE save = value &= prg_rom_rd(address);
	DBWORD bank;

	if (type == IC74X161X161X32B) {
		if (value & 0x80) {
			mirroring_SCR1();
		} else {
			mirroring_SCR0();
		}
	}

	control_bank_with_AND(0x0F, chr_rom_8k_max)
	bank = value << 13;
	chr.bank_1k[0] = &chr.data[bank];
	chr.bank_1k[1] = &chr.data[bank | 0x0400];
	chr.bank_1k[2] = &chr.data[bank | 0x0800];
	chr.bank_1k[3] = &chr.data[bank | 0x0C00];
	chr.bank_1k[4] = &chr.data[bank | 0x1000];
	chr.bank_1k[5] = &chr.data[bank | 0x1400];
	chr.bank_1k[6] = &chr.data[bank | 0x1800];
	chr.bank_1k[7] = &chr.data[bank | 0x1C00];

	value = save >> 4;
	control_bank(prg_rom_16k_max)
	map_prg_rom_8k(2, 0, value);
	map_prg_rom_8k_update();
}
