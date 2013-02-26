/*
 * mapperWaixing.c
 *
 *  Created on: 11/set/2011
 *      Author: fhorse
 */

#include <stdlib.h>
#include <string.h>
#include "mappers.h"
#include "memmap.h"
#include "irqA12.h"
#include "save_slot.h"

WORD prgRom16kMax, prgRom8kBeforeLast, prgRom8kMax, chrRom4kMax, chrRom1kMax;
BYTE min, max;

#define waixing_swapChrBank1k(src, dst)\
{\
	BYTE *chrBank1k = chr.bank_1k[src];\
	chr.bank_1k[src] = chr.bank_1k[dst];\
	chr.bank_1k[dst] = chrBank1k;\
	WORD map = waixing.chrmap[src];\
	waixing.chrmap[src] = waixing.chrmap[dst];\
	waixing.chrmap[dst] = map;\
}
#define waixing_8000(function)\
{\
	const BYTE chrRomCfgOld = mmc3.chrRomCfg;\
	const BYTE prgRomCfgOld = mmc3.prgRomCfg;\
	function;\
	mmc3.prgRomCfg = (value & 0x40) >> 5;\
	mmc3.chrRomCfg = (value & 0x80) >> 5;\
	/*\
	 * se il tipo di configurazione della chr cambia,\
	 * devo swappare i primi 4 banchi con i secondi\
	 * quattro.\
	 */\
	if (mmc3.chrRomCfg != chrRomCfgOld) {\
		waixing_swapChrBank1k(0, 4)\
		waixing_swapChrBank1k(1, 5)\
		waixing_swapChrBank1k(2, 6)\
		waixing_swapChrBank1k(3, 7)\
	}\
	if (mmc3.prgRomCfg != prgRomCfgOld) {\
		WORD p0 = mapper.rom_map_to[0];\
		WORD p2 = mapper.rom_map_to[2];\
		mapper.rom_map_to[0] = p2;\
		mapper.rom_map_to[2] = p0;\
		/*\
		 * prgRomCfg 0x00 : $C000 - $DFFF fisso al penultimo banco\
		 * prgRomCfg 0x02 : $8000 - $9FFF fisso al penultimo banco\
		 */\
		map_prg_rom_8k(1, mmc3.prgRomCfg ^ 0x02, prgRom8kBeforeLast);\
		map_prg_rom_8k_update();\
	}\
}

#define Waixing_TypeACDE_chr1k(a)\
	if ((value >= min) && (value <= max)) {\
		chr.bank_1k[a] = &waixing.chrRam[(value - min) << 10];\
	} else {\
		chr.bank_1k[a] = &chr.data[value << 10];\
	}
#define Waixing_TypeACDE_8001()\
{\
	switch (mmc3.bankToUpdate) {\
		case 0:\
			waixing.chrmap[mmc3.chrRomCfg] = value;\
			waixing.chrmap[mmc3.chrRomCfg | 0x01] = value + 1;\
			control_bank_with_AND(0xFE, chrRom1kMax)\
			Waixing_TypeACDE_chr1k(mmc3.chrRomCfg)\
			value++;\
			Waixing_TypeACDE_chr1k(mmc3.chrRomCfg | 0x01)\
			return;\
		case 1:\
			waixing.chrmap[mmc3.chrRomCfg | 0x02] = value;\
			waixing.chrmap[mmc3.chrRomCfg | 0x03] = value + 1;\
			control_bank_with_AND(0xFE, chrRom1kMax)\
			Waixing_TypeACDE_chr1k(mmc3.chrRomCfg | 0x02)\
			value++;\
			Waixing_TypeACDE_chr1k(mmc3.chrRomCfg | 0x03)\
			return;\
		case 2:\
			waixing.chrmap[mmc3.chrRomCfg ^ 0x04] = value;\
			control_bank(chrRom1kMax)\
			Waixing_TypeACDE_chr1k(mmc3.chrRomCfg ^ 0x04)\
			return;\
		case 3:\
			waixing.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x01] = value;\
			control_bank(chrRom1kMax)\
			Waixing_TypeACDE_chr1k((mmc3.chrRomCfg ^ 0x04) | 0x01)\
			return;\
		case 4:\
			waixing.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x02] = value;\
			control_bank(chrRom1kMax)\
			Waixing_TypeACDE_chr1k((mmc3.chrRomCfg ^ 0x04) | 0x02)\
			return;\
		case 5:\
			waixing.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x03] = value;\
			control_bank(chrRom1kMax)\
			Waixing_TypeACDE_chr1k((mmc3.chrRomCfg ^ 0x04) | 0x03)\
			return;\
	}\
}

#define Waixing_TypeB_chr1k(a)\
	if (save & 0x80) {\
		chr.bank_1k[a] = &waixing.chrRam[value << 10];\
	} else {\
		chr.bank_1k[a] = &chr.data[value << 10];\
	}
#define Waixing_TypeB_8001()\
{\
	switch (mmc3.bankToUpdate) {\
		case 0:\
			waixing.chrmap[mmc3.chrRomCfg] = save;\
			waixing.chrmap[mmc3.chrRomCfg | 0x01] = save + 1;\
			control_bank_with_AND(0xFE, chrRom1kMax)\
			Waixing_TypeB_chr1k(mmc3.chrRomCfg)\
			value++;\
			Waixing_TypeB_chr1k(mmc3.chrRomCfg | 0x01)\
			return;\
		case 1:\
			waixing.chrmap[mmc3.chrRomCfg | 0x02] = save;\
			waixing.chrmap[mmc3.chrRomCfg | 0x03] = save + 1;\
			control_bank_with_AND(0xFE, chrRom1kMax)\
			Waixing_TypeB_chr1k(mmc3.chrRomCfg | 0x02)\
			value++;\
			Waixing_TypeB_chr1k(mmc3.chrRomCfg | 0x03)\
			return;\
		case 2:\
			waixing.chrmap[mmc3.chrRomCfg ^ 0x04] = save;\
			control_bank(chrRom1kMax)\
			Waixing_TypeB_chr1k(mmc3.chrRomCfg ^ 0x04)\
			return;\
		case 3:\
			waixing.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x01] = save;\
			control_bank(chrRom1kMax)\
			Waixing_TypeB_chr1k((mmc3.chrRomCfg ^ 0x04) | 0x01)\
			return;\
		case 4:\
			waixing.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x02] = save;\
			control_bank(chrRom1kMax)\
			Waixing_TypeB_chr1k((mmc3.chrRomCfg ^ 0x04) | 0x02)\
			return;\
		case 5:\
			waixing.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x03] = save;\
			control_bank(chrRom1kMax)\
			Waixing_TypeB_chr1k((mmc3.chrRomCfg ^ 0x04) | 0x03)\
			return;\
	}\
}

#define Waixing_TypeG_chr1k(a)\
	if (save < 8) {\
		chr.bank_1k[a] = &waixing.chrRam[save << 10];\
	} else {\
		chr.bank_1k[a] = &chr.data[value << 10];\
	}
#define Waixing_TypeG_8001()\
{\
	switch (mmc3.bankToUpdate) {\
		case 0:\
			waixing.chrmap[0] = value;\
			control_bank(chrRom1kMax)\
			Waixing_TypeG_chr1k(0)\
			return;\
		case 1:\
			waixing.chrmap[2] = value;\
			control_bank(chrRom1kMax)\
			Waixing_TypeG_chr1k(2)\
			return;\
		case 2:\
			waixing.chrmap[4] = value;\
			control_bank(chrRom1kMax)\
			Waixing_TypeG_chr1k(4)\
			return;\
		case 3:\
			waixing.chrmap[5] = value;\
			control_bank(chrRom1kMax)\
			Waixing_TypeG_chr1k(5)\
			return;\
		case 4:\
			waixing.chrmap[6] = value;\
			control_bank(chrRom1kMax)\
			Waixing_TypeG_chr1k(6)\
			return;\
		case 5:\
			waixing.chrmap[7] = value;\
			control_bank(chrRom1kMax)\
			Waixing_TypeG_chr1k(7)\
			return;\
		case 6:\
			control_bank(prgRom8kMax)\
			map_prg_rom_8k(1, 0, value);\
			map_prg_rom_8k_update();\
			return;\
		case 7:\
			control_bank(prgRom8kMax)\
			map_prg_rom_8k(1, 1, value);\
			map_prg_rom_8k_update();\
			return;\
		case 8:\
			control_bank(prgRom8kMax)\
			map_prg_rom_8k(1, 2, value);\
			map_prg_rom_8k_update();\
			return;\
		case 9:\
			control_bank(prgRom8kMax)\
			map_prg_rom_8k(1, 3, value);\
			map_prg_rom_8k_update();\
			return;\
		case 10:\
			waixing.chrmap[1] = value;\
			control_bank(chrRom1kMax)\
			Waixing_TypeG_chr1k(1)\
			return;\
		case 11:\
			waixing.chrmap[3] = value;\
			control_bank(chrRom1kMax)\
			Waixing_TypeG_chr1k(3)\
			return;\
	}\
}

#define Waixing_TypeH_chr1k(a)\
	if (mapper.write_vram) {\
		chr.bank_1k[a] = &waixing.chrRam[(value & 0x07) << 10];\
	} else {\
		control_bank(chrRom1kMax)\
		chr.bank_1k[a] = &chr.data[value << 10];\
	}
#define Waixing_TypeH_prg8k(vl)\
	value = (vl & 0x3F) | ((waixing.ctrl[0] & 0x02) << 5)
#define Waixing_TypeH_prg8kupdate()\
{\
	BYTE i;\
	for (i = 0; i < 4; i++) {\
		Waixing_TypeH_prg8k(waixing.prgmap[i]);\
		control_bank(prgRom8kMax)\
		map_prg_rom_8k(1, i, value);\
	}\
	map_prg_rom_8k_update();\
}
#define Waixing_TypeH_8000()\
{\
	const BYTE chrRomCfgOld = mmc3.chrRomCfg;\
	const BYTE prgRomCfgOld = mmc3.prgRomCfg;\
	mmc3.bankToUpdate = value & 0x07;\
	mmc3.prgRomCfg = (value & 0x40) >> 5;\
	mmc3.chrRomCfg = (value & 0x80) >> 5;\
	/*\
	 * se il tipo di configurazione della chr cambia,\
	 * devo swappare i primi 4 banchi con i secondi\
	 * quattro.\
	 */\
	if (mmc3.chrRomCfg != chrRomCfgOld) {\
		waixing_swapChrBank1k(0, 4)\
		waixing_swapChrBank1k(1, 5)\
		waixing_swapChrBank1k(2, 6)\
		waixing_swapChrBank1k(3, 7)\
	}\
	if (mmc3.prgRomCfg != prgRomCfgOld) {\
		WORD p0 = mapper.rom_map_to[0];\
		WORD p2 = mapper.rom_map_to[2];\
		mapper.rom_map_to[0] = p2;\
		mapper.rom_map_to[2] = p0;\
		p0 = waixing.prgmap[0];\
		p2 = waixing.prgmap[2];\
		waixing.prgmap[0] = p2;\
		waixing.prgmap[2] = p0;\
		waixing.prgmap[mmc3.prgRomCfg ^ 0x02] = prgRom8kBeforeLast;\
		/*\
		 * prgRomCfg 0x00 : $C000 - $DFFF fisso al penultimo banco\
		 * prgRomCfg 0x02 : $8000 - $9FFF fisso al penultimo banco\
		 */\
		Waixing_TypeH_prg8k(prgRom8kBeforeLast);\
		control_bank(prgRom8kMax)\
		map_prg_rom_8k(1, mmc3.prgRomCfg ^ 0x02, value);\
		map_prg_rom_8k_update();\
	}\
}
#define Waixing_TypeH_8001()\
{\
	switch (mmc3.bankToUpdate) {\
		case 0:\
			value &= 0xFE;\
			waixing.chrmap[mmc3.chrRomCfg] = value;\
			waixing.chrmap[mmc3.chrRomCfg | 0x01] = value + 1;\
			Waixing_TypeH_chr1k(mmc3.chrRomCfg)\
			value++;\
			Waixing_TypeH_chr1k(mmc3.chrRomCfg | 0x01)\
			if (mapper.write_vram) {\
				waixing.ctrl[0] = save;\
				Waixing_TypeH_prg8kupdate()\
			}\
			return;\
		case 1:\
			value &= 0xFE;\
			waixing.chrmap[mmc3.chrRomCfg | 0x02] = value;\
			waixing.chrmap[mmc3.chrRomCfg | 0x03] = value + 1;\
			Waixing_TypeH_chr1k(mmc3.chrRomCfg | 0x02)\
			value++;\
			Waixing_TypeH_chr1k(mmc3.chrRomCfg | 0x03)\
			Waixing_TypeH_prg8kupdate()\
			return;\
		case 2:\
			waixing.chrmap[mmc3.chrRomCfg ^ 0x04] = value;\
			Waixing_TypeH_chr1k(mmc3.chrRomCfg ^ 0x04)\
			Waixing_TypeH_prg8kupdate()\
			return;\
		case 3:\
			waixing.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x01] = value;\
			Waixing_TypeH_chr1k((mmc3.chrRomCfg ^ 0x04) | 0x01)\
			Waixing_TypeH_prg8kupdate()\
			return;\
		case 4:\
			waixing.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x02] = value;\
			Waixing_TypeH_chr1k((mmc3.chrRomCfg ^ 0x04) | 0x02)\
			Waixing_TypeH_prg8kupdate()\
			return;\
		case 5:\
			waixing.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x03] = value;\
			Waixing_TypeH_chr1k((mmc3.chrRomCfg ^ 0x04) | 0x03)\
			Waixing_TypeH_prg8kupdate()\
			return;\
		case 6:\
			waixing.prgmap[mmc3.prgRomCfg] = value;\
			Waixing_TypeH_prg8k(value);\
			control_bank(prgRom8kMax)\
			map_prg_rom_8k(1, mmc3.prgRomCfg, value);\
			map_prg_rom_8k_update();\
			return;\
		case 7:\
			waixing.prgmap[1] = value;\
			Waixing_TypeH_prg8k(value);\
			control_bank(prgRom8kMax)\
			map_prg_rom_8k(1, 1, value);\
			map_prg_rom_8k_update();\
			return;\
	}\
}

#define Waixing_SH2_chr4k(a, v)\
{\
	if (!v) {\
		waixing.ctrl[a >> 2] = 0;\
		chr.bank_1k[a] = &waixing.chrRam[0];\
		chr.bank_1k[a | 0x01] = &waixing.chrRam[0x0400];\
		chr.bank_1k[a | 0x02] = &waixing.chrRam[0x0800];\
		chr.bank_1k[a | 0x03] = &waixing.chrRam[0x0C00];\
	} else {\
		DBWORD bank = v << 12;\
		waixing.ctrl[a >> 2] = 1;\
		chr.bank_1k[a] = &chr.data[bank];\
		chr.bank_1k[a | 0x01] = &chr.data[bank | 0x0400];\
		chr.bank_1k[a | 0x02] = &chr.data[bank | 0x0800];\
		chr.bank_1k[a | 0x03] = &chr.data[bank | 0x0C00];\
	}\
}
#define Waixing_SH2_PPUFD()\
	if (waixing.reg == 0xFD) {\
		Waixing_SH2_chr4k(0, waixing.chrmap[0])\
		Waixing_SH2_chr4k(4, waixing.chrmap[2])\
	}
#define Waixing_SH2_PPUFE()\
	if (waixing.reg == 0xFE) {\
		Waixing_SH2_chr4k(0, waixing.chrmap[1])\
		Waixing_SH2_chr4k(4, waixing.chrmap[4])\
	}
#define Waixing_SH2_PPU(a)\
	if ((a & 0x1FF0) == 0x1FD0) {\
		waixing.reg = 0xFD;\
		Waixing_SH2_PPUFD()\
	} else if ((a & 0x1FF0) == 0x1FE0) {\
		waixing.reg = 0xFE;\
		Waixing_SH2_PPUFE()\
	}
#define Waixing_SH2_8000()\
{\
	const BYTE prgRomCfgOld = mmc3.prgRomCfg;\
	mmc3.bankToUpdate = value & 0x07;\
	mmc3.prgRomCfg = (value & 0x40) >> 5;\
	mmc3.chrRomCfg = (value & 0x80) >> 5;\
	if (mmc3.prgRomCfg != prgRomCfgOld) {\
		WORD p0 = mapper.rom_map_to[0];\
		WORD p2 = mapper.rom_map_to[2];\
		mapper.rom_map_to[0] = p2;\
		mapper.rom_map_to[2] = p0;\
		/*\
		 * prgRomCfg 0x00 : $C000 - $DFFF fisso al penultimo banco\
		 * prgRomCfg 0x02 : $8000 - $9FFF fisso al penultimo banco\
		 */\
		map_prg_rom_8k(1, mmc3.prgRomCfg ^ 0x02, prgRom8kBeforeLast);\
		map_prg_rom_8k_update();\
	}\
}
#define Waixing_SH2_8001()\
{\
	switch (mmc3.bankToUpdate) {\
		case 0:\
			waixing.chrmap[0] = value >> 2;\
			_control_bank(waixing.chrmap[0], chrRom4kMax)\
			Waixing_SH2_PPUFD()\
			return;\
		case 1:\
			waixing.chrmap[1] = value >> 2;\
			_control_bank(waixing.chrmap[1], chrRom4kMax)\
			Waixing_SH2_PPUFE()\
			return;\
		case 2:\
			waixing.chrmap[2] = value >> 2;\
			_control_bank(waixing.chrmap[2], chrRom4kMax)\
			Waixing_SH2_PPUFD()\
			return;\
		case 3:\
			return;\
		case 4:\
			waixing.chrmap[4] = value >> 2;\
			_control_bank(waixing.chrmap[4], chrRom4kMax)\
			Waixing_SH2_PPUFE()\
			return;\
		case 5:\
			return;\
	}\
}

void map_init_Waixing(BYTE model) {
	prgRom16kMax = info.prg_rom_16k_count - 1;
	prgRom8kMax = info.prg_rom_8k_count - 1;
	prgRom8kBeforeLast = info.prg_rom_8k_count - 2;
	chrRom4kMax = info.chr_rom_4k_count - 1;
	chrRom1kMax = info.chr_rom_1k_count - 1;

	switch (model) {
		case WPSX:
			EXTCL_CPU_WR_MEM(Waixing_PSx);

			map_prg_rom_8k(4, 0, 0);
			break;
		case WTB:
			EXTCL_CPU_WR_MEM(Waixing_TypeB);
			EXTCL_SAVE_MAPPER(Waixing_TypeB);
			EXTCL_WR_CHR(Waixing_TypeB);
			EXTCL_CPU_EVERY_CYCLE(MMC3);
			EXTCL_PPU_000_TO_34X(MMC3);
			EXTCL_PPU_000_TO_255(MMC3);
			EXTCL_PPU_256_TO_319(MMC3);
			EXTCL_PPU_320_TO_34X(MMC3);
			EXTCL_UPDATE_R2006(MMC3);
			mapper.internal_struct[0] = (BYTE *) &waixing;
			mapper.internal_struct_size[0] = sizeof(waixing);
			mapper.internal_struct[1] = (BYTE *) &mmc3;
			mapper.internal_struct_size[1] = sizeof(mmc3);

			if (info.reset >= HARD) {
				memset(&mmc3, 0x00, sizeof(mmc3));
				memset(&irqA12, 0x00, sizeof(irqA12));
				memset(&waixing, 0x00, sizeof(waixing));

				{
					BYTE i;

					chr_bank_1k_reset()

					for (i = 0; i < 8; i++) {
						waixing.chrmap[i] = i;
					}
				}
			}

			irqA12.present = TRUE;
			irqA12_delay = 1;
			break;
		case WTA:
		case WTC:
		case WTD:
		case WTE:
			if (model == WTA) {
				min = 0x08;
				max = 0x09;
			} else if (model == WTC) {
				min = 0x08;
				max = 0x0B;
			} else if (model == WTD) {
				min = 0x00;
				max = 0x01;
			} else if (model == WTE) {
				min = 0x00;
				max = 0x03;
			}
			EXTCL_CPU_WR_MEM(Waixing_TypeACDE);
			EXTCL_SAVE_MAPPER(Waixing_TypeACDE);
			EXTCL_WR_CHR(Waixing_TypeACDE);
			EXTCL_CPU_EVERY_CYCLE(MMC3);
			EXTCL_PPU_000_TO_34X(MMC3);
			EXTCL_PPU_000_TO_255(MMC3);
			EXTCL_PPU_256_TO_319(MMC3);
			EXTCL_PPU_320_TO_34X(MMC3);
			EXTCL_UPDATE_R2006(MMC3);
			mapper.internal_struct[0] = (BYTE *) &waixing;
			mapper.internal_struct_size[0] = sizeof(waixing);
			mapper.internal_struct[1] = (BYTE *) &mmc3;
			mapper.internal_struct_size[1] = sizeof(mmc3);

			if (info.reset >= HARD) {
				memset(&mmc3, 0x00, sizeof(mmc3));
				memset(&irqA12, 0x00, sizeof(irqA12));
				memset(&waixing, 0x00, sizeof(waixing));

				{
					BYTE i;

					chr_bank_1k_reset()

					for (i = 0; i < 8; i++) {
						waixing.chrmap[i] = i;

						if ((waixing.chrmap[i] >= min) && (waixing.chrmap[i] <= max)) {
							chr.bank_1k[i] = &waixing.chrRam[(waixing.chrmap[i] - min) << 10];
						}
					}
				}
			}

			irqA12.present = TRUE;
			irqA12_delay = 1;
			break;
		case WTG:
			EXTCL_CPU_WR_MEM(Waixing_TypeG);
			EXTCL_SAVE_MAPPER(Waixing_TypeG);
			EXTCL_WR_CHR(Waixing_TypeG);
			EXTCL_CPU_EVERY_CYCLE(MMC3);
			EXTCL_PPU_000_TO_34X(MMC3);
			EXTCL_PPU_000_TO_255(MMC3);
			EXTCL_PPU_256_TO_319(MMC3);
			EXTCL_PPU_320_TO_34X(MMC3);
			EXTCL_UPDATE_R2006(MMC3);
			mapper.internal_struct[0] = (BYTE *) &waixing;
			mapper.internal_struct_size[0] = sizeof(waixing);
			mapper.internal_struct[1] = (BYTE *) &mmc3;
			mapper.internal_struct_size[1] = sizeof(mmc3);

			if (info.reset >= HARD) {
				memset(&mmc3, 0x00, sizeof(mmc3));
				memset(&irqA12, 0x00, sizeof(irqA12));
				memset(&waixing, 0x00, sizeof(waixing));

				{
					BYTE i;

					chr_bank_1k_reset()

					for (i = 0; i < 8; i++) {
						waixing.chrmap[i] = i;

						if (waixing.chrmap[i] < 8) {
							chr.bank_1k[i] = &waixing.chrRam[waixing.chrmap[i] << 10];
						}
					}
				}
			}

			irqA12.present = TRUE;
			irqA12_delay = 1;
			break;
		case WTH:
			EXTCL_CPU_WR_MEM(Waixing_TypeH);
			EXTCL_SAVE_MAPPER(Waixing_TypeH);
			EXTCL_CPU_EVERY_CYCLE(MMC3);
			EXTCL_PPU_000_TO_34X(MMC3);
			EXTCL_PPU_000_TO_255(MMC3);
			EXTCL_PPU_256_TO_319(MMC3);
			EXTCL_PPU_320_TO_34X(MMC3);
			EXTCL_UPDATE_R2006(MMC3);
			mapper.internal_struct[0] = (BYTE *) &waixing;
			mapper.internal_struct_size[0] = sizeof(waixing);
			mapper.internal_struct[1] = (BYTE *) &mmc3;
			mapper.internal_struct_size[1] = sizeof(mmc3);

			if (info.reset >= HARD) {
				memset(&mmc3, 0x00, sizeof(mmc3));
				memset(&irqA12, 0x00, sizeof(irqA12));
				memset(&waixing, 0x00, sizeof(waixing));

				{
					BYTE i, value;

					map_prg_rom_8k_reset();
					chr_bank_1k_reset()

					for (i = 0; i < 8; i++) {
						if (i < 4) {
							waixing.prgmap[i] = mapper.rom_map_to[i];
						}

						waixing.chrmap[i] = i;

						if (mapper.write_vram) {
							chr.bank_1k[i] = &waixing.chrRam[waixing.chrmap[i] << 10];
						}
					}

					Waixing_TypeH_prg8kupdate()
				}
			}

			irqA12.present = TRUE;
			irqA12_delay = 1;
			break;
		case SH2:
			EXTCL_CPU_WR_MEM(Waixing_SH2);
			EXTCL_SAVE_MAPPER(Waixing_SH2);
			EXTCL_AFTER_RD_CHR(Waixing_SH2);
			EXTCL_UPDATE_R2006(Waixing_SH2);
			EXTCL_WR_CHR(Waixing_SH2);
			EXTCL_CPU_EVERY_CYCLE(MMC3);
			EXTCL_PPU_000_TO_34X(MMC3);
			EXTCL_PPU_000_TO_255(MMC3);
			EXTCL_PPU_256_TO_319(MMC3);
			EXTCL_PPU_320_TO_34X(MMC3);
			mapper.internal_struct[0] = (BYTE *) &waixing;
			mapper.internal_struct_size[0] = sizeof(waixing);
			mapper.internal_struct[1] = (BYTE *) &mmc3;
			mapper.internal_struct_size[1] = sizeof(mmc3);

			if (info.reset >= HARD) {
				memset(&mmc3, 0x00, sizeof(mmc3));
				memset(&irqA12, 0x00, sizeof(irqA12));
				memset(&waixing, 0x00, sizeof(waixing));

				waixing.reg = 0xFD;

				waixing.ctrl[0] = 1;
				waixing.ctrl[1] = 1;

				map_prg_rom_8k_reset();
				chr_bank_1k_reset()

				waixing.chrmap[0] = 0;
				waixing.chrmap[1] = 0;
				waixing.chrmap[2] = 0;
				waixing.chrmap[4] = 0;

				Waixing_SH2_PPUFD();
			}

			irqA12.present = TRUE;
			irqA12_delay = 1;
			break;
	}
}

void extcl_cpu_wr_mem_Waixing_PSx(WORD address, BYTE value) {
	BYTE swap = value >> 7;

	if (value & 0x40) {
		mirroring_H();
	} else {
		mirroring_V();
	}

	value &= 0x3F;

	switch (address & 0x0003) {
		case 0x0000:
			control_bank(prgRom16kMax)
			map_prg_rom_8k(2, 0, value);
			value++;
			control_bank(prgRom16kMax)
			map_prg_rom_8k(2, 2, value);
			break;
		case 0x0001:
			control_bank(prgRom16kMax)
			map_prg_rom_8k(2, 0, value);
			map_prg_rom_8k(2, 2, prgRom16kMax);
			break;
		case 0x0002:
			value = (value << 1) | swap;
			control_bank(prgRom8kMax)
			map_prg_rom_8k(1, 0, value);
			map_prg_rom_8k(1, 1, value);
			map_prg_rom_8k(1, 2, value);
			map_prg_rom_8k(1, 3, value);
			break;
		case 0x0003:
			control_bank(prgRom16kMax)
			map_prg_rom_8k(2, 0, value);
			map_prg_rom_8k(2, 2, value);
			break;
	}
	map_prg_rom_8k_update();
}

void extcl_cpu_wr_mem_Waixing_TypeACDE(WORD address, BYTE value) {
	BYTE save = value;

	switch (address & 0xE001) {
		case 0x8000:
			waixing_8000(mmc3.bankToUpdate = value & 0x07)
			return;
		case 0x8001:
			Waixing_TypeACDE_8001()
			break;
		case 0xA001:
			return;
	}
	extcl_cpu_wr_mem_MMC3(address, save);
}
BYTE extcl_save_mapper_Waixing_TypeACDE(BYTE mode, BYTE slot, FILE *fp) {
	save_slot_ele(mode, slot, waixing.chrmap);
	save_slot_ele(mode, slot, waixing.chrRam);
	extcl_save_mapper_MMC3(mode, slot, fp);

	if (mode == SAVE_SLOT_READ) {
		BYTE i;

		for (i = 0; i < 8; i++) {
			if ((waixing.chrmap[i] >= min) && (waixing.chrmap[i] <= max)) {
				chr.bank_1k[i] = &waixing.chrRam[(waixing.chrmap[i] - min) << 10];
			}
		}
	}

	return (EXIT_OK);
}
void extcl_wr_chr_Waixing_TypeACDE(WORD address, BYTE value) {
	const BYTE slot = address >> 10;

	if ((waixing.chrmap[slot] >= min) && (waixing.chrmap[slot] <= max)) {
		chr.bank_1k[slot][address & 0x3FF] = value;
	}
}

void extcl_cpu_wr_mem_Waixing_TypeB(WORD address, BYTE value) {
	BYTE save = value;

	switch (address & 0xE001) {
		case 0x8000:
			waixing_8000(mmc3.bankToUpdate = value & 0x07)
			return;
		case 0x8001:
			Waixing_TypeB_8001()
			break;
		case 0xA001:
			return;
	}
	extcl_cpu_wr_mem_MMC3(address, save);
}
BYTE extcl_save_mapper_Waixing_TypeB(BYTE mode, BYTE slot, FILE *fp) {
	save_slot_ele(mode, slot, waixing.chrmap);
	save_slot_ele(mode, slot, waixing.chrRam);
	extcl_save_mapper_MMC3(mode, slot, fp);

	if (mode == SAVE_SLOT_READ) {
		BYTE i;

		for (i = 0; i < 8; i++) {
			if (waixing.chrmap[i] & 0x80) {
				BYTE value = waixing.chrmap[i];

				control_bank(chrRom1kMax)
				chr.bank_1k[i] = &waixing.chrRam[value << 10];
			}
		}
	}

	return (EXIT_OK);
}
void extcl_wr_chr_Waixing_TypeB(WORD address, BYTE value) {
	const BYTE slot = address >> 10;

	if (waixing.chrmap[slot] & 0x80) {
		chr.bank_1k[slot][address & 0x3FF] = value;
	}
}

void extcl_cpu_wr_mem_Waixing_TypeG(WORD address, BYTE value) {
	BYTE save = value;

	switch (address & 0xE001) {
		case 0x8000:
			waixing_8000(mmc3.bankToUpdate = value & 0x0F)
			return;
		case 0x8001:
			Waixing_TypeG_8001()
			return;
		case 0xA000:
			switch (value & 0x03) {
				case 0:
					mirroring_V();
					break;
				case 1:
					mirroring_H();
					break;
				case 2:
					mirroring_SCR0();
					break;
				case 3:
					mirroring_SCR1();
					break;
			}
			return;
		case 0xA001:
			return;
	}
	extcl_cpu_wr_mem_MMC3(address, save);
}
BYTE extcl_save_mapper_Waixing_TypeG(BYTE mode, BYTE slot, FILE *fp) {
	save_slot_ele(mode, slot, waixing.chrmap);
	save_slot_ele(mode, slot, waixing.chrRam);
	extcl_save_mapper_MMC3(mode, slot, fp);

	if (mode == SAVE_SLOT_READ) {
		BYTE i;

		for (i = 0; i < 8; i++) {
			if (waixing.chrmap[i] < 8) {
				chr.bank_1k[i] = &waixing.chrRam[waixing.chrmap[i] << 10];
			}
		}
	}

	return (EXIT_OK);
}
void extcl_wr_chr_Waixing_TypeG(WORD address, BYTE value) {
	const BYTE slot = address >> 10;

	if (waixing.chrmap[slot] < 8) {
		chr.bank_1k[slot][address & 0x3FF] = value;
	}
}

void extcl_cpu_wr_mem_Waixing_TypeH(WORD address, BYTE value) {
	BYTE save = value;

	switch (address & 0xE001) {
		case 0x8000:
			Waixing_TypeH_8000()
			return;
		case 0x8001:
			Waixing_TypeH_8001()
			return;
	}
	extcl_cpu_wr_mem_MMC3(address, save);
}
BYTE extcl_save_mapper_Waixing_TypeH(BYTE mode, BYTE slot, FILE *fp) {
	save_slot_ele(mode, slot, waixing.prgmap);
	save_slot_ele(mode, slot, waixing.chrmap);
	save_slot_ele(mode, slot, waixing.chrRam);
	save_slot_ele(mode, slot, waixing.ctrl);
	extcl_save_mapper_MMC3(mode, slot, fp);

	if ((mode == SAVE_SLOT_READ) && mapper.write_vram) {
		BYTE i;

		for (i = 0; i < 8; i++) {
			chr.bank_1k[i] = &waixing.chrRam[waixing.chrmap[i] << 10];
		}
	}

	return (EXIT_OK);
}

void extcl_cpu_wr_mem_Waixing_SH2(WORD address, BYTE value) {
	BYTE save = value;

	switch (address & 0xE001) {
		case 0x8000:
			Waixing_SH2_8000()
			return;
		case 0x8001:
			Waixing_SH2_8001()
			break;
	}
	extcl_cpu_wr_mem_MMC3(address, save);
}
BYTE extcl_save_mapper_Waixing_SH2(BYTE mode, BYTE slot, FILE *fp) {
	save_slot_ele(mode, slot, waixing.chrmap);
	save_slot_ele(mode, slot, waixing.chrRam);
	save_slot_ele(mode, slot, waixing.reg);
	save_slot_ele(mode, slot, waixing.ctrl);
	extcl_save_mapper_MMC3(mode, slot, fp);

	if (mode == SAVE_SLOT_READ) {
		Waixing_SH2_PPUFD()
		Waixing_SH2_PPUFE()
	}

	return (EXIT_OK);
}
void extcl_after_rd_chr_Waixing_SH2(WORD address) {
	Waixing_SH2_PPU(address)
}
void extcl_update_r2006_Waixing_SH2(WORD old_r2006) {
	/* questo e' per l'MMC3 */
	irqA12_IO(old_r2006);

	if (r2006.value >= 0x2000) {
		return;
	}

	Waixing_SH2_PPU(r2006.value)
}
void extcl_wr_chr_Waixing_SH2(WORD address, BYTE value) {
	if (!waixing.ctrl[address >> 12]) {
		chr.bank_1k[address >> 10][address & 0x3FF] = value;
	}
}
