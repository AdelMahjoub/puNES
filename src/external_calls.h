/*
 * external_calls.h
 *
 *  Created on: 03/set/2011
 *      Author: fhorse
 */

#ifndef EXTERNAL_CALLS_H_
#define EXTERNAL_CALLS_H_

#include <stdio.h>
#include "common.h"

/* mappers */
#define EXTCL_CPU_WR_MEM(n) extcl_cpu_wr_mem = extcl_cpu_wr_mem_##n
#define EXTCL_CPU_RD_MEM(n) extcl_cpu_rd_mem = extcl_cpu_rd_mem_##n
#define EXTCL_SAVE_MAPPER(n) extcl_save_mapper = extcl_save_mapper_##n

/* CPU */
#define EXTCL_CPU_EVERY_CYCLE(n) extcl_cpu_every_cycle = extcl_cpu_every_cycle_##n
#define EXTCL_CPU_WR_R4016(n) extcl_cpu_wr_r4016 = extcl_cpu_wr_r4016_##n

/* PPU */
#define EXTCL_PPU_000_TO_34X(n) extcl_ppu_000_to_34x = extcl_ppu_000_to_34x_##n
#define EXTCL_PPU_000_TO_255(n) extcl_ppu_000_to_255 = extcl_ppu_000_to_255_##n
#define EXTCL_PPU_256_TO_319(n) extcl_ppu_256_to_319 = extcl_ppu_256_to_319_##n
#define EXTCL_PPU_320_TO_34X(n) extcl_ppu_320_to_34x = extcl_ppu_320_to_34x_##n
#define EXTCL_UPDATE_R2006(n) extcl_update_r2006 = extcl_update_r2006_##n
#define EXTCL_AFTER_RD_CHR(n) extcl_after_rd_chr = extcl_after_rd_chr_##n
#define EXTCL_RD_NMT(n) extcl_rd_nmt = extcl_rd_nmt_##n
#define EXTCL_RD_CHR(n) extcl_rd_chr = extcl_rd_chr_##n
#define EXTCL_WR_CHR(n) extcl_wr_chr = extcl_wr_chr_##n

/* APU */
#define EXTCL_LENGTH_CLOCK(n) extcl_length_clock = extcl_length_clock_##n
#define EXTCL_ENVELOPE_CLOCK(n) extcl_envelope_clock = extcl_envelope_clock_##n
#define EXTCL_APU_TICK(n) extcl_apu_tick = extcl_apu_tick_##n

/* irqA12 */
#define EXTCL_IRQ_A12_CLOCK(n) extcl_irq_A12_clock = extcl_irq_A12_clock_##n

/* battery */
#define EXTCL_BATTERY_IO(n) extcl_battery_io = extcl_battery_io_##n

/* snd */
#define EXTCL_SND_START(n) extcl_snd_start = extcl_snd_start_##n

void extcl_init(void);

/* mappers */
void (*extcl_cpu_wr_mem)(WORD address, BYTE value);
BYTE (*extcl_cpu_rd_mem)(WORD address, BYTE openbus, BYTE before);
BYTE (*extcl_save_mapper)(BYTE mode, BYTE slot, FILE *fp);

/* CPU */
void (*extcl_cpu_every_cycle)(void);
/* viene chiamata ogni volta si scrive qualcosa nel registro $4016 */
void (*extcl_cpu_wr_r4016)(BYTE value);

/* PPU */
/* viene chiamata sempre, ad ogni ciclo della PPU */
void (*extcl_ppu_000_to_34x)(void);
/*
 * viene chiamata se (!r2002.vblank && (ppu.screenY < SCRLINES))
 * quindi per essere sicuri di essere durante il rendering della PPU
 * nella funzione devo controllare anche se r2001.visible non e' a zero.
 */
void (*extcl_ppu_000_to_255)(void);
/*
 * vengono chiamate solo se la PPU e' in fase di rendering
 * (!r2002.vblank && r2001.visible && (ppu.screenY < SCRLINES))
 */
void (*extcl_ppu_256_to_319)(void);
void (*extcl_ppu_320_to_34x)(void);
/* viene chiamata dopo ogni cambiamento del $2006 in cpu_inline.h */
void (*extcl_update_r2006)(WORD old_r2006);
/* vengono chiamate in ppuinline.h */
BYTE (*extcl_rd_nmt)(WORD address);
BYTE (*extcl_rd_chr)(WORD address);
/* viene chiamata dopo il FETCHB e dopo il fetch dello sprite */
void (*extcl_after_rd_chr)(WORD address);
/* viene chiamato quando si tenta di scrivere nella CHR Ram */
void (*extcl_wr_chr)(WORD address, BYTE value);

/* APU */
void (*extcl_length_clock)(void);
void (*extcl_envelope_clock)(void);
void (*extcl_apu_tick)(void);

/* irqA12 */
void (*extcl_irq_A12_clock)(void);

/* battery */
void (*extcl_battery_io)(BYTE mode, FILE *fp);

/* snd */
void (*extcl_snd_start)(WORD samplarate);

#endif /* EXTERNAL_CALLS_H_ */
