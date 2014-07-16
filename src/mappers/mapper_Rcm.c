/*
 * mapper_Rcm.c
 *
 *  Created on: 23/mar/2012
 *      Author: fhorse
 */

#include "mappers.h"
#include "mem_map.h"

void map_init_Rcm(BYTE type) {
	switch (type) {
		case GS2015:
			EXTCL_CPU_WR_MEM(GS2015);

			if (info.reset >= HARD) {
				map_prg_rom_8k(4, 0, 0);
			}
			break;
	}
}
void extcl_cpu_wr_mem_GS2015(WORD address, BYTE value) {
	DBWORD bank;

	value = address;
	control_bank(info.prg.rom.max.banks_32k)
	map_prg_rom_8k(4, 0, value);
	map_prg_rom_8k_update();

	value = address >> 1;
	control_bank(info.chr.rom.max.banks_8k)
	bank = value << 13;
	chr.bank_1k[0] = chr_chip_byte_pnt(0, bank);
	chr.bank_1k[1] = chr_chip_byte_pnt(0, bank | 0x0400);
	chr.bank_1k[2] = chr_chip_byte_pnt(0, bank | 0x0800);
	chr.bank_1k[3] = chr_chip_byte_pnt(0, bank | 0x0C00);
	chr.bank_1k[4] = chr_chip_byte_pnt(0, bank | 0x1000);
	chr.bank_1k[5] = chr_chip_byte_pnt(0, bank | 0x1400);
	chr.bank_1k[6] = chr_chip_byte_pnt(0, bank | 0x1800);
	chr.bank_1k[7] = chr_chip_byte_pnt(0, bank | 0x1C00);
}
