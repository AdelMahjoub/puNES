/*
 * ppu_inline.h
 *
 *  Created on: 23/apr/2010
 *      Author: fhorse
 */

#ifndef PPU_INLINE_H_
#define PPU_INLINE_H_

#include "external_calls.h"

static BYTE INLINE ppu_rd_mem(WORD address);

static BYTE INLINE ppu_rd_mem(WORD address) {
	if (extcl_rd_ppu) {
		/*
		 * utilizzato dalle mappers :
		 * 90_201_211
		 */
		extcl_rd_ppu(address);
	}

	address &= 0x3FFF;

	if (address < 0x2000) {
		if (extcl_rd_chr) {
			/*
			 * utilizzato dalle mappers :
			 * MMC5
			 */
			return (extcl_rd_chr(address));
		}
		return (chr.bank_1k[address >> 10][address & 0x3FF]);
	}
	if (address < 0x3F00) {
		address &= 0x0FFF;
		if (extcl_rd_nmt) {
			/*
			 * utilizzato dalle mappers :
			 * MMC5
			 * Bandai (B161X02X74)
			 */
			return (extcl_rd_nmt(address));
		}
		return (ntbl.bank_1k[address >> 10][address & 0x3FF]);
	}
	return (palette.color[address & 0x1F]);
}

#endif /* PPU_INLINE_H_ */
