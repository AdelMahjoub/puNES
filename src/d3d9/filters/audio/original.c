/*
 * original.c
 *
 *  Created on: 11/lug/2012
 *      Author: fhorse
 */

#include "audio_quality.h"
#include "apu.h"
#include "snd.h"
#include "mappers.h"
#include "mappers/mapper_VRC7_snd.h"
#include "fds.h"
#include "cfg_file.h"
#include "clock.h"
#include "fps.h"
#include "original.h"

#define mixer_cut_and_high() mixer *= 45

SWORD (*extra_mixer_original)(SWORD mixer);
SWORD mixer_original_FDS(SWORD mixer);
SWORD mixer_original_MMC5(SWORD mixer);
SWORD mixer_original_Namco_N163(SWORD mixer);
SWORD mixer_original_Sunsoft_FM7(SWORD mixer);
SWORD mixer_original_VRC6(SWORD mixer);
SWORD mixer_original_VRC7(SWORD mixer);

BYTE audio_quality_init_original(void) {
	audio_quality_quit = audio_quality_quit_original;

	snd_apu_tick = audio_quality_apu_tick_original;

	init_nla_table(502, 522)

	switch (info.mapper) {
		case FDS_MAPPER:
			/* FDS */
			extra_mixer_original = mixer_original_FDS;
			break;
		case 5:
			/* MMC5 */
			extra_mixer_original = mixer_original_MMC5;
			break;
		case 19:
			/* Namcot N163 */
			extra_mixer_original = mixer_original_Namco_N163;
			break;
		case 69:
			/* Sunsoft FM7 */
			extra_mixer_original = mixer_original_Sunsoft_FM7;
			break;
		case 24:
		case 26:
			/* VRC6 */
			extra_mixer_original = mixer_original_VRC6;
			break;
		case 85:
			/* VRC7 */
			extra_mixer_original = mixer_original_VRC7;
			break;
		default:
			extra_mixer_original = NULL;
			break;
	}

	return (EXIT_OK);
}
void audio_quality_quit_original(void) {
	return;
}
void audio_quality_apu_tick_original(void) {
	_callback_data *cache = snd.cache;

	//return;

	if (!cfg->audio) {
		return;
	}

	//if (snd.brk) {
	//	if (cache->filled < 3) {
	//		snd.brk = FALSE;
	//	} else {
	//		return;
	//	}
	//}

	if ((snd.pos.current = snd.cycles++ / snd.frequency) == snd.pos.last) {
		return;
	}

	/*
	 * se la posizione e' maggiore o uguale al numero
	 * di samples che compongono il frame, vuol dire che
	 * sono passato nel frame successivo.
	 */
	if (snd.pos.current >= SND_BUFFER_SAMPLES) {
		/* azzero posizione e contatore dei cicli del frame audio */
		snd.pos.current = snd.cycles = 0;

		//SDL_mutexP(cache->lock);

		++cache->filled;

		/* incremento il contatore dei frames pieni non ancora 'riprodotti' */
		//if (++cache->filled >= snd.buffer.count) {
		//	snd.brk = TRUE;
		//} else if (cache->filled >= ((snd.buffer.count >> 1) + 1)) {
		//	snd_frequency(snd_factor[apu.type][SND_FACTOR_NONE])
		//} else if (cache->filled < 3) {
		//	snd_frequency(snd_factor[apu.type][SND_FACTOR_NORMAL])
		//}

		//SDL_mutexV(cache->lock);
	}

	{
		SWORD mixer = 0;
		mixer += nla_table.pulse[S1.output + S2.output];
		mixer += nla_table.tnd[(TR.output * 3) + (NS.output * 2) + DMC.output];

		if (extra_mixer_original) {
			mixer = extra_mixer_original(mixer);
		} else {
			mixer_cut_and_high();
		}

		/* mono or left*/
		(*cache->write++) = mixer;

		/* stereo */
		if (cfg->channels == STEREO) {
			/* salvo il dato nel buffer del canale sinistro */
			snd.channel.ptr[CH_LEFT][snd.channel.pos] = mixer;
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

		snd.pos.last = snd.pos.current;
	}

	return;
}

/* --------------------------------------------------------------------------------------- */
/*                                   Extra Audio Mixer                                     */
/* --------------------------------------------------------------------------------------- */
SWORD mixer_original_FDS(SWORD mixer) {
	mixer_cut_and_high();

	//return (mixer + (fds.snd.main.output + (fds.snd.main.output >> 1)));
	return (mixer + fds.snd.main.output);
}
SWORD mixer_original_MMC5(SWORD mixer) {
	mixer += (((mmc5.S3.output + mmc5.S4.output) + mmc5.pcm.output) << 3);

	mixer_cut_and_high();

	return (mixer);
}
SWORD mixer_original_Namco_N163(SWORD mixer) {
	BYTE i;
	SWORD a = 0;

	for (i = n163.snd_ch_start; i < 8; i++) {
		if (n163.ch[i].active) {
			a += ((n163.ch[i].output << 1) * (n163.ch[i].volume >> 2));
		}
	}

	mixer += a;

	mixer_cut_and_high();

	return (mixer);
}
SWORD mixer_original_Sunsoft_FM7(SWORD mixer) {
	mixer += ((fm7.square[0].output + fm7.square[1].output + fm7.square[2].output) << 3);

	mixer_cut_and_high();

	return (mixer);
}
SWORD mixer_original_VRC6(SWORD mixer) {
	mixer += (((vrc6.S3.output << 1) + (vrc6.S4.output << 1) + (vrc6.saw.output / 5)) << 2);

	mixer_cut_and_high();

	return (mixer);
}
SWORD mixer_original_VRC7(SWORD mixer) {
	mixer_cut_and_high();

	return (mixer + (opll_calc() << 2));
}
