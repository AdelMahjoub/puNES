/*
 * snd.c
 *
 *  Created on: 29/lug/2014
 *      Author: fhorse
 */

#include <string.h>
#include <pthread.h>
#include <alsa/asoundlib.h>
#include "info.h"
#include "snd.h"
#include "clock.h"
#include "conf.h"
#include "fps.h"
#include "audio_quality.h"
#include "gui.h"

enum alsa_thread_loop_actions {
	AT_UNINITIALIZED,
	AT_RUN,
	AT_STOP,
	AT_PAUSE
};

static BYTE set_hwparams(void);
static BYTE set_swparams(void);
void *alsa_loop_thread(void *data);

typedef struct {
	snd_pcm_t *handle;

	snd_pcm_uframes_t bsize;
	snd_pcm_uframes_t psize;

	snd_output_t *output;

	uint32_t bytes_written;
} _alsa;
typedef struct {
	pthread_t thread;
	pthread_mutex_t lock;

	BYTE action;

	void *cache;
	_alsa *alsa;
} _thread;

_thread loop;
_alsa alsa;

BYTE snd_init(void) {
	memset(&snd, 0x00, sizeof(_snd));
	memset(&alsa, 0x00, sizeof(_alsa));
	memset(&loop, 0x00, sizeof(_thread));

	snd_apu_tick = NULL;
	snd_end_frame = NULL;

	/* apro e avvio la riproduzione */
	if (snd_start()) {
		return (EXIT_ERROR);
	}

	return (EXIT_OK);
}
BYTE snd_start(void) {
	_callback_data *cache;
	int rc;

	if (!cfg->apu.channel[APU_MASTER]) {
		return (EXIT_OK);
	}

	/* se il thread del loop e' gia' in funzione lo metto in pausa */
	if (loop.action == AT_RUN) {
		loop.action = AT_PAUSE;
		gui_sleep(200);

		snd_lock_cache(NULL);
	}

	/* come prima cosa blocco eventuali riproduzioni */
	snd_stop();

	memset(&snd, 0x00, sizeof(_snd));
	memset(&alsa, 0x00, sizeof(_alsa));

	cache = (_callback_data *) malloc(sizeof(_callback_data));
	memset(cache, 0x00, sizeof(_callback_data));
	snd.cache = cache;

 	{
		double latency = 200.0f;
 		double sample_latency;

 		switch (cfg->samplerate) {
			case S48000:
				snd.samplerate = 48000;
				snd.buffer.size = (48000 / (11025 / 128)) * 8;
				break;
			case S44100:
				snd.samplerate = 44100;
				snd.buffer.size = 512 * 8;
				break;
			case S22050:
				snd.samplerate = 22050;
				snd.buffer.size = 256 * 8;
				break;
			case S11025:
				snd.samplerate = 11025;
				snd.buffer.size = 128 * 8;
				break;
 		}

		if (cfg->channels == MONO) {
			latency *= 2.0f;
		}

		sample_latency = latency * (double) snd.samplerate * (double) cfg->channels / 1000.0f;
		snd.buffer.count = sample_latency / snd.buffer.size;
	}

	snd.frequency = ((fps.nominal * (double) machine.cpu_cycles_frame) / (double) snd.samplerate);
	snd.samples = snd.buffer.size / cfg->channels;
	snd.opened = TRUE;

	if (cfg->channels == STEREO) {
		BYTE i;

		snd.channel.max_pos = snd.samples * cfg->stereo_delay;
		snd.channel.pos = 0;

		for (i = 0; i < 2; i++) {
			DBWORD size = snd.samples * sizeof(*cache->write);

			snd.channel.buf[i] = (SWORD *) malloc(size);
			memset(snd.channel.buf[i], 0x00, size);
			snd.channel.ptr[i] = snd.channel.buf[i];

			snd.channel.bck.start = (SWORD *) malloc(size * 2);
			memset(snd.channel.bck.start, 0x00, size * 2);
			snd.channel.bck.write = snd.channel.bck.start;
			snd.channel.bck.middle = snd.channel.bck.start + snd.samples;
			snd.channel.bck.end = (SBYTE *) snd.channel.bck.start + (size * 2);
		}
	}

	snd_frequency(snd_factor[apu.type][SND_FACTOR_SPEED])

	{
		/* dimensione in bytes del buffer */
		DBWORD total_buffer_size = snd.buffer.size * snd.buffer.count * sizeof(*cache->write);

		//printf("snd.buffer.size   : %d\n", snd.buffer.size);
		//printf("snd.buffer.count  : %d\n", snd.buffer.count);
		//printf("total_buffer_size : %d\n", total_buffer_size);

		/* alloco il buffer in memoria */
		if (!(cache->start = (SWORD *) malloc(total_buffer_size))) {
			fprintf(stderr, "Unable to allocate audio buffers\n");
			goto snd_start_error;
		}

		if (!(cache->silence = (SWORD *) malloc(snd.buffer.size * sizeof(*cache->write)))) {
			fprintf(stderr, "Unable to allocate silence buffer\n");
			goto snd_start_error;
		}

		/* inizializzo il frame di scrittura */
		cache->write = cache->start;
		/* inizializzo il frame di lettura */
		cache->read = (SBYTE *) cache->start;
		/* punto alla fine del buffer */
		cache->end = cache->read + total_buffer_size;
		/* creo il lock */
		if (pthread_mutex_init(&loop.lock, NULL) != 0) {
			fprintf(stderr, "Unable to allocate the mutex\n");
			goto snd_start_error;
		}
		/* azzero completamente i buffers */
		memset(cache->start, 0x00, total_buffer_size);
		/* azzero completamente il buffer del silenzio */
		memset(cache->silence, 0x00, snd.buffer.size * sizeof(*cache->write));

		/* punto all'inizio del frame di scrittura */
		snd.pos.current = snd.pos.last = 0;

		cache->lock = (void *) &loop.lock;
	}

	if (extcl_snd_start) {
		extcl_snd_start((WORD) snd.samplerate);
	}

	audio_quality(cfg->audio_quality);

	if ((rc = snd_pcm_open(&alsa.handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		fprintf(stderr, "Playback open error: %s\n", snd_strerror(rc));
		goto snd_start_error;
	}
	if (set_hwparams() != EXIT_OK) {
		goto snd_start_error;
	}
	if (set_swparams() != EXIT_OK) {
		goto snd_start_error;
	}

	/* gestione del thread del looop */
	if (loop.action == AT_UNINITIALIZED) {
		loop.alsa = &alsa;
		loop.action = AT_PAUSE;

		/* creo il lock */
		if (pthread_mutex_init(&loop.lock, NULL) != 0) {
			fprintf(stderr, "Unable to allocate the thread mutex\n");
			goto snd_start_error;
		}

		snd_lock_cache(NULL);

		/* creo ed eseguo il thread */
		if ((rc = pthread_create(&loop.thread, NULL, alsa_loop_thread, (void *) &loop))) {
			fprintf(stderr, "Error - pthread_create() return code: %d\n", rc);
			goto snd_start_error;;
		}
	}

	loop.cache = cache;

	if ((rc = snd_pcm_prepare(alsa.handle)) < 0) {
		fprintf(stderr, "cannot prepare audio interface for use (%s)\n", snd_strerror(rc));
		goto snd_start_error;
	}

	snd_unlock_cache(NULL);
	gui_sleep(50);

	loop.action = AT_RUN;

	return (EXIT_OK);

	snd_start_error:
	snd_stop();
	return (EXIT_ERROR);
}
void snd_stereo_delay(void) {
	int i;
	_callback_data *cache = (_callback_data *) snd.cache;
	SWORD *here;

	snd.channel.max_pos = snd.samples * cfg->stereo_delay;
	snd.channel.pos = 0;

	for (i = 0; i < 2; i++) {
		snd.channel.ptr[i] = snd.channel.buf[i];
	}

	here = snd.channel.bck.write - snd.channel.max_pos;

	if (here >= snd.channel.bck.start) {
		memcpy(snd.channel.ptr[CH_RIGHT], here, snd.channel.max_pos * sizeof(*cache->write));
	} else {
		DBWORD step = snd.channel.bck.start - here;
		SWORD *src1 = (SWORD *) snd.channel.bck.end - step;
		SWORD *src2 = snd.channel.bck.start;
		SWORD *dst1 = snd.channel.ptr[CH_RIGHT];
		SWORD *dst2 = snd.channel.ptr[CH_RIGHT] + step;

		memcpy(dst1, src1, step * sizeof(*cache->write));
		memcpy(dst2, src2, (snd.channel.max_pos - step) * sizeof(*cache->write));
	}
}
void snd_output(void *udata, BYTE *stream, int len) {

}
void snd_lock_cache(_callback_data *cache) {
	pthread_mutex_lock(&loop.lock);
}
void snd_unlock_cache(_callback_data *cache) {
	pthread_mutex_unlock(&loop.lock);
}
void snd_stop(void) {
	snd.opened = FALSE;
	gui_sleep(100);

	if (snd.cache) {
		_callback_data *cache = (_callback_data *) snd.cache;

		if (cache->start) {
			free(cache->start);
		}

		if (cache->silence) {
			free(cache->silence);
		}

		free(snd.cache);
		snd.cache = NULL;
	}

	{
		BYTE i;

		if (snd.channel.bck.start) {
			free(snd.channel.bck.start);
			snd.channel.bck.start = NULL;
		}

		for (i = 0; i < STEREO; i++) {
			/* rilascio la memoria */
			if (snd.channel.buf[i]) {
				free(snd.channel.buf[i]);
			}
			/* azzero i puntatori */
			snd.channel.ptr[i] = snd.channel.buf[i] = NULL;
		}
	}

	if (audio_quality_quit) {
		audio_quality_quit();
	}

	if (alsa.handle) {
		snd_pcm_close(alsa.handle);
		alsa.handle = NULL;
	}
}
void snd_quit(void) {
	if (loop.action != AT_UNINITIALIZED) {
		loop.action = AT_STOP;

		pthread_join(loop.thread, NULL);
    	pthread_mutex_destroy(&loop.lock);
    	memset(&loop, 0x00, sizeof(_thread));
    }

	snd_stop();

#if !defined (RELEASE)
	fprintf(stderr, "\n");
#endif
}

static BYTE set_hwparams(void) {
	snd_pcm_uframes_t size;
	snd_pcm_hw_params_t *params = NULL;
	unsigned int rrate;
	int rc, dir;

	snd_pcm_hw_params_alloca(&params);

	/* choose all parameters */
	if ((rc = snd_pcm_hw_params_any(alsa.handle, params)) < 0) {
		fprintf(stderr, "Broken configuration for playback: no configurations available: %s\n",
				snd_strerror(rc));
		return (EXIT_ERROR);
	}
	/* anable hardware resampling */
	if ((rc = snd_pcm_hw_params_set_rate_resample(alsa.handle, params, 1)) < 0) {
		fprintf(stderr, "Resampling setup failed for playback: %s\n", snd_strerror(rc));
		return (EXIT_ERROR);
	}
	/* set the interleaved read/write format */
	if ((rc = snd_pcm_hw_params_set_access(alsa.handle, params, SND_PCM_ACCESS_RW_INTERLEAVED))
			< 0) {
		fprintf(stderr, "Access type not available for playback: %s\n", snd_strerror(rc));
		return (EXIT_ERROR);
	}
	/* set the sample format */
	if ((rc = snd_pcm_hw_params_set_format(alsa.handle, params, SND_PCM_FORMAT_S16)) < 0) {
		fprintf(stderr, "Sample format not available for playback: %s\n", snd_strerror(rc));
		return (EXIT_ERROR);
	}
	/* set the channels */
	if ((rc = snd_pcm_hw_params_set_channels(alsa.handle, params, cfg->channels)) < 0) {
		fprintf(stderr, "Channels count (%i) not available for playbacks: %s\n", cfg->channels,
				snd_strerror(rc));
		return (EXIT_ERROR);
	}
	/* set the stream rate */
	rrate = snd.samplerate;

	if ((rc = snd_pcm_hw_params_set_rate_near(alsa.handle, params, &rrate, 0)) < 0) {
		fprintf(stderr, "Rate %iHz not available for playback: %s\n", snd.samplerate,
				snd_strerror(rc));
		return (EXIT_ERROR);
	}
	if (rrate != snd.samplerate) {
		fprintf(stderr, "Rate doesn't match (requested %iHz, get %iHz)\n", snd.samplerate, rrate);
		return (EXIT_ERROR);
	}
	/* set the buffer size */
	alsa.bsize = snd.samples * snd.buffer.count;

	if ((rc = snd_pcm_hw_params_set_buffer_size(alsa.handle, params, alsa.bsize)) < 0) {
		fprintf(stderr, "Unable to set buffer size for playback: %s. Use the nearest.\n",
				snd_strerror(rc));

		printf("Buffer Size Request  : %ld\n", alsa.bsize);

		if ((rc = snd_pcm_hw_params_set_buffer_size_near(alsa.handle, params, & alsa.bsize)) < 0) {
			fprintf(stderr, "Unable to set buffer size for playback: %s\n", snd_strerror(rc));
			return (EXIT_ERROR);
		}

		snd_pcm_hw_params_get_buffer_size_min(params, &size);
		printf("Buffer Size Min      : %ld\n", size);
		snd_pcm_hw_params_get_buffer_size_max(params, &size);
		printf("Buffer Size Max      : %ld\n", size);
	}
	/* set the period size */
	alsa.psize = (snd.samples / 2);

	if ((rc = snd_pcm_hw_params_set_period_size(alsa.handle, params, alsa.psize, 0)) < 0) {
		fprintf(stderr, "Unable to set period size for playback: %s. Use the nearest\n",
				snd_strerror(rc));

		printf("Period Size Request  : %ld\n", alsa.psize);

		alsa.psize = (snd.samples / 3) * 2;
		dir = 0;

		if ((rc = snd_pcm_hw_params_set_period_size_near(alsa.handle, params, &alsa.psize, &dir))
				< 0) {
			fprintf(stderr, "Unable to set period size for playback: %s\n", snd_strerror(rc));
			return (EXIT_ERROR);
		}

		snd_pcm_hw_params_get_period_size_min(params, &size, &dir);
		printf("Period Size Min      : %ld\n", size);
		snd_pcm_hw_params_get_period_size_max(params, &size, &dir);
		printf("Period Size Max      : %ld\n", size);
	}
	/* write the parameters to device */
	if ((rc = snd_pcm_hw_params(alsa.handle, params)) < 0) {
		fprintf(stderr, "Unable to set hw params for playback: %s\n", snd_strerror(rc));
		return (EXIT_ERROR);
	}
	return (EXIT_OK);
}
static BYTE set_swparams(void) {
	snd_pcm_sw_params_t *params = NULL;
	int rc;

	snd_pcm_sw_params_alloca(&params);

	/* get the current swparams */
	if ((rc = snd_pcm_sw_params_current(alsa.handle, params)) < 0) {
		fprintf(stderr, "Unable to determine current swparams for playback: %s\n", snd_strerror(rc));
		return (EXIT_ERROR);
	}
	/* start the transfer when the buffer is almost full: */
	/* (buffer_size / avail_min) * avail_min */
	if ((rc = snd_pcm_sw_params_set_start_threshold(alsa.handle, params,
			(alsa.bsize / alsa.psize) * alsa.psize)) < 0) {
		fprintf(stderr, "Unable to set start threshold mode for playback: %s\n", snd_strerror(rc));
		return (EXIT_ERROR);
	}
	/* allow the transfer when at least period_size samples can be processed */
	if ((rc = snd_pcm_sw_params_set_avail_min(alsa.handle, params, alsa.psize)) < 0) {
		fprintf(stderr, "Unable to set avail min for playback: %s\n", snd_strerror(rc));
		return (EXIT_ERROR);
	}
	/* write the parameters to the playback device */
	if ((rc = snd_pcm_sw_params(alsa.handle, params)) < 0) {
		fprintf(stderr, "Unable to set sw params for playback: %s\n", snd_strerror(rc));
		return (EXIT_ERROR);
	}
	return (EXIT_OK);
}
void *alsa_loop_thread(void *data) {
	_thread *th = (_thread *) data;
	snd_pcm_sframes_t avail;
	uint32_t len;
	void *buffer = NULL;
	int rc;

	while (TRUE) {
		_callback_data *cache = (_callback_data *) th->cache;

		if (th->action == AT_STOP) {
			break;
		} else if ((th->action == AT_PAUSE) || (snd.opened == FALSE)) {
			gui_sleep(10);
			continue;
		}

		if ((rc = snd_pcm_wait(th->alsa->handle , 100)) < 0) {
			fprintf(stderr, "poll failed (%s)\n", strerror(errno));
			continue;
		}

		/* controllo quanti frames alsa sono richiesti */
		if ((avail = snd_pcm_avail_update(th->alsa->handle)) < 0) {
			if (avail == -EPIPE) {
				fprintf(stderr, "an xrun occured\n");
				continue;
			} else {
				fprintf(stderr, "unknown ALSA avail update return value (%ld)\n", avail);
				continue;
			}
		}

		snd_lock_cache(NULL);

#if !defined (RELEASE)
		//snd_pcm_sframes_t request = avail;
#endif

		avail = (avail > alsa.psize ? alsa.psize : avail);
		len = avail * cfg->channels * sizeof(*cache->write);

#if !defined (RELEASE)
		/*
		fprintf(stderr, "snd : %7ld %7ld %d %d %d %7d %d %d %d %f %f %4s\r",
				request,
				avail,
				len,
				snd.buffer.count,
				snd.brk,
				fps.total_frames_skipped,
				cache->bytes_available,
				cache->filled,
				snd.out_of_sync,
				snd.frequency,
				machine.ms_frame,
				"");
		*/
#endif

		if (info.no_rom) {
			buffer = cache->silence;
			memset(buffer, 0x00, len);
		} else if (cache->bytes_available < len) {
			snd.out_of_sync++;

			buffer = cache->silence;
			memset(buffer, 0x00, len);
		} else {
			if ((cache->read + len) >= cache->end) {
				uint32_t l1 = 0, l2 = 0;

				buffer = cache->silence;
				l1 = cache->end - cache->read;
				memcpy((BYTE *) buffer, cache->read, l1);

				cache->read = (SBYTE *) cache->start;
				l2 = len - l1;
				memcpy((BYTE *) (buffer + l1), cache->read, l2);

				cache->read += l2;
			} else {
				buffer = (void *) cache->read;

				cache->read += len;
			}

			if ((th->alsa->bytes_written += len) > (snd.buffer.size * sizeof(*cache->write))) {
				th->alsa->bytes_written -= (snd.buffer.size * sizeof(*cache->write));
				cache->filled--;
			}

			cache->bytes_available -= len;
		}

		if ((rc = snd_pcm_writei(th->alsa->handle, buffer, avail)) < 0) {
			fprintf(stderr, "write failed (%s)\n", snd_strerror(rc));
		}

		snd_unlock_cache(NULL);
 	}

	pthread_exit(EXIT_OK);
}
