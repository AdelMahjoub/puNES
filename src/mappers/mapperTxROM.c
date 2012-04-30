/*
 * mapperTxROM.c
 *
 *  Created on: 28/set/2011
 *      Author: fhorse
 */

#include <stdlib.h>
#include <string.h>
#include "mappers.h"
#include "memmap.h"
#include "irqA12.h"
#include "savestate.h"

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
		controlBank(3)\
		txrom.chr[a][1] = value << 11;\
		txrom.chr[b][1] = txrom.chr[a][1] | 0x400;\
		chr.bank1k[a] = &txrom.chrRam[txrom.chr[a][1]];\
		chr.bank1k[b] = &txrom.chrRam[txrom.chr[b][1]];\
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
		controlBank(7)\
		txrom.chr[a][1] = value << 10;\
		chr.bank1k[a] = &txrom.chrRam[txrom.chr[a][1]];\
		return;\
	} else {\
		txrom.chr[a][0] = txrom.chr[a][1] = FALSE;\
	}\
}

WORD prgRom8kMax, prgRom8kBeforeLast, chrRom1kMax;
BYTE type;

void mapInit_TxROM(BYTE model) {
	prgRom8kMax = info.prgRom8kCount - 1;
	chrRom1kMax = info.chrRom1kCount - 1;
	prgRom8kBeforeLast = info.prgRom8kCount - 2;

	switch (model) {
		case TLSROM:
		case TKSROM:
			EXTCLCPUWRMEM(TKSROM);

			irqA12_delay = 1;

			if (model == TKSROM) {
				info.prgRamPlus8kCount = 1;
				info.prgRamBatBanks = 1;
			}
			break;
		case TQROM:
			EXTCLCPUWRMEM(TQROM);
			EXTCLWRCHR(TQROM);

			mapper.writeVRAM = FALSE;

			irqA12_delay = 1;

			break;
	}

	EXTCLSAVEMAPPER(TxROM);
	EXTCLCPUEVERYCYCLE(MMC3);
	EXTCLPPU000TO34X(MMC3);
	EXTCLPPU000TO255(MMC3);
	EXTCLPPU256TO319(MMC3);
	EXTCLPPU320TO34X(MMC3);
	EXTCL2006UPDATE(MMC3);
	mapper.intStruct[0] = (BYTE *) &txrom;
	mapper.intStructSize[0] = sizeof(txrom);
	mapper.intStruct[1] = (BYTE *) &mmc3;
	mapper.intStructSize[1] = sizeof(mmc3);

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

void extclCpuWrMem_TKSROM(WORD address, BYTE value) {
	switch (address & 0xE001) {
		case 0x8001: {
			switch (mmc3.bankToUpdate) {
				case 0:
				case 1:
					if (!mmc3.chrRomCfg) {
						const BYTE slot = mmc3.bankToUpdate << 1;
						ntbl.bank1k[slot] = &ntbl.data[((value >> 7) ^ 0x01) << 10];
						ntbl.bank1k[slot | 0x01] = ntbl.bank1k[slot];
					}
					break;
				case 2:
				case 3:
				case 4:
				case 5:
					if (mmc3.chrRomCfg) {
						ntbl.bank1k[mmc3.bankToUpdate - 2] =
								&ntbl.data[((value >> 7) ^ 0x01) << 10];
					}
					break;
			}
			break;
		}
		case 0xA000:
			return;
	}
	extclCpuWrMem_MMC3(address, value);
}

void extclCpuWrMem_TQROM(WORD address, BYTE value) {
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
	extclCpuWrMem_MMC3(address, value);
}
void extclWrChr_TQROM(WORD address, BYTE value) {
	const BYTE slot = address >> 10;

	if (txrom.chr[slot]) {
		chr.bank1k[slot][address & 0x3FF] = value;
	}
}

BYTE extclSaveMapper_TxROM(BYTE mode, BYTE slot, FILE *fp) {
	savestateEle(mode, slot, txrom.delay);
	if (type == TQROM) {
		savestateEle(mode, slot, txrom.chr);
		if (mode == SSREAD) {
			BYTE i;
			for (i = 0; i < LENGTH(txrom.chr); i++) {
				if (txrom.chr[i][0]) {
					chr.bank1k[i] = &txrom.chrRam[txrom.chr[i][1]];
				}
			}
		}
		savestateEle(mode, slot, txrom.chrRam);
	}
	extclSaveMapper_MMC3(mode, slot, fp);

	return (EXIT_OK);
}
