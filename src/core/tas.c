/*
 *  Copyright (C) 2010-2016 Fabio Cavallo (aka FHorse)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>
#include "tas.h"
#include "text.h"
#include "emu.h"
#include "info.h"

#define tas_set_data_port_ctrlstd(prt, dt)\
		prt.data[dt] = tas.il[tas.index].prt[dt]
#define tas_increment_index()\
	if (++tas.index == tas.count) {\
		tas_read();\
	}

static _port tas_port_bck[PORT_MAX];

BYTE tas_file(char *ext, char *file) {
	if (!(strcasecmp(ext, ".fm2")) || !(strcasecmp(ext, ".FM2"))) {
		tas.type = FM2;
		tas_header = tas_header_FM2;
		tas_read = tas_read_FM2;
		tas_frame = tas_frame_FM2;
	}

	if (tas.type) {
		BYTE found = FALSE;

		{
			BYTE i;

			for (i = PORT1; i < PORT_MAX; i++) {
				memcpy(&tas_port_bck[i], &port[i], sizeof(_port));
			}
		}

		strncpy(tas.file, file, sizeof(tas.file));

		tas.fp = fopen(tas.file, "r");

		tas_header(file);

		{
			BYTE i;
			const char rom_ext[4][10] = { ".nes\0", ".NES\0", ".fds\0", ".FDS\0" };

			for (i = 0; i < LENGTH(rom_ext); i++) {
				char rom_file[LENGTH_FILE_NAME_MID];

				strncpy(rom_file, info.rom_file, sizeof(rom_file));
				strcat(rom_file, rom_ext[i]);

				if (emu_file_exist(rom_file) == EXIT_OK) {
					strncpy(info.rom_file, rom_file, sizeof(info.rom_file));
					found = TRUE;
					break;
				}
			}
		}

		if (found) {
			tas_read();
		} else {
			info.rom_file[0] = 0;
			tas_quit();
		}
	}
	return (EXIT_OK);
}
void tas_quit(void) {
	if (tas.fp) {
		fclose(tas.fp);
	}

	tas_header = NULL;
	tas_read = NULL;

	{
		BYTE i;

		for (i = PORT1; i < PORT_MAX; i++) {
			memcpy(&port[i], &tas_port_bck[i], sizeof(_port));
		}
	}

	input_init(NO_SET_CURSOR);

	tas.type = NOTAS;
}

void tas_frame_FM2(void) {
	/* il primo frame */
	if (!tas.frame) {
		/*
		 * sembra proprio che il mio reset / powerup
		 * duri un frame in meno rispetto all'fceux.
		 */
		if ((tas.emulator == FCEUX) && !tas.index) {
			text_add_line_info(1, "enabled FCEUX compatible mode");
			tas.add_fake_frame = TRUE;
		}
		text_add_line_info(1, "[yellow]silence, the movie has begun[normal]");
	}

	if (tas.il[tas.index].state > 0) {
		if (tas.il[tas.index].state == 1) {
			emu_reset(RESET);
		} else if (tas.il[tas.index].state == 2) {
			emu_reset(HARD);
		}
		tas.il[tas.index].state = 0;
		/*
		 * sembra proprio che il mio reset / powerup
		 * duri un frame in meno rispetto all'fceux.
		 */
		if (tas.emulator == FCEUX) {
			tas.add_fake_frame = TRUE;
		}
	}

	if (tas.add_fake_frame) {
		tas_increment_index()
		tas.frame++;
		tas.total_lag_frames++;
		tas.add_fake_frame = FALSE;
	}

	if (++tas.frame >= tas.total) {
		if (tas.frame == tas.total) {
			text_add_line_single(4, FONT_12X10, 200, TXT_CENTER, TXT_CENTER, 0, 0, "The End");
		} else if (tas.frame == tas.total + 1) {
			/*
			 * finito il video l'FCEUX lascia settato il primo frame non del film
			 * ai valori dell'ultimo frame del film. Se non lo faccio,
			 * aglar-marblemadness.fm2 non finira' il gioco.
			 */
			return;
		} else {
			tas_quit();
			return;
		}
	}

	{
		BYTE i;

		for (i = PORT1; i < PORT_MAX; i++) {
			if (port[i].type == CTRL_STANDARD) {
				tas_set_data_port_ctrlstd(port[i], BUT_A);
				tas_set_data_port_ctrlstd(port[i], BUT_B);
				tas_set_data_port_ctrlstd(port[i], SELECT);
				tas_set_data_port_ctrlstd(port[i], START);
				tas_set_data_port_ctrlstd(port[i], UP);
				tas_set_data_port_ctrlstd(port[i], DOWN);
				tas_set_data_port_ctrlstd(port[i], LEFT);
				tas_set_data_port_ctrlstd(port[i], RIGHT);
			}

		}
	}

	tas_increment_index()
}
void tas_header_FM2(char *file) {
	char line[256];
	unsigned int start;

	tas.emulator = FCEUX;

	while (fgets(line, sizeof(line), tas.fp)) {
		char *key = 0, *value = 0;

		for (start = 0; start < strlen(line); start++) {
			if ((line[start] == ' ') || (line[start] == '\t')) {
				continue;
			}
			break;
		}

		if ((line[start] == '#') || (line[start] == '\r') || (line[start] == '\n')) {
			continue;
		}

		if (!(key = strtok(line + start, " "))) {
			continue;
		}

		if ((value = strtok(NULL, "\n"))) {
			unsigned int i;

			for (i = 0; i < strlen(value); i++) {
				if (value[i] == '\r') {
					value[i] = 0;
				}
			}
		}

		if (key[0] == '|') {
			tas.total++;
		} else if (strcasecmp(key, "emulator") == 0) {
			if (strcasecmp(value, "punes") == 0) {
				tas.emulator = PUNES;
			}
		} else if (strcasecmp(key, "emuVersion") == 0) {
			tas.emu_version = atoi(value);
		} else if (strcasecmp(key, "punesStartFrame") == 0) {
			tas.start_frame = atoi(value) + 1;
		} else if (strcasecmp(key, "romFilename") == 0) {
			//sprintf(info.rom_file, "%s/%s", dirname(file), value);
			strcpy(info.rom_file, dirname(file));
			strcat(info.rom_file, "/");
			strcat(info.rom_file, value);
		} else if (strcasecmp(key, "port0") == 0) {
			port[PORT1].type = atoi(value);
		} else if (strcasecmp(key, "port1") == 0) {
			port[PORT2].type = atoi(value);
		}
	}

	if (tas.emulator == FCEUX) {
		info.r4014_precise_timing_disabled = TRUE;
		//if (tas.emu_version <= 9828) {
			info.r2002_race_condition_disabled = TRUE;
			info.r4016_dmc_double_read_disabled = TRUE;
		//}
	}

	fseek(tas.fp, 0, SEEK_SET);
}
void tas_read_FM2(void) {
	unsigned int start;
	char line[256], *sep;

	tas.count = tas.index = 0;

	while (fgets(line, sizeof(line), tas.fp)) {
		for (start = 0; start < strlen(line); start++) {
			if ((line[start] == ' ') || (line[start] == '\t')) {
				continue;
			}
			break;
		}
		if (line[start] != '|') {
			continue;
		}

		start++;

		sep = strtok(line + start, "|");

		tas.il[tas.count].state = atoi(sep);

		/* port1 */
		{
			BYTE a, b;

			for (a = PORT1; a <= PORT2; a++) {
				sep = strtok(NULL, "|");

				if (port[a].type == CTRL_STANDARD) {
					for (b = 0; b < 8; b++) {
						tas.il[tas.count].port[a][RIGHT - b] = PRESSED;

						if ((sep[b] == ' ') || (sep[b] == '.')) {
							tas.il[tas.count].port[a][RIGHT - b] = RELEASED;
						}
					}
				}
			}
		}

		if (++tas.count == LENGTH(tas.il)) {
			break;
		}
	}
}
