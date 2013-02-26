/*
 * mapperTxROM.c
 *
 *  Created on: 28/set/2011
 *      Author: fhorse
 */

#include <stdlib.h>
#include <string.h>
#include "mappers.h"
#include "mem_map.h"
#include "irqA12.h"
#include "save_slot.h"

#define tqrom8000SwapChrRam1k(slot0, slot1)\
{\
	uint32_t save[1][2];\
	save[0][0] = txrom.chr[slot0][0];\
	save[0][1] = txrom.chr[slot0][1];\
	txrom.chr[slot0][0] = txrom.chr[slot1][0];\
	txrom.chr[slot0][1] = txrom.chr[slot1][1];\
	txrom.chr[slot1][0] = save[0][0];\
	txrom.chr[slot1][1] = save[0][1];\
}
#define tqrom8001SwapChr2k(slot0, slot1)\
{\
	const BYTE a = slot0;\
	const BYTE b = slot1;\
	if (value & 0x40) {\
		txrom.chr[a][0] = txrom.chr[b][0] = TRUE;\
		value >>= 1;\
		control_bank(3)\
		txrom.chr[a][1] = value << 11;\
		txrom.chr[b][1] = txrom.chr[a][1] | 0x400;\
		chr.bank_1k[a] = &txrom.chrRam[txrom.chr[a][1]];\
		chr.bank_1k[b] = &txrom.chrRam[txrom.chr[b][1]];\
		return;\
	} else {\
		txrom.chr[a][0] = txrom.chr[b][0] = FALSE;\
		txrom.chr[a][1] = txrom.chr[b][1] = FALSE;\
	}\
}
#define tqrom8001SwapChr1k(slot0)\
{\
	const BYTE a = slot0;\
	if (value & 0x40) {\
		txrom.chr[a][0] = TRUE;\
		control_bank(7)\
		txrom.chr[a][1] = value << 10;\
		chr.bank_1k[a] = &txrom.chrRam[txrom.chr[a][1]];\
		return;\
	} else {\
		txrom.chr[a][0] = txrom.chr[a][1] = FALSE;\
	}\
}

WORD prgRom8kMax, prgRom8kBeforeLast, chrRom1kMax;
BYTE type;

void map_init_TxROM(BYTE model) {
	prgRom8kMax = info.prg_rom_8k_count - 1;
	chrRom1kMax = info.chr_rom_1k_count - 1;
	prgRom8kBeforeLast = info.prg_rom_8k_count - 2;

	switch (model) {
		case TLSROM:
		case TKSROM:
			EXTCL_CPU_WR_MEM(TKSROM);

			irqA12_delay = 1;

			if (model == TKSROM) {
				info.prg_ram_plus_8k_count = 1;
				info.prg_ram_bat_banks = 1;
			}
			break;
		case TQROM:
			EXTCL_CPU_WR_MEM(TQROM);
			EXTCL_WR_CHR(TQROM);

			mapper.write_vram = FALSE;

			irqA12_delay = 1;

			break;
	}

	EXTCL_SAVE_MAPPER(TxROM);
	EXTCL_CPU_EVERY_CYCLE(MMC3);
	EXTCL_PPU_000_TO_34X(MMC3);
	EXTCL_PPU_000_TO_255(MMC3);
	EXTCL_PPU_256_TO_319(MMC3);
	EXTCL_PPU_320_TO_34X(MMC3);
	EXTCL_UPDATE_R2006(MMC3);
	mapper.internal_struct[0] = (BYTE *) &txrom;
	mapper.internal_struct_size[0] = sizeof(txrom);
	mapper.internal_struct[1] = (BYTE *) &mmc3;
	mapper.internal_struct_size[1] = sizeof(mmc3);

	if (info.reset >= HARD) {
		memset(&txrom.chr, 0x00, sizeof(txrom.chr));
		memset(&txrom.chrRam, 0x00, sizeof(txrom.chrRam));
		memset(&mmc3, 0x00, sizeof(mmc3));
	}

	memset(&irqA12, 0x00, sizeof(irqA12));
	txrom.delay = 0;

	irqA12.present = TRUE;

	type = model;
}

void extcl_cpu_wr_mem_TKSROM(WORD address, BYTE value) {
	switch (address & 0xE001) {
		case 0x8001: {
			switch (mmc3.bankToUpdate) {
				case 0:
				case 1:
					if (!mmc3.chrRomCfg) {
						const BYTE slot = mmc3.bankToUpdate << 1;
						ntbl.bank_1k[slot] = &ntbl.data[((value >> 7) ^ 0x01) << 10];
						ntbl.bank_1k[slot | 0x01] = ntbl.bank_1k[slot];
					}
					break;
				case 2:
				case 3:
				case 4:
				case 5:
					if (mmc3.chrRomCfg) {
						ntbl.bank_1k[mmc3.bankToUpdate - 2] =
								&ntbl.data[((value >> 7) ^ 0x01) << 10];
					}
					break;
			}
			break;
		}
		case 0xA000:
			return;
	}
	extcl_cpu_wr_mem_MMC3(address, value);
}

void extcl_cpu_wr_mem_TQROM(WORD address, BYTE value) {
	const WORD adr = address & 0xE001;
	if (adr == 0x8000) {
		if (mmc3.chrRomCfg != ((value & 0x80) >> 5)) {
			tqrom8000SwapChrRam1k(0, 4)
			tqrom8000SwapChrRam1k(1, 5)
			tqrom8000SwapChrRam1k(2, 6)
			tqrom8000SwapChrRam1k(3, 7)
		}
	} else if (adr == 0x8001) {
		switch (mmc3.bankToUpdate) {
			case 0:
				tqrom8001SwapChr2k(mmc3.chrRomCfg, mmc3.chrRomCfg | 0x01)
				break;
			case 1:
				tqrom8001SwapChr2k(mmc3.chrRomCfg | 0x02, mmc3.chrRomCfg | 0x03)
				break;
			case 2:
				tqrom8001SwapChr1k(mmc3.chrRomCfg ^ 0x04)
				break;
			case 3:
				tqrom8001SwapChr1k((mmc3.chrRomCfg ^ 0x04) | 0x01)
				break;
			case 4:
				tqrom8001SwapChr1k((mmc3.chrRomCfg ^ 0x04) | 0x02)
				break;
			case 5:
				tqrom8001SwapChr1k((mmc3.chrRomCfg ^ 0x04) | 0x03)
				break;
		}
	}
	extcl_cpu_wr_mem_MMC3(address, value);
}
void extcl_wr_chr_TQROM(WORD address, BYTE value) {
	const BYTE slot = address >> 10;

	if (txrom.chr[slot]) {
		chr.bank_1k[slot][address & 0x3FF] = value;
	}
}

BYTE extcl_save_mapper_TxROM(BYTE mode, BYTE slot, FILE *fp) {
	save_slot_ele(mode, slot, txrom.delay);
	if (type == TQROM) {
		save_slot_ele(mode, slot, txrom.chr);
		if (mode == SAVE_SLOT_READ) {
			BYTE i;
			for (i = 0; i < LENGTH(txrom.chr); i++) {
				if (txrom.chr[i][0]) {
					chr.bank_1k[i] = &txrom.chrRam[txrom.chr[i][1]];
				}
			}
		}
		save_slot_ele(mode, slot, txrom.chrRam);
	}
	extcl_save_mapper_MMC3(mode, slot, fp);

	return (EXIT_OK);
}
