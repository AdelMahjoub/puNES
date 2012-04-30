/*
 * mapperVRC4.c
 *
 *  Created on: 10/set/2011
 *      Author: fhorse
 */

#include <stdlib.h>
#include <string.h>
#include "mappers.h"
#include "memmap.h"
#include "cpu6502.h"
#include "savestate.h"

#define chrRom1kUpdate(slot, mask, shift)\
	value = (vrc4.chrRomBank[slot] & mask) | ((value & 0x0F) << shift);\
	controlBank(chrRom1kMax)\
	chr.bank1k[slot] = &chr.data[value << 10];\
	vrc4.chrRomBank[slot] = value

WORD prgRom8kMax, prgRom8kBeforeLast, chrRom1kMax;
BYTE type;

const BYTE shiftVRC4[5] = { 0x01, 0x00, 0x06, 0x02, 0x02 };
const WORD maskVRC4[5]  = { 0x0006, 0x0003, 0x00C0, 0x000C, 0x000C };
const WORD tableVRC4[5][4] = {
	{0x0000, 0x0001, 0x0002, 0x0003},
	{0x0000, 0x0002, 0x0001, 0x0003},
	{0x0000, 0x0001, 0x0002, 0x0003},
	{0x0000, 0x0002, 0x0001, 0x0003},
	{0x0000, 0x0001, 0x0002, 0x0003},
};

void mapInit_VRC4(BYTE revision) {
	prgRom8kMax = info.prgRom8kCount - 1;
	prgRom8kBeforeLast = prgRom8kMax - 1;
	chrRom1kMax = info.chrRom1kCount - 1;

	EXTCLCPUWRMEM(VRC4);
	EXTCLSAVEMAPPER(VRC4);
	EXTCLCPUEVERYCYCLE(VRC4);
	mapper.intStruct[0] = (BYTE *) &vrc4;
	mapper.intStructSize[0] = sizeof(vrc4);

	if (info.reset >= HARD) {
		BYTE i;

		memset(&vrc4, 0x00, sizeof(vrc4));
		for (i = 0; i < 8; i++) {
			vrc4.chrRomBank[i] = i;
		}
	} else {
		vrc4.irqEnabled = 0;
		vrc4.irqReload = 0;
		vrc4.irqMode = 0;
		vrc4.irqAcknowledge = 0;
		vrc4.irqCount = 0;
		vrc4.irqPrescaler = 0;
	}

	type = revision;
}
void extclCpuWrMem_VRC4(WORD address, BYTE value) {
	WORD tmp = address & 0xF000;

	if ((tmp == 0x8000) || (tmp == 0xA000)) {
		address &= 0xF000;
	} else {
		address = (address & 0xF000)
		        | tableVRC4[type][(address & maskVRC4[type]) >> shiftVRC4[type]];
	}

	switch (address) {
		case 0x8000:
			controlBankWithAND(0x1F, prgRom8kMax)
			mapPrgRom8k(1, vrc4.swapMode, value);
			mapPrgRom8k(1, 0x02 >> vrc4.swapMode, prgRom8kBeforeLast);
			mapPrgRom8kUpdate();
			return;
		case 0xA000:
			controlBankWithAND(0x1F, prgRom8kMax)
			mapPrgRom8k(1, 1, value);
			mapPrgRom8kUpdate();
			return;
		case 0x9000:
		case 0x9001: {
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
		}
		case 0x9002:
		case 0x9003:
			value &= 0x02;
			if (vrc4.swapMode != value) {
				WORD swap = mapper.romMapTo[0];
				mapper.romMapTo[0] = mapper.romMapTo[2];
				mapper.romMapTo[2] = swap;
				mapPrgRom8kUpdate();
				vrc4.swapMode = value;
			}
			return;
		case 0xB000:
			chrRom1kUpdate(0, 0xF0, 0);
			return;
		case 0xB001:
			chrRom1kUpdate(0, 0x0F, 4);
			return;
		case 0xB002:
			chrRom1kUpdate(1, 0xF0, 0);
			return;
		case 0xB003:
			chrRom1kUpdate(1, 0x0F, 4);
			return;
		case 0xC000:
			chrRom1kUpdate(2, 0xF0, 0);
			return;
		case 0xC001:
			chrRom1kUpdate(2, 0x0F, 4);
			return;
		case 0xC002:
			chrRom1kUpdate(3, 0xF0, 0);
			return;
		case 0xC003:
			chrRom1kUpdate(3, 0x0F, 4);
			return;
		case 0xD000:
			chrRom1kUpdate(4, 0xF0, 0);
			return;
		case 0xD001:
			chrRom1kUpdate(4, 0x0F, 4);
			return;
		case 0xD002:
			chrRom1kUpdate(5, 0xF0, 0);
			return;
		case 0xD003:
			chrRom1kUpdate(5, 0x0F, 4);
			return;
		case 0xE000:
			chrRom1kUpdate(6, 0xF0, 0);
			return;
		case 0xE001:
			chrRom1kUpdate(6, 0x0F, 4);
			return;
		case 0xE002:
			chrRom1kUpdate(7, 0xF0, 0);
			return;
		case 0xE003:
			chrRom1kUpdate(7, 0x0F, 4);
			return;
		case 0xF000:
			vrc4.irqReload = (vrc4.irqReload & 0xF0) | (value & 0x0F);
			return;
		case 0xF001:
			vrc4.irqReload = (vrc4.irqReload & 0x0F) | ((value & 0x0F) << 4);
			return;
		case 0xF002:
			vrc4.irqAcknowledge = value & 0x01;
			vrc4.irqEnabled = value & 0x02;
			vrc4.irqMode = value & 0x04;
			if (vrc4.irqEnabled) {
				vrc4.irqPrescaler = 0;
				vrc4.irqCount = vrc4.irqReload;
			}
			irq.high &= ~EXTIRQ;
			return;
		case 0xF003:
			vrc4.irqEnabled = vrc4.irqAcknowledge;
			irq.high &= ~EXTIRQ;
			return;
		default:
			return;
	}
}
BYTE extclSaveMapper_VRC4(BYTE mode, BYTE slot, FILE *fp) {
	savestateEle(mode, slot, vrc4.chrRomBank);
	savestateEle(mode, slot, vrc4.swapMode);
	savestateEle(mode, slot, vrc4.irqEnabled);
	savestateEle(mode, slot, vrc4.irqReload);
	savestateEle(mode, slot, vrc4.irqMode);
	savestateEle(mode, slot, vrc4.irqAcknowledge);
	savestateEle(mode, slot, vrc4.irqCount);
	savestateEle(mode, slot, vrc4.irqPrescaler);

	return (EXIT_OK);
}
void extclCPUEveryCycle_VRC4(void) {
	if (!vrc4.irqEnabled) {
		return;
	}

	if (!vrc4.irqMode) {
		if (vrc4.irqPrescaler < 338) {
			vrc4.irqPrescaler += 3;
			return;
		}
		vrc4.irqPrescaler -= 338;
	}

	if (vrc4.irqCount != 0xFF) {
		vrc4.irqCount++;
		return;
	}

	vrc4.irqCount = vrc4.irqReload;
	irq.delay = TRUE;
	irq.high |= EXTIRQ;
}

void mapInit_VRC4BMC(void) {
	prgRom8kMax = info.prgRom8kCount - 1;

	mapInit_VRC4(VRC4E);

	EXTCLCPUWRMEM(VRC4BMC);
}
void extclCpuWrMem_VRC4BMC(WORD address, BYTE value) {
	if (address < 0x6000) {
		return;
	}

	if ((address >= 0x8000) && (address <= 0x8FFF)) {
		value = (mapper.romMapTo[0] & 0x20) | (value & 0x1F);
		controlBank(prgRom8kMax)
		mapPrgRom8k(1, vrc4.swapMode, value);
		mapPrgRom8kUpdate();
		return;
	}
	if ((address >= 0xA000) && (address <= 0xAFFF)) {
		value = (mapper.romMapTo[0] & 0x20) | (value & 0x1F);
		controlBank(prgRom8kMax)
		mapPrgRom8k(1, 1, value);
		mapPrgRom8kUpdate();
		return;
	}
	if ((address >= 0xB000) && (address <= 0xEFFF)) {
		BYTE save = value << 2 & 0x20;

		value = (mapper.romMapTo[0] & 0x1F) | save ;
		controlBank(prgRom8kMax)
		mapPrgRom8k(1, 0, value);

		value = (mapper.romMapTo[1] & 0x1F) | save ;
		controlBank(prgRom8kMax)
		mapPrgRom8k(1, 1, value);

		value = (mapper.romMapTo[2] & 0x1F) | save ;
		controlBank(prgRom8kMax)
		mapPrgRom8k(1, 2, value);

		value = (mapper.romMapTo[3] & 0x1F) | save ;
		controlBank(prgRom8kMax)
		mapPrgRom8k(1, 3, value);

		mapPrgRom8kUpdate();
		return;
	}

	extclCpuWrMem_VRC4(address, value);
}
