/*
 * mapperActive.c
 *
 *  Created on: 02/feb/2011
 *      Author: fhorse
 */

#include <stdlib.h>
#include <string.h>
#include "mappers.h"
#include "memmap.h"
#include "savestate.h"

WORD prgRom32kMax, prgRom16kMax, chrRom8kMax;

void map_init_Active(void) {
	prgRom32kMax = (info.prg_rom_16k_count >> 1) - 1;
	prgRom16kMax = info.prg_rom_16k_count - 1;
	chrRom8kMax = info.chr_rom_8k_count - 1;

	EXTCL_CPU_WR_MEM(Active);
	EXTCL_CPU_RD_MEM(Active);
	EXTCL_SAVE_MAPPER(Active);
	mapper.internal_struct[0] = (BYTE *) &active;
	mapper.internal_struct_size[0] = sizeof(active);

	info.mapper_extend_wr = TRUE;

	if (info.reset >= HARD) {
		map_prg_rom_8k(4, 0, 0);
		memset(&active, 0x00, sizeof(active));
	} else {
		active.openbus = FALSE;
	}
}
void extcl_cpu_wr_mem_Active(WORD address, BYTE value) {
	BYTE save = value, prgChip = address >> 10;
	DBWORD bank;

	if ((address < 0x6000) && (address >= 0x4020)) {
		active.prgRam[address & 0x0003] = value & 0x0F;
		return;
	}

	if (prgChip == 2) {
		active.openbus = TRUE;
	} else {
		active.openbus = FALSE;

		if (prgChip == 3) {
			value = (address >> 6) & 0x5F;
		} else {
			value = (address >> 6) & 0x7F;
		}

		if (address & 0x0020) {
			control_bank(prgRom16kMax)
			map_prg_rom_8k(2, 0, value);
			map_prg_rom_8k(2, 2, value);
		} else {
			value >>= 1;
			control_bank(prgRom32kMax)
			map_prg_rom_8k(4, 0, value);
		}
		map_prg_rom_8k_update();
	}


	if (address & 0x2000) {
		mirroring_H();
	} else {
		mirroring_V();
	}

	value = ((address << 2) & 0x3C) | (save & 0x03);
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
BYTE extcl_cpu_rd_mem_Active(WORD address, BYTE openbus, BYTE before) {
	if ((address >= 0x4020) && (address < 0x6000)) {
		return (active.prgRam[address & 0x0003]);
	}

	return (openbus);
}
BYTE extcl_save_mapper_Active(BYTE mode, BYTE slot, FILE *fp) {
	savestateEle(mode, slot, active.openbus);
	savestateEle(mode, slot, active.prgRam);

	return (EXIT_OK);
}
