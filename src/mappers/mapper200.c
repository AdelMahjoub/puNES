/*
 * mapper200.c
 *
 *  Created on: 21/mar/2012
 *      Author: fhorse
 */

#include "mappers.h"
#include "mem_map.h"

WORD prg_rom_16k_max, chr_rom_8k_max;

void map_init_200(void) {
	prg_rom_16k_max = info.prg_rom_16k_count - 1;
	chr_rom_8k_max = (info.chr_rom_4k_count >> 1) - 1;

	EXTCL_CPU_WR_MEM(200);

	if (info.reset >= HARD) {
		extcl_cpu_wr_mem_200(0x8000, 0);
	}
}
void extcl_cpu_wr_mem_200(WORD address, BYTE value) {
	BYTE save = (address & 0xFF);
	DBWORD bank;

	value = save;
	control_bank(prg_rom_16k_max)
	map_prg_rom_8k(2, 0, value);
	map_prg_rom_8k(2, 2, value);
	map_prg_rom_8k_update();

	value = save;
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

	if (address & 0x0008) {
		mirroring_H();
	} else {
		mirroring_V();
	}
}
