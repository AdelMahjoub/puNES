/*
 * none.c
 *
 *  Created on: 11/lug/2012
 *      Author: fhorse
 */

#include "audio_filter.h"
#include "apu.h"
#include "mappers.h"
#include "mappers/mapperVRC7snd.h"
#include "fds.h"
#include "none.h"

/*
 * questa viene chiamata in ogni extclApuMixer chiamata dalle mappers
 * tranne che nel VRC7.
 */
#define mixer_cut_and_high()\
	/* taglio il risultato */\
	if (mixer > 255) {\
		mixer = 255;\
	} else if (mixer < -255) {\
		mixer = -255;\
	}\
	/* ne aumento il volume */\
	mixer <<= 7

SWORD (*extra_mixer_none)(SWORD mixer);
SWORD mixer_none_FDS(SWORD mixer);
SWORD mixer_none_MMC5(SWORD mixer);
SWORD mixer_none_Namco_N163(SWORD mixer);
SWORD mixer_none_Sunsoft_FM7(SWORD mixer);
SWORD mixer_none_VRC6(SWORD mixer);
SWORD mixer_none_VRC7(SWORD mixer);

void audio_filter_init_none(void) {
	audio_filter_apu_tick = audio_filter_apu_tick_none;
	audio_filter_apu_mixer = audio_filter_apu_mixer_none;

	extra_mixer_none = NULL;

	switch (info.mapper) {
		case FDS_MAPPER:
			/* FDS */
			extra_mixer_none = mixer_none_FDS;
			break;
		case 5:
			/* MMC5 */
			extra_mixer_none = mixer_none_MMC5;
			break;
		case 19:
			/* Namcot N163 */
			extra_mixer_none = mixer_none_Namco_N163;
			break;
		case 69:
			/* Sunsoft FM7 */
			extra_mixer_none = mixer_none_Sunsoft_FM7;
			break;
		case 24:
		case 26:
			/* VRC6 */
			extra_mixer_none = mixer_none_VRC6;
			break;
		case 85:
			/* VRC7 */
			extra_mixer_none = mixer_none_VRC7;
			break;
	}
	return;
}
void audio_filter_apu_tick_none(void) {
	return;
}
SWORD audio_filter_apu_mixer_none(void) {
	SWORD mixer = (S1.output + S2.output) + (TR.output + NS.output + DMC.output);

	if (extra_mixer_none) {
		mixer = extra_mixer_none(mixer);
	} else {
		mixer_cut_and_high();
	}

	return (mixer);
}

/* --------------------------------------------------------------------------------------- */
/*                                   Extra Audio Mixer                                     */
/* --------------------------------------------------------------------------------------- */
SWORD mixer_none_FDS(SWORD mixer) {
	mixer_cut_and_high();

	return (mixer + (fds.snd.main.output + (fds.snd.main.output >> 1)));
}
SWORD mixer_none_MMC5(SWORD mixer) {
	mixer += (mmc5.S3.output + mmc5.S4.output) + mmc5.pcmSample;

	mixer_cut_and_high();

	return (mixer);
}
SWORD mixer_none_Namco_N163(SWORD mixer) {
	BYTE i;
	SWORD a = 0;

	for (i = n163.sndChStart; i < 8; i++) {
		if (n163.ch[i].active) {
			a += ((n163.ch[i].output << 1) * (n163.ch[i].volume >> 2));
		}
	}

	mixer += a;

	mixer_cut_and_high();

	return (mixer);
}
SWORD mixer_none_Sunsoft_FM7(SWORD mixer) {
	mixer += ((fm7.square[0].output << 1) + (fm7.square[1].output << 1)
	        + (fm7.square[2].output << 1));

	mixer_cut_and_high();

	return (mixer);
}
SWORD mixer_none_VRC6(SWORD mixer) {
	mixer += (vrc6.S3.output << 1) + (vrc6.S4.output << 1) + (vrc6.saw.output / 5);

	mixer_cut_and_high();

	return (mixer);
}
SWORD mixer_none_VRC7(SWORD mixer) {
	mixer_cut_and_high();

	return (mixer + (opll_calc() << 2));
}
