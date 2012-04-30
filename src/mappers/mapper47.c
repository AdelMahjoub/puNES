/*
 * mapper47.c
 *
 *  Created on: 21/apr/2012
 *      Author: fhorse
 */

#include <stdlib.h>
#include <string.h>
#include "mappers.h"
#include "memmap.h"
#include "irqA12.h"
#include "savestate.h"

#define m47chr1k(vl) bank = (m47.reg << 7) | (vl & 0x7F)
#define m47prg8k(vl) value = (m47.reg << 4) | (vl & 0x0F)
#define m47chr1kupdate()\
{\
	BYTE i;\
	for (i = 0; i < 8; i++) {\
		WORD bank;\
		m47chr1k(m47.chrmap[i]);\
		_controlBank(bank, chrRom1kMax)\
		chr.bank1k[i] = &chr.data[bank << 10];\
	}\
}
#define m47prg8kupdate()\
{\
	BYTE i;\
	for (i = 0; i < 4; i++) {\
		m47prg8k(m47.prgmap[i]);\
		controlBank(prgRom8kMax)\
		mapPrgRom8k(1, i, value);\
	}\
	mapPrgRom8kUpdate();\
}
#define m47swapChrBank1k(src, dst)\
{\
	BYTE *chrBank1k = chr.bank1k[src];\
	chr.bank1k[src] = chr.bank1k[dst];\
	chr.bank1k[dst] = chrBank1k;\
	WORD map = m47.chrmap[src];\
	m47.chrmap[src] = m47.chrmap[dst];\
	m47.chrmap[dst] = map;\
}
#define m47_8000()\
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
		m47swapChrBank1k(0, 4)\
		m47swapChrBank1k(1, 5)\
		m47swapChrBank1k(2, 6)\
		m47swapChrBank1k(3, 7)\
	}\
	if (mmc3.prgRomCfg != prgRomCfgOld) {\
		WORD p0 = mapper.romMapTo[0];\
		WORD p2 = mapper.romMapTo[2];\
		mapper.romMapTo[0] = p2;\
		mapper.romMapTo[2] = p0;\
		p0 = m47.prgmap[0];\
		p2 = m47.prgmap[2];\
		m47.prgmap[0] = p2;\
		m47.prgmap[2] = p0;\
		m47.prgmap[mmc3.prgRomCfg ^ 0x02] = prgRom8kBeforeLast;\
		/*\
		 * prgRomCfg 0x00 : $C000 - $DFFF fisso al penultimo banco\
		 * prgRomCfg 0x02 : $8000 - $9FFF fisso al penultimo banco\
		 */\
		m47prg8k(prgRom8kBeforeLast);\
		controlBank(prgRom8kMax)\
		mapPrgRom8k(1, mmc3.prgRomCfg ^ 0x02, value);\
		mapPrgRom8kUpdate();\
	}\
}
#define m47_8001()\
{\
	WORD bank;\
	switch (mmc3.bankToUpdate) {\
		case 0:\
			m47.chrmap[mmc3.chrRomCfg] = value;\
			m47.chrmap[mmc3.chrRomCfg | 0x01] = value + 1;\
			m47chr1k(value);\
			bank &= 0xFFE;\
			_controlBank(bank, chrRom1kMax)\
			chr.bank1k[mmc3.chrRomCfg] = &chr.data[bank << 10];\
			chr.bank1k[mmc3.chrRomCfg | 0x01] = &chr.data[(bank + 1) << 10];\
			return;\
		case 1:\
			m47.chrmap[mmc3.chrRomCfg | 0x02] = value;\
			m47.chrmap[mmc3.chrRomCfg | 0x03] = value + 1;\
			m47chr1k(value);\
			bank &= 0xFFE;\
			_controlBank(bank, chrRom1kMax)\
			chr.bank1k[mmc3.chrRomCfg | 0x02] = &chr.data[bank << 10];\
			chr.bank1k[mmc3.chrRomCfg | 0x03] = &chr.data[(bank + 1) << 10];\
			return;\
		case 2:\
			m47.chrmap[mmc3.chrRomCfg ^ 0x04] = value;\
			m47chr1k(value);\
			_controlBank(bank, chrRom1kMax)\
			chr.bank1k[mmc3.chrRomCfg ^ 0x04] = &chr.data[bank << 10];\
			return;\
		case 3:\
			m47.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x01] = value;\
			m47chr1k(value);\
			_controlBank(bank, chrRom1kMax)\
			chr.bank1k[(mmc3.chrRomCfg ^ 0x04) | 0x01] = &chr.data[bank << 10];\
			return;\
		case 4:\
			m47.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x02] = value;\
			m47chr1k(value);\
			_controlBank(bank, chrRom1kMax)\
			chr.bank1k[(mmc3.chrRomCfg ^ 0x04) | 0x02] = &chr.data[bank << 10];\
			return;\
		case 5:\
			m47.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x03] = value;\
			m47chr1k(value);\
			_controlBank(bank, chrRom1kMax)\
			chr.bank1k[(mmc3.chrRomCfg ^ 0x04) | 0x03] = &chr.data[bank << 10];\
			return;\
		case 6:\
			m47.prgmap[mmc3.prgRomCfg] = value;\
			m47prg8k(value);\
			controlBank(prgRom8kMax)\
			mapPrgRom8k(1, mmc3.prgRomCfg, value);\
			mapPrgRom8kUpdate();\
			return;\
		case 7:\
			m47.prgmap[1] = value;\
			m47prg8k(value);\
			controlBank(prgRom8kMax)\
			mapPrgRom8k(1, 1, value);\
			mapPrgRom8kUpdate();\
			return;\
	}\
}

WORD prgRom8kMax, prgRom8kBeforeLast, chrRom1kMax;

void mapInit_47(void) {
	prgRom8kMax = info.prgRom8kCount - 1;
	prgRom8kBeforeLast = info.prgRom8kCount - 2;
	chrRom1kMax = info.chrRom1kCount - 1;

	EXTCLCPUWRMEM(47);
	EXTCLSAVEMAPPER(47);
	EXTCLCPUEVERYCYCLE(MMC3);
	EXTCLPPU000TO34X(MMC3);
	EXTCLPPU000TO255(MMC3);
	EXTCLPPU256TO319(MMC3);
	EXTCLPPU320TO34X(MMC3);
	EXTCL2006UPDATE(MMC3);
	mapper.intStruct[0] = (BYTE *) &m47;
	mapper.intStructSize[0] = sizeof(m47);
	mapper.intStruct[1] = (BYTE *) &mmc3;
	mapper.intStructSize[1] = sizeof(mmc3);

	memset(&mmc3, 0x00, sizeof(mmc3));
	memset(&irqA12, 0x00, sizeof(irqA12));
	memset(&m47, 0x00, sizeof(m47));

	{
		BYTE value, i;

		mapPrgRom8kReset();
		chrBank1kReset()

		for (i = 0; i < 8; i++) {
			if (i < 4) {
				m47.prgmap[i] = mapper.romMapTo[i];
			}
			m47.chrmap[i] = i;
		}

		m47prg8kupdate()
		m47chr1kupdate()
	}

	info.mapperExtendWrite = TRUE;

	irqA12.present = TRUE;
	irqA12_delay = 1;
}
void extclCpuWrMem_47(WORD address, BYTE value) {
	if (address > 0x7FFF) {
		switch (address & 0xE001) {
			case 0x8000:
				m47_8000()
				return;
			case 0x8001:
				m47_8001()
				return;
		}
		extclCpuWrMem_MMC3(address, value);
		return;
	}

	if (address >= 0x6000) {
		value &= 0x01;

		if (m47.reg != value) {
			m47.reg = value;

			m47prg8kupdate()
			m47chr1kupdate()
		}
	}
}
BYTE extclSaveMapper_47(BYTE mode, BYTE slot, FILE *fp) {
	savestateEle(mode, slot, m47.reg);
	savestateEle(mode, slot, m47.prgmap);
	savestateEle(mode, slot, m47.chrmap);
	extclSaveMapper_MMC3(mode, slot, fp);

	return (EXIT_OK);
}
