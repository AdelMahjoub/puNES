/*
 * main.c
 *
 *  Created on: 21/lug/2011
 *      Author: fhorse
 */

#include <string.h>
#include "main.h"
#include "emu.h"
#include "memmap.h"
#include "sdlgfx.h"
#include "sdltext.h"
#include "cfgfile.h"
#include "cmdline.h"
#include "timeline.h"
#include "version.h"
#include "gui.h"

#include "gamegenie.h"

#if defined MINGW32 || defined MINGW64
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
		PSTR szCmdLine, int iCmdShow) {
	int argc = __argc;
	char **argv = (char **)__argv;
#if !defined SDL
	gui.mainhInstance = hInstance;
#endif
#else
int main(int argc, char **argv) {
#endif
	BYTE optind;

	memset(&info, 0x00, sizeof(info));
	info.machine = info.machineDb = DEFAULT;

	guiInit(argc, argv);

#define controlDir(fld, txt)\
{\
	char file[512];\
	sprintf(file, fld, info.baseFolder);\
	if (emuMakeDir(file)) {\
		fprintf(stderr, txt);\
		return (EXIT_ERROR);\
	}\
}

	/* controllo l'esistenza della directory principale */
	if (emuMakeDir(info.baseFolder)) {
		fprintf(stderr, "error on create puNES folder\n");
		return (EXIT_ERROR);
	}
	/* creo le sottocartelle */
	controlDir("%s" SAVEFOLDER, "error on create save folder\n")
	controlDir("%s" PERGAMEFOLDER, "error on create psg folder\n")
	controlDir("%s" BIOSFOLDER, "error on create bios folder\n")
	controlDir("%s" DIFFFOLDER, "error on create diff folder\n")

#ifdef __NETPLAY__
	netplay_init();
#endif

	gamegenie_init();

	textInit();

	textAddLineInfo(1,"[yellow]p[red]u[green]N[cyan]E[brown]S[normal]"
	        " [font8](by [cyan]FHorse[normal]) [font12]%s", VERSION);

	/*
	 * tratto il file di configurazione ed
	 * i parametri passati dalla riga di comando.
	 */
	cfgfileInit();
	cfgfileParse();
	optind = cmdlineParse(argc, argv);
	/* casi particolari */
	if ((gfx.scale == X1) && (gfx.filter != NOFILTER)) {
		gfx.scale = X2;
	}
#ifdef OPENGL
	if (gfx.fullscreen == FULLSCR) {
		if (!gfx.opengl) {
			gfx.fullscreen = NOFULLSCR;
		} else {
			gfx.scaleBeforeFullscreen = gfx.scale;
		}
	}
#endif

	if (argc == optind) {
#ifndef DEBUG
		//if(!info.gui) {
		//	strcpy(info.romFile, "rom.nes");
		//}
#else
		//strcpy(info.romFile, "/home/fhorse/sviluppo/personale/roms/85/Lagrange Point (J).nes");
#endif
	} else {
		strcpy(info.romFile, argv[optind]);
	}

	if (emuTurnON()) {
		emuQuit(EXIT_FAILURE);
	}

	guiStart();

	emuQuit(EXIT_SUCCESS);

	return (EXIT_SUCCESS);
}
