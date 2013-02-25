/*
 * cmd_line.c
 *
 *  Created on: 03/ago/2011
 *      Author: fhorse
 */

#include <getopt.h>
#include <string.h>
#include <libgen.h>
#include "cmd_line.h"
#include "cfg_file.h"
#include "sdlgfx.h"
#include "version.h"
#include "emu.h"
#include "gamegenie.h"
#include "opengl.h"
#define __CMDLINE__
#include "param.h"

void usage(char *name);

BYTE cmd_line_parse(int argc, char **argv) {
	int longIndex = 0, opt = 0;

	opt = getopt_long(argc, argv, optShort, optLong, &longIndex);
	while (opt != -1) {
		switch (opt) {
			case 0:
				/* long options */
				if (!(strcmp(optLong[longIndex].name, "swap-duty"))) {
					param_search(0, optarg, param_no_yes, cfg_from_file.swap_duty = index);
				}
				break;
			case 'a':
				param_search(0, optarg, param_off_on, cfg_from_file.audio = index);
				break;
			case 'c':
				param_search(0, optarg, param_channels, cfg_from_file.channels = index);
				break;
			case 'f':
				param_search(0, optarg, param_fps, cfg_from_file.fps = index);
				break;
			case 'g':
				param_search(0, optarg, param_no_yes, cfg_from_file.gamegenie = index);
				break;
			case 'h':
			case '?':
				usage(basename(argv[0]));
				break;
			case 'V': {
				if (!info.portable) {
					fprintf(stdout, "%s %s\n", NAME, VERSION);
				} else {
					fprintf(stdout, "Portable %s %s\n", NAME, VERSION);
				}
				emuQuit(EXIT_SUCCESS);
				break;
			}
			case 'k':
				param_search(0, optarg, param_fsk, cfg_from_file.frameskip = index);
				break;
			case 'i':
				param_search(0, optarg, param_filter, cfg_from_file.filter = index);
				break;
			case 'l':
				param_search(0, optarg, param_samplerate, cfg_from_file.samplerate = index);
				break;
			case 'm':
				param_search(0, optarg, param_mode, cfg_from_file.mode = index);
				break;
			case 'n':
				param_search(0, optarg, param_ntsc, cfg_from_file.ntsc_format = index);
				break;
			case 'o':
				param_search(0, optarg, param_oscan, cfg_from_file.oscan = index);
				break;
			case 'p':
				param_search(0, optarg, param_ntsc, cfg_from_file.palette = index);
				break;
			case 'q':
				param_search(0, optarg, param_audio_quality, cfg_from_file.audio_quality = index);
				break;
			case 's':
				param_search(1, optarg, param_size, cfg_from_file.scale = index);
				gfx.scale_before_fscreen = cfg_from_file.scale;
				break;
			case 'r':
				param_search(0, optarg, param_render, cfg_from_file.render = index);
				gfxSetRender(cfg_from_file.render);
				break;
			case 'v':
				param_search(0, optarg, param_off_on, cfg_from_file.vsync = index);
				break;
			case 't':
				param_search(0, optarg, param_no_yes, cfg_from_file.aspect_ratio = !index);
				break;
			case 'u':
				param_search(0, optarg, param_no_yes, cfg_from_file.fullscreen = index);
				break;
			default:
				break;
		}

		opt = getopt_long(argc, argv, optShort, optLong, &longIndex);
	}
	return (optind);
}
void usage(char *name) {
	char *istructions = { "Usage: %s [options] file...\n\n"
			"Options:\n"
			"-h, --help                print this help\n"
			"-V, --version             print the version\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
			"%s\n"
	};
	fprintf(stdout, istructions, name,
			param[P_MODE].help,
			param[P_FPS].help,
	        param[P_FSK].help,
	        param[P_SIZE].help,
	        param[P_OVERSCAN].help,
	        param[P_FILTER].help,
	        param[P_NTSCFORMAT].help,
	        param[P_PALETTE].help,
			param[P_RENDER].help,
			param[P_VSYNC].help,
			param[P_FSCREEN].help,
			param[P_STRETCH].help,
			param[P_AUDIO].help,
			param[P_SAMPLERATE].help,
			param[P_CHANNELS].help,
			param[P_AUDIO_QUALITY].help,
			param[P_SWAP_DUTY].help,
			param[P_GAMEGENIE].help
	);
	emuQuit(EXIT_SUCCESS);
}
