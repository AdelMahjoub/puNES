/*
 * audio_filter.c
 *
 *  Created on: 11/lug/2012
 *      Author: fhorse
 */

#include "audio_filter.h"
#include "cfgfile.h"
#include "apu.h"
#include "mappers.h"
#include "mappers/mapperVRC7snd.h"
#include "fds.h"
#include "filters/audio/original.h"
#include "filters/audio/approximation.h"
#include "filters/audio/linear.h"
#include "filters/audio/linear2.h"

#include "sdlsnd.h"

void audio_filter(BYTE filter) {

	snd_write = sndWrite;

	switch (filter) {
		default:
		case AF_ORIGINAL:
			audio_filter_init = audio_filter_init_original;
			break;
		case AF_APPROXIMATION:
			audio_filter_init = audio_filter_init_approximation;

			audio_filter_init = audio_filter_init_linear2;
			snd_write = audio_filter_snd_write_linear2;
			break;
		case AF_LINEAR:
			audio_filter_init = audio_filter_init_linear;
			break;
	}
	audio_filter_init();
}
void audio_filter_popolate_table_approx(void) {
	WORD i;

	for (i = 0; i < LENGTH(af_table_approx.pulse); i++) {
		float vl = 95.52 / (8128.0 / i + 100);
		//af_table_approx.pulse[i] = (vl * 0x1C00) - 0x0E00;
		af_table_approx.pulse[i] = (vl * 0x2FFF);
	}

	for (i = 0; i < LENGTH(af_table_approx.tnd); i++) {
		float vl = 163.67 / (24329.0 / i + 100);
		af_table_approx.tnd[i] = (vl * 0x2FFF);
	}
}
/*
 * con questa routine azzero tutti gli output dei possibili canali.
 * La utilizzo dopo un caricamento di un savestate (perche' alcuni
 * vecchi savestate possono ancora contenere valori di output
 * negativi [ormai obsoleti], che potrebbe mandare in crash alcuni
 * filtri [es. linear]) o di una timeline.
 */
void audio_filter_reset_output_channels(void) {
	S1.output = 0;
	S2.output = 0;
	TR.output = 0;
	NS.output = 0;
	DMC.output = 0;

	fds.snd.main.output = 0;

	mmc5.S3.output = 0;
	mmc5.S4.output = 0;
	mmc5.pcmSample = 0;

	fm7.square[0].output = 0;
	fm7.square[1].output = 0;
	fm7.square[2].output = 0;

	n163.ch[0].output = 0;
	n163.ch[1].output = 0;
	n163.ch[2].output = 0;
	n163.ch[3].output = 0;
	n163.ch[4].output = 0;
	n163.ch[5].output = 0;
	n163.ch[6].output = 0;
	n163.ch[7].output = 0;

	vrc6.S3.output = 0;
	vrc6.S4.output = 0;
	vrc6.saw.output = 0;
}
