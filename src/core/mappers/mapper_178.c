/*
 * mapper_178.c
 *
 *  Created on: 6/ott/2011
 *      Author: fhorse
 */

#include <string.h>
#include "mappers.h"
#include "info.h"
#include "mem_map.h"
#include "save_slot.h"

BYTE type;

void map_init_178(BYTE model) {
	EXTCL_CPU_WR_MEM(178);
	EXTCL_SAVE_MAPPER(178);
	mapper.internal_struct[0] = (BYTE *) &m178;
	mapper.internal_struct_size[0] = sizeof(m178);

	info.mapper.extend_wr = TRUE;

	if (info.reset >= HARD) {
		memset(&m178, 0x00, sizeof(m178));
		map_prg_rom_8k(4, 0, 0);
	}

	type = model;
}
void extcl_cpu_wr_mem_178(WORD address, BYTE value) {
	switch (address) {
		case 0x4800:
			if (value & 0x01) {
				mirroring_H();
			} else {
				mirroring_V();
			}
			return;
		case 0x4801:
			value = m178.reg = (m178.reg & 0x0C) | ((value >> 1) & 0x03);
			control_bank(info.prg.rom.max.banks_32k)
			map_prg_rom_8k(4, 0, value);
			map_prg_rom_8k_update();
			return;
		case 0x4802:
			m178.reg = ((value << 2) & 0x0C) | (m178.reg & 0x03);
			return;
		case 0x8000:
			if (type == XINGJI) {
				control_bank(info.prg.rom.max.banks_32k)
				map_prg_rom_8k(4, 0, value);
				map_prg_rom_8k_update();
			}
			return;
	}
}
BYTE extcl_save_mapper_178(BYTE mode, BYTE slot, FILE *fp) {
	save_slot_ele(mode, slot, m178.reg);

	return (EXIT_OK);
}
