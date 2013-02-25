/*
 * mapper52.c
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

#define m52chr1k(vl)\
	bank = ((((m52.reg >> 3) & 0x04) | ((m52.reg >> 1) & 0x02) |\
		((m52.reg >> 6) & (m52.reg >> 4) & 0x01)) << 7) |\
		(vl & (((m52.reg & 0x40) << 1) ^ 0xFF))
#define m52prg8k(vl)\
	value = (((m52.reg & 0x06) | ((m52.reg >> 3) & m52.reg & 0x01)) << 4) |\
		(vl & (((m52.reg << 1) & 0x10) ^ 0x1F))
#define m52chr1kupdate()\
{\
	BYTE i;\
	for (i = 0; i < 8; i++) {\
		WORD bank;\
		m52chr1k(m52.chrmap[i]);\
		_controlBank(bank, chrRom1kMax)\
		chr.bank1k[i] = &chr.data[bank << 10];\
	}\
}
#define m52prg8kupdate()\
{\
	BYTE i;\
	for (i = 0; i < 4; i++) {\
		m52prg8k(m52.prgmap[i]);\
		controlBank(prgRom8kMax)\
		mapPrgRom8k(1, i, value);\
	}\
	mapPrgRom8kUpdate();\
}
#define m52swapChrBank1k(src, dst)\
{\
	BYTE *chrBank1k = chr.bank1k[src];\
	chr.bank1k[src] = chr.bank1k[dst];\
	chr.bank1k[dst] = chrBank1k;\
	WORD map = m52.chrmap[src];\
	m52.chrmap[src] = m52.chrmap[dst];\
	m52.chrmap[dst] = map;\
}
#define m52_8000()\
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
		m52swapChrBank1k(0, 4)\
		m52swapChrBank1k(1, 5)\
		m52swapChrBank1k(2, 6)\
		m52swapChrBank1k(3, 7)\
	}\
	if (mmc3.prgRomCfg != prgRomCfgOld) {\
		WORD p0 = mapper.romMapTo[0];\
		WORD p2 = mapper.romMapTo[2];\
		mapper.romMapTo[0] = p2;\
		mapper.romMapTo[2] = p0;\
		p0 = m52.prgmap[0];\
		p2 = m52.prgmap[2];\
		m52.prgmap[0] = p2;\
		m52.prgmap[2] = p0;\
		/*\
		 * prgRomCfg 0x00 : $C000 - $DFFF fisso al penultimo banco\
		 * prgRomCfg 0x02 : $8000 - $9FFF fisso al penultimo banco\
		 */\
		m52.prgmap[mmc3.prgRomCfg ^ 0x02] = prgRom8kBeforeLast;\
		m52prg8k(prgRom8kBeforeLast);\
		controlBank(prgRom8kMax)\
		mapPrgRom8k(1, mmc3.prgRomCfg ^ 0x02, value);\
		mapPrgRom8kUpdate();\
	}\
}
#define m52_8001()\
{\
	WORD bank;\
	switch (mmc3.bankToUpdate) {\
		case 0:\
			m52.chrmap[mmc3.chrRomCfg] = value;\
			m52.chrmap[mmc3.chrRomCfg | 0x01] = value + 1;\
			m52chr1k(value);\
			bank &= 0xFFE;\
			_controlBank(bank, chrRom1kMax)\
			chr.bank1k[mmc3.chrRomCfg] = &chr.data[bank << 10];\
			chr.bank1k[mmc3.chrRomCfg | 0x01] = &chr.data[(bank + 1) << 10];\
			return;\
		case 1:\
			m52.chrmap[mmc3.chrRomCfg | 0x02] = value;\
			m52.chrmap[mmc3.chrRomCfg | 0x03] = value + 1;\
			m52chr1k(value);\
			bank &= 0xFFE;\
			_controlBank(bank, chrRom1kMax)\
			chr.bank1k[mmc3.chrRomCfg | 0x02] = &chr.data[bank << 10];\
			chr.bank1k[mmc3.chrRomCfg | 0x03] = &chr.data[(bank + 1) << 10];\
			return;\
		case 2:\
			m52.chrmap[mmc3.chrRomCfg ^ 0x04] = value;\
			m52chr1k(value);\
			_controlBank(bank, chrRom1kMax)\
			chr.bank1k[mmc3.chrRomCfg ^ 0x04] = &chr.data[bank << 10];\
			return;\
		case 3:\
			m52.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x01] = value;\
			m52chr1k(value);\
			_controlBank(bank, chrRom1kMax)\
			chr.bank1k[(mmc3.chrRomCfg ^ 0x04) | 0x01] = &chr.data[bank << 10];\
			return;\
		case 4:\
			m52.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x02] = value;\
			m52chr1k(value);\
			_controlBank(bank, chrRom1kMax)\
			chr.bank1k[(mmc3.chrRomCfg ^ 0x04) | 0x02] = &chr.data[bank << 10];\
			return;\
		case 5:\
			m52.chrmap[(mmc3.chrRomCfg ^ 0x04) | 0x03] = value;\
			m52chr1k(value);\
			_controlBank(bank, chrRom1kMax)\
			chr.bank1k[(mmc3.chrRomCfg ^ 0x04) | 0x03] = &chr.data[bank << 10];\
			return;\
		case 6:\
			m52.prgmap[mmc3.prgRomCfg] = value;\
			m52prg8k(value);\
			controlBank(prgRom8kMax)\
			mapPrgRom8k(1, mmc3.prgRomCfg, value);\
			mapPrgRom8kUpdate();\
			return;\
		case 7:\
			m52.prgmap[1] = value;\
			m52prg8k(value);\
			controlBank(prgRom8kMax)\
			mapPrgRom8k(1, 1, value);\
			mapPrgRom8kUpdate();\
			return;\
	}\
}

WORD prgRom8kMax, prgRom8kBeforeLast, chrRom1kMax;

void mapInit_52(void) {
	prgRom8kMax = info.prgRom8kCount - 1;
	prgRom8kBeforeLast = info.prgRom8kCount - 2;
	chrRom1kMax = info.chrRom1kCount - 1;

	EXTCL_CPU_WR_MEM(52);
	EXTCL_SAVE_MAPPER(52);
	EXTCL_CPU_EVERY_CYCLE(MMC3);
	EXTCL_PPU_000_TO_34X(MMC3);
	EXTCL_PPU_000_TO_255(MMC3);
	EXTCL_PPU_256_TO_319(MMC3);
	EXTCL_PPU_320_TO_34X(MMC3);
	EXTCL_UPDATE_R2006(MMC3);
	mapper.intStruct[0] = (BYTE *) &m52;
	mapper.intStructSize[0] = sizeof(m52);
	mapper.intStruct[1] = (BYTE *) &mmc3;
	mapper.intStructSize[1] = sizeof(mmc3);

	memset(&mmc3, 0x00, sizeof(mmc3));
	memset(&irqA12, 0x00, sizeof(irqA12));
	memset(&m52, 0x00, sizeof(m52));

	{
		BYTE value, i;

		mapPrgRom8kReset();
		chrBank1kReset()

		for (i = 0; i < 8; i++) {
			if (i < 4) {
				m52.prgmap[i] = mapper.romMapTo[i];
			}
			m52.chrmap[i] = i;
		}

		m52prg8kupdate()
		m52chr1kupdate()
	}

	info.mapperExtendWrite = TRUE;

	irqA12.present = TRUE;
	irqA12_delay = 1;
}
void extcl_cpu_wr_mem_52(WORD address, BYTE value) {
	if (address > 0x7FFF) {
		switch (address & 0xE001) {
			case 0x8000:
				m52_8000()
				return;
			case 0x8001:
				m52_8001()
				return;
		}
		extcl_cpu_wr_mem_MMC3(address, value);
		return;
	}

	if (address >= 0x6000) {
		if (!m52.disabled) {
			m52.disabled = TRUE;
			m52.reg = value;

			m52prg8kupdate()
			m52chr1kupdate()
		}
	}
}
BYTE extcl_save_mapper_52(BYTE mode, BYTE slot, FILE *fp) {
	savestateEle(mode, slot, m52.disabled);
	savestateEle(mode, slot, m52.reg);
	savestateEle(mode, slot, m52.prgmap);
	savestateEle(mode, slot, m52.chrmap);
	extcl_save_mapper_MMC3(mode, slot, fp);

	return (EXIT_OK);
}
