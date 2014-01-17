/*
 * mapper_244.c
 *
 *  Created on: 24/mar/2012
 *      Author: fhorse
 */

#include <stdio.h>

#include "mappers.h"
#include "mem_map.h"

WORD prg_rom_32k_max, chr_rom_8k_max;

void map_init_244(void) {
	prg_rom_32k_max = (info.prg.rom.banks_16k >> 1) - 1;
	chr_rom_8k_max = (info.chr.rom.banks_4k >> 1) - 1;

	EXTCL_CPU_WR_MEM(244);

	if (info.reset >= HARD) {
		map_prg_rom_8k(4, 0, 0);
	}
}
void extcl_cpu_wr_mem_244(WORD address, BYTE value) {
	if ((address >= 0x8065) && (address <= 0x80A4)) {
		value = (address - 0x8065) & 0x03;
		control_bank(prg_rom_32k_max)
		map_prg_rom_8k(4, 0, value);
		map_prg_rom_8k_update();
	};
	if ((address >= 0x80A5) && (address <= 0x80E4)) {
		DBWORD bank;

		value = (address - 0x80A5) & 0x07;
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
	};
}
