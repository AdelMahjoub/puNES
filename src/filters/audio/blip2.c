/*
 * blip2.c
 *
 *  Created on: 28/lug/2012
 *      Author: fhorse
 */

#include <string.h>
#include "audio_quality.h"
#include "apu.h"
#include "sdlsnd.h"
#include "mappers.h"
#include "mappers/mapperVRC7snd.h"
#include "fds.h"
#include "blip2.h"

#include "cfgfile.h"
#include "clock.h"
#include "fps.h"
#include "blip_buf.h"

enum { master_vol = 65536 / 15 , volume_fator = 1, min_period = 20 };
enum { PULSE, TND };

#define update_amp_bl2(blt, grp, new_amp)\
{\
	int delta = new_amp * bl2.group[grp].gain - bl2.group[grp].amp;\
	bl2.group[grp].amp += delta;\
	blip_add_delta(bl2.blt, bl2.group[grp].time, delta);\
}
#define update_general_channel_bl2(blt, grp, restart, out)\
{\
	SWORD output = out;\
	bl2.group[grp].time += bl2.group[grp].period;\
	update_amp_bl2(blt, grp, output)\
	bl2.group[grp].period = restart;\
}
#define update_end_frame_channel_bl2(blt, grp, out)\
{\
	update_general_channel_bl2(blt, grp, 0, out);\
	bl2.group[grp].time -= bl2.counter;\
}

typedef struct blip2_chan _blip2_chan;
typedef struct af_blip2 _af_blip2;

struct blip2_chan {
	int gain; /* overall volume of channel */
	int time; /* clock time of next delta */
	int phase; /* position within waveform */
	int amp; /* current amplitude in delta buffer */

	int period;
	int min_period;
};
struct af_blip2 {
	DBWORD counter;
	blip_buffer_t *pulse;
	blip_buffer_t *tnd;
	_blip2_chan group[2];
	_blip2_chan ch[APU_TOT_CH];
} bl2;

struct _af_table_approx {
	SWORD pulse[32];
	SWORD tnd[203];
} af_table_approx;

BYTE audio_quality_init_blip2(void) {
	memset(&bl2, 0, sizeof(bl2));

	audio_quality_quit = audio_quality_quit_blip2;

	snd_apu_tick = audio_quality_apu_tick_blip2;
	snd_end_frame = audio_quality_end_frame_blip2;

	{
		WORD i;

		for (i = 0; i < LENGTH(af_table_approx.pulse); i++) {
			double vl = 95.52 / (8128.0 / (double) i + 100.0);
			af_table_approx.pulse[i] = (vl * 512);
		}

		for (i = 0; i < LENGTH(af_table_approx.tnd); i++) {
			double vl = 163.67 / (24329.0 / (double) i + 100.0);
			af_table_approx.tnd[i] = (vl * 512);
		}
	}

	{
		SDL_AudioSpec *dev = snd.dev;

		bl2.pulse = blip_new(dev->freq / 10);
		bl2.tnd = blip_new(dev->freq / 10);

		if (bl2.pulse == NULL) {
			 /* out of memory */
			return (EXIT_ERROR);
		}

		blip_set_rates(bl2.pulse, machine.cpuHz, dev->freq);
		blip_set_rates(bl2.tnd, machine.cpuHz, dev->freq);

		bl2.group[PULSE].gain = master_vol * (1.0 * volume_fator) / 100;
		bl2.group[TND].gain = master_vol * (1.0 * volume_fator) / 100;

		bl2.group[PULSE].min_period  = min_period;
		bl2.group[TND].min_period  = min_period / 2.5;
	}

	return (EXIT_OK);
}
void audio_quality_quit_blip2(void) {
	if (bl2.pulse) {
		blip_delete(bl2.pulse);
		bl2.pulse = NULL;
	}
	if (bl2.tnd) {
		blip_delete(bl2.tnd);
		bl2.tnd = NULL;
	}
}
void audio_quality_apu_tick_blip2(void) {
	if (!bl2.pulse || !bl2.tnd) {
		return;
	}

	if ((S1.clocked | S2.clocked) && (bl2.group[PULSE].period >= bl2.group[PULSE].min_period)) {
		S1.clocked = S2.clocked = FALSE;
		update_general_channel_bl2(pulse, PULSE, 1, af_table_approx.pulse[S1.output + S2.output])
	} else {
		bl2.group[PULSE].period++;
	}

	if ((TR.clocked | NS.clocked | DMC.clocked)
			&& (bl2.group[TND].period >= bl2.group[TND].min_period)) {
		TR.clocked = NS.clocked = DMC.clocked = FALSE;
		update_general_channel_bl2(tnd, TND, 1,
				af_table_approx.tnd[(3 * TR.output) + (2 * NS.output) + DMC.output])
	} else {
		bl2.group[TND].period++;
	}

	bl2.counter++;
}
void audio_quality_end_frame_blip2(void) {
	SDL_AudioSpec *dev = snd.dev;
	_callbackData *cache = snd.cache;

	if (!bl2.pulse || !bl2.tnd) {
		return;
	}

	update_end_frame_channel_bl2(pulse, PULSE, af_table_approx.pulse[S1.output + S2.output])
	update_end_frame_channel_bl2(tnd, TND,
			af_table_approx.tnd[(3 * TR.output) + (2 * NS.output) + DMC.output])

	blip_end_frame(bl2.pulse, bl2.counter);
	blip_end_frame(bl2.tnd, bl2.counter);
	bl2.counter = 0;

	{
		int i, count = blip_samples_avail(bl2.pulse);
		short pulse[count], tnd[count];

		blip_read_samples(bl2.pulse, pulse, count, 0);
		blip_read_samples(bl2.tnd, tnd, count, 0);

		if (snd.brk) {
			if (cache->filled < 3) {
				snd.brk = FALSE;
			} else {
				return;
			}
		}

		for (i = 0; i < count; i++) {
			SWORD data = pulse[i] + tnd[i];

			/* mono or left*/
			(*cache->write++) = data;

			/* stereo */
			if (dev->channels == STEREO) {
				/* salvo il dato nel buffer del canale sinistro */
				snd.channel.ptr[CH_LEFT][snd.channel.pos] = data;
				/* scrivo nel nel frame audio il canale destro ritardato di un frame */
				(*cache->write++) = snd.channel.ptr[CH_RIGHT][snd.channel.pos];
				/* swappo i buffers dei canali */
				if (++snd.channel.pos >= snd.channel.max_pos) {
					SWORD *swap = snd.channel.ptr[CH_RIGHT];

					snd.channel.ptr[CH_RIGHT] = snd.channel.ptr[CH_LEFT];
					snd.channel.ptr[CH_LEFT] = swap;
					snd.channel.pos = 0;
				}
			}

			if (cache->write >= (SWORD *) cache->end) {
				cache->write = cache->start;
			}

			if (++snd.pos.current >= dev->samples) {
				snd.pos.current = 0;

				SDL_mutexP(cache->lock);

				/* incremento il contatore dei frames pieni non ancora 'riprodotti' */
				if (++cache->filled >= snd.buffer.count) {
					snd.brk = TRUE;
				} else if (cache->filled >= ((snd.buffer.count >> 1) + 1)) {
					snd_frequency(sndFactor[apu.type][FCNONE])
				} else if (cache->filled < 3) {
					snd_frequency(sndFactor[apu.type][FCNORMAL])
				}

				SDL_mutexV(cache->lock);
			}
		}
	}
}
