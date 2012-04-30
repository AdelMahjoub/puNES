/*
 * mapperJaleco.h
 *
 *  Created on: 16/lug/2011
 *      Author: fhorse
 */

#ifndef MAPPERJALECO_H_
#define MAPPERJALECO_H_

#include "common.h"

enum {
	JF05      = 2,
	JF11      = 3,
	JF13      = 4,
	JF16      = 5,
	JF17      = 6,
	JF19      = 7,
	SS8806    = 8,
	HOLYDIVER = 9
};
enum {
	JAJAMARU,
	MEZASETOPPRO,
};

struct _ss8806 {
	BYTE chrRomBank[8];
	BYTE enabled;
	WORD mask;
	WORD reload;
	WORD count;
	BYTE delay;
} ss8806;

void mapInit_Jaleco(BYTE model);

void extclCpuWrMem_Jaleco_JF05(WORD address, BYTE value);

void extclCpuWrMem_Jaleco_JF11(WORD address, BYTE value);

void extclCpuWrMem_Jaleco_JF13(WORD address, BYTE value);

void extclCpuWrMem_Jaleco_JF16(WORD address, BYTE value);

void extclCpuWrMem_Jaleco_JF17(WORD address, BYTE value);

void extclCpuWrMem_Jaleco_JF19(WORD address, BYTE value);

void extclCpuWrMem_Jaleco_SS8806(WORD address, BYTE value);
BYTE extclSaveMapper_Jaleco_SS8806(BYTE mode, BYTE slot, FILE *fp);
void extclCPUEveryCycle_Jaleco_SS8806(void);

#endif /* MAPPERJALECO_H_ */
