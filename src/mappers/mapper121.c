/*
 * mapper121.c
 *
 *  Created on: 01/ott/2011
 *      Author: fhorse
 */

#include <stdlib.h>
#include <string.h>
#include "mappers.h"
#include "memmap.h"
#include "cpu6502.h"
#include "irqA12.h"
#include "savestate.h"

#define m121Swap8kPrg()\
	if (m121.reg[0]) {\
		value = m121.reg[0];\
		controlBank(prgRom8kMax)\
		mapPrgRom8k(1, 2, value);\
	} else {\
		mapper.romMapTo[2] = m121.bck[1];\
	}\
	if (m121.reg[1]) {\
		value = m121.reg[1];\
		controlBank(prgRom8kMax)\
		mapPrgRom8k(1, 3, value);\
	} else {\
		mapper.romMapTo[3] = prgRom8kMax;\
	}\
	mapPrgRom8kUpdate()

static const BYTE vlu121[4] = { 0x00, 0x83, 0x42, 0x00 };

WORD prgRom32kMax, prgRom8kMax, prgRom8kBeforeLast, chrRom1kMax;

void mapInit_121(void) {
	prgRom32kMax = (info.prgRom16kCount >> 1) - 1;
	prgRom8kMax = info.prgRom8kCount - 1;
	chrRom1kMax = info.chrRom1kCount - 1;
	prgRom8kBeforeLast = info.prgRom8kCount - 2;

	EXTCLCPUWRMEM(121);
	EXTCLCPURDMEM(121);
	EXTCLSAVEMAPPER(121);
	EXTCLCPUEVERYCYCLE(MMC3);
	EXTCLPPU000TO34X(MMC3);
	EXTCLPPU000TO255(MMC3);
	EXTCLPPU256TO319(MMC3);
	EXTCLPPU320TO34X(MMC3);
	EXTCL2006UPDATE(MMC3);
	mapper.intStruct[0] = (BYTE *) &m121;
	mapper.intStructSize[0] = sizeof(m121);
	mapper.intStruct[1] = (BYTE *) &mmc3;
	mapper.intStructSize[1] = sizeof(mmc3);

	info.mapperExtendWrite = TRUE;

	if (info.reset >= HARD) {
		memset(&m121, 0x00, sizeof(m121));
		memset(&mmc3, 0x00, sizeof(mmc3));

		m121.bck[0] = mapper.romMapTo[0];
		m121.bck[1] = mapper.romMapTo[2];
	}

	memset(&irqA12, 0x00, sizeof(irqA12));

	irqA12.present = TRUE;
	irqA12_delay = 1;
}
void extclCpuWrMem_121(WORD address, BYTE value) {
	if (address >= 0x8000) {
		const BYTE prgRomCfg = (value & 0x40) >> 5;

		extclCpuWrMem_MMC3(address, value);

		switch (address & 0xE003) {
			case 0x8000: {
				if (mmc3.prgRomCfg != prgRomCfg) {
					mapper.romMapTo[2] = m121.bck[0];
					mapper.romMapTo[0] = m121.bck[1];
					m121.bck[0] = mapper.romMapTo[0];
					m121.bck[1] = mapper.romMapTo[2];
				}
				m121Swap8kPrg();
				break;
			}
			case 0x8001:
				if (mmc3.bankToUpdate == 6) {
					if (mmc3.prgRomCfg) {
						controlBank(prgRom8kMax)
						m121.bck[1] = value;
					} else {
						controlBank(prgRom8kMax)
						m121.bck[0] = value;
					}
				}
				m121Swap8kPrg();
				break;
			case 0x8003:
				switch (value) {
					case 0x20:
						m121.reg[1] = 0x13;
						break;
					case 0x29:
						m121.reg[1] = 0x1B;
						break;
					case 0x28:
						m121.reg[0] = 0x0C;
						break;
					case 0x26:
						m121.reg[1] = 0x08;
						break;
					case 0xAB:
						m121.reg[1] = 0x07;
						break;
					case 0xEC:
					case 0xEF:
						m121.reg[1] = 0x0D;
						break;
					case 0xFF:
						m121.reg[1] = 0x09;
						break;
					default:
						m121.reg[0] = m121.reg[1] = 0;
						break;
				}
				m121Swap8kPrg();
				break;
		}
		return;
	}

	if ((address < 0x5000) || (address > 0x5FFF)) {
		return;
	}

	m121.reg[2] = vlu121[value & 0x03];
	return;
}
BYTE extclCpuRdMem_121(WORD address, BYTE openbus, BYTE before) {
	if ((address < 0x5000) || (address > 0x5FFF)) {
		return (openbus);
	}

	return (m121.reg[2]);
}
BYTE extclSaveMapper_121(BYTE mode, BYTE slot, FILE *fp) {
	if (savestate.version < 6) {
		if (mode == SSREAD) {
			BYTE old_prgRomBank[2], i;

			savestateEle(mode, slot, old_prgRomBank)

			for (i = 0; i < 2; i++) {
				m121.bck[i] = old_prgRomBank[i];
			}
		} else if (mode == SSCOUNT) {
			savestate.totSize[slot] += sizeof(BYTE) * 2;
		}
	} else {
		savestateEle(mode, slot, m121.bck);
	}
	savestateEle(mode, slot, m121.reg);
	extclSaveMapper_MMC3(mode, slot, fp);

	return (EXIT_OK);
}
