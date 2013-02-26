/*
 * cfg_file.c
 *
 *  Created on: 31/lug/2011
 *      Author: fhorse
 */

#include <libgen.h>
#include <stdlib.h>
#include "cfg_file.h"
#include "gui.h"
#include "version.h"
#include "clock.h"
#include "sdlgfx.h"
#include "sdlsnd.h"
#include "sdltext.h"
#include "fps.h"
#include "param.h"
#include "save_slot.h"
#include "input.h"
#include "gamegenie.h"
#include "audio_quality.h"
#include "opengl.h"

#define INIFILE NAME  ".cfg"
#define INPUTFILE     "input.cfg"
#define MAXLEN         512
#define cfg_evaluate(src, dst, chr)\
{\
	char *buf = 0;\
	buf = strtok(src, chr);\
	if (!buf) {\
		continue;\
	}\
	strcpy(dst, buf);\
}
#define cfg_search(structp, prm, start, desc, cmd)\
{\
	char buf[MAXLEN];\
	strcpy(buf,  structp[prm].lname);\
	trim_space(buf);\
	if (strcmp(key, buf) == 0) {\
		param_search(start, value, desc, cmd);\
		continue;\
	}\
}
#define cfg_input_search(param, port, type)\
{\
	BYTE index, found = FALSE;\
	for(index = 0; index < LENGTH(param); index++) {\
		char buf[512];\
		strcpy(buf, param[index].lname);\
		trim_space(buf);\
		if (strcmp(key, buf) == 0) {\
			if (type == JOYSTICK) {\
				if (index == (LENGTH(param) - 1)) {\
					port.joy_id = nameToJsn(value);\
				} else {\
					port.input[JOYSTICK][index] = nameToJsv(value);\
				}\
			} else {\
				port.input[KEYBOARD][index] = keyvalFromName(value);\
			}\
			found = TRUE;\
			break;\
		}\
	}\
	if (found) {\
		continue;\
	}\
}
void set_default(void);
void set_default_pgs(void);
void trim_space(char *src);
void write_param(_param *prmtr, FILE *fp, BYTE prm, char *value);
void write_input_param(_param *prmtr, FILE *fp, BYTE end, _port port, BYTE numport, BYTE type);
BYTE name_pgs_file(char *file);

void cfg_file_init(void) {
	cfg = &cfg_from_file;
}
void cfg_file_parse(void) {
	FILE *fp;
	char tmp[MAXLEN], line[MAXLEN];

	/* attivo la modalita' configurazione */
	info.on_cfg = TRUE;

	/* default */
	set_default();
	/* leggo la configurazione input */
	cfg_file_input_parse();
	/* apro il file di configurazione */
	sprintf(tmp, "%s/%s", info.base_folder, INIFILE);
	/* se non esiste allora lo creo */
	if ((fp = fopen(tmp, "r")) == NULL) {
		textAddLineInfo(1, "configuration [yellow]not found, [green]created");
		cfg_file_save();
		return;
	}
	/* leggo il file di configurazione */
	while (fgets(line, sizeof(line), fp)) {
		char key[MAXLEN], value[MAXLEN];
		/* elimino gli spazi */
		trim_space(line);
		/* se la linea non inizia con # (commento) o '\n' allora la esamino */
		if ((line[0] != '#') && (line[0] != '\n')) {
			/* separo quello che viene prima dell'uguale... */
			cfg_evaluate(line, key, "=");
			/* ...da quello che c'e' dopo */
			cfg_evaluate(NULL, value, "\n");
			/* mode */
			cfg_search(param, P_MODE, 0, param_mode, cfg_from_file.mode = index);
			/* fps */
			cfg_search(param, P_FPS, 0, param_fps, cfg_from_file.fps = index);
			/* frame skip */
			cfg_search(param, P_FSK, 0, param_fsk, cfg_from_file.frameskip = index);
			/* size */
			cfg_search(param, P_SIZE, 1, param_size, cfg_from_file.scale = index);
			/* overscan default */
			cfg_search(param, P_OVERSCAN, 0, param_oscan, cfg_from_file.oscan_default = index);
			/* filter */
			cfg_search(param, P_FILTER, 0, param_filter, cfg_from_file.filter = index);
			/* ntsc format */
			cfg_search(param, P_NTSCFORMAT, 0, param_ntsc, cfg_from_file.ntsc_format = index);
			/* palette */
			cfg_search(param, P_PALETTE, 0, param_palette, cfg_from_file.palette = index);
			/* rendering */
			cfg_search(param, P_RENDER, 0, param_render, cfg_from_file.render = index);
			/* vsync */
			cfg_search(param, P_VSYNC, 0, param_off_on, cfg_from_file.vsync = index);
			/* fullscreen */
			cfg_search(param, P_FSCREEN, 0, param_no_yes, cfg_from_file.fullscreen = index);
			/* stretch in fullscreen */
			cfg_search(param, P_STRETCH, 0, param_no_yes, cfg_from_file.aspect_ratio = !index);
			/* audio */
			cfg_search(param, P_AUDIO, 0, param_off_on, cfg_from_file.audio = index);
			/* sample rate */
			cfg_search(param, P_SAMPLERATE, 0, param_samplerate, cfg_from_file.samplerate = index);
			/* channels */
			cfg_search(param, P_CHANNELS, 0, param_channels, cfg_from_file.channels = index);
			/* audio quality */
			cfg_search(param, P_AUDIO_QUALITY, 0, param_audio_quality,
			        cfg_from_file.audio_quality = index);
			/* swap duty cycles */
			cfg_search(param, P_SWAP_DUTY, 0, param_no_yes, cfg_from_file.swap_duty = index);
			/* game genie */
			cfg_search(param, P_GAMEGENIE, 0, param_no_yes, cfg_from_file.gamegenie = index);
			/* save on exit */
			cfg_search(param, P_SAVEONEXIT, 0, param_no_yes, cfg_from_file.save_on_exit = index);
		}
	}

	textAddLineInfo(1, "configuration [green]loaded");

	gfx.scale_before_fscreen = cfg_from_file.scale;

	gfx_set_render(cfg_from_file.render);

	if (cfg_from_file.gamegenie) {
		gamegenie_check_rom_present(TRUE);
	}

	/* the end */
	fclose(fp);
}
void cfg_file_save(void) {
	FILE *fp;
	char tmp[MAXLEN];

	/* apro il file */
	sprintf(tmp, "%s/%s", info.base_folder, INIFILE);
	if ((fp = fopen(tmp, "w")) == NULL) {
		fprintf(stderr, "ERROR: File not found : %s", INIFILE);
		return;
	}
	/* mode */
	write_param((_param *) param, fp, P_MODE, param_mode[cfg_from_file.mode].sname);
	/* fps */
	write_param((_param *) param, fp, P_FPS, param_fps[cfg_from_file.fps].sname);
	/* fps */
	write_param((_param *) param, fp, P_FSK, param_fsk[cfg_from_file.frameskip].sname);
	/* size */
	{
		BYTE scale = (cfg_from_file.fullscreen ? gfx.scale_before_fscreen : cfg_from_file.scale);

		write_param((_param *) param, fp, P_SIZE, param_size[scale].sname);
	}
	/* overscan default */
	write_param((_param *) param, fp, P_OVERSCAN, param_oscan[cfg_from_file.oscan_default].sname);
	/* filter */
	write_param((_param *) param, fp, P_FILTER, param_filter[cfg_from_file.filter].sname);
	/* ntsc format */
	write_param((_param *) param, fp, P_NTSCFORMAT, param_ntsc[cfg_from_file.ntsc_format].sname);
	/* palette */
	write_param((_param *) param, fp, P_PALETTE, param_palette[cfg_from_file.palette].sname);
	/* rendering */
	write_param((_param *) param, fp, P_RENDER, param_render[cfg_from_file.render].sname);
	/* vsync */
	write_param((_param *) param, fp, P_VSYNC, param_off_on[cfg_from_file.vsync].sname);
	/* fullscreen */
	write_param((_param *) param, fp, P_FSCREEN, param_no_yes[cfg_from_file.fullscreen].sname);
	/* stretch in fullscreen */
	write_param((_param *) param, fp, P_STRETCH, param_no_yes[!cfg_from_file.aspect_ratio].sname);
	/* audio */
	write_param((_param *) param, fp, P_AUDIO, param_off_on[cfg_from_file.audio].sname);
	/* sample rate */
	write_param((_param *) param, fp, P_SAMPLERATE,
	        param_samplerate[cfg_from_file.samplerate].sname);
	/* channels */
	write_param((_param *) param, fp, P_CHANNELS, param_channels[cfg_from_file.channels].sname);
	/* audio quality */
	write_param((_param *) param, fp, P_AUDIO_QUALITY,
			param_audio_quality[cfg_from_file.audio_quality].sname);
	/* swap duty cycles */
	write_param((_param *) param, fp, P_SWAP_DUTY, param_no_yes[cfg_from_file.swap_duty].sname);
	/* game genie */
	write_param((_param *) param, fp, P_GAMEGENIE, param_no_yes[cfg_from_file.gamegenie].sname);
	/* save settings on exit */
	write_param((_param *) param, fp, P_SAVEONEXIT, param_no_yes[cfg_from_file.save_on_exit].sname);
	/* the end */
	fclose(fp);

	cfg_file_input_save();

}
void cfg_file_pgs_parse(void) {
	FILE *fp;
	char tmp[MAXLEN], line[MAXLEN];

	/* default */
	set_default_pgs();

	if (name_pgs_file(tmp)) {
		return;
	}

	if ((fp = fopen(tmp, "r")) == NULL) {
		if (!gamegenie.print) {
			textAddLineInfo(1, "rom configuration [yellow]not found");
		}
		return;
	}

	/* leggo il file di configurazione */
	while (fgets(line, sizeof(line), fp)) {
		char key[MAXLEN], value[MAXLEN];
		/* elimino gli spazi */
		trim_space(line);
		/* se la linea non inizia con # (commento) o '\n' allora la esamino */
		if ((line[0] != '#') && (line[0] != '\n')) {
			/* separo quello che viene prima dell'uguale... */
			cfg_evaluate(line, key, "=");
			/* ...da quello che c'e' dopo */
			cfg_evaluate(NULL, value, "\n");
			/* last save slot */
			cfg_search(param_pgs, PGS_SLOT, 0, param_slot, save_slot.slot = index);
			/* overscan */
			cfg_search(param_pgs, PGS_OVERSCAN, 0, param_oscan, cfg_from_file.oscan = index);
		}
	}

	if (!gamegenie.print) {
		textAddLineInfo(1, "rom configuration [green]loaded");
	}

	fclose(fp);
}
void cfg_file_pgs_save(void) {
	FILE *fp;
	char tmp[MAXLEN];

	if (name_pgs_file(tmp)) {
		return;
	}
	if ((fp = fopen(tmp, "w")) == NULL) {
		return;
	}
	/* last save slot */
	write_param((_param *) param_pgs, fp, PGS_SLOT, param_slot[save_slot.slot].sname);
	/* overscan */
	write_param((_param *) param_pgs, fp, PGS_OVERSCAN, param_oscan[cfg_from_file.oscan].sname);

	fclose(fp);
}
void cfg_file_input_parse(void) {
	FILE *fp;
	char tmp[MAXLEN], line[MAXLEN];

	/* apro il file di configurazione */
	sprintf(tmp, "%s/%s", info.base_folder, INPUTFILE);
	/* se non esiste lo creo */
	if ((fp = fopen(tmp, "r")) == NULL) {
		cfg_file_input_save();
		return;
	}
	/* leggo il file di configurazione */
	while (fgets(line, sizeof(line), fp)) {
		char key[MAXLEN], value[MAXLEN];
		/* elimino gli spazi */
		trim_space(line);
		/* se la linea non inizia con # (commento) o '\n' allora la esamino */
		if ((line[0] != '#') && (line[0] != '\n')) {
			/* separo quello che viene prima dell'uguale... */
			cfg_evaluate(line, key, "=");
			/* ...da quello che c'e' dopo */
			cfg_evaluate(NULL, value, "\n");

			cfg_search(param_input_ctrl, 0, 0, param_controller, port1.type = index);
			cfg_search(param_input_ctrl, 1, 0, param_controller, port2.type = index);

			cfg_input_search(param_input_p1k, port1, KEYBOARD);
			cfg_input_search(param_input_p1j, port1, JOYSTICK);

			cfg_input_search(param_input_p2k, port2, KEYBOARD);
			cfg_input_search(param_input_p2j, port2, JOYSTICK);
		}
	}

	textAddLineInfo(1, "input configuration [green]loaded");

	fclose(fp);
}
void cfg_file_input_save(void) {
	FILE *fp;
	char tmp[MAXLEN];

	/* apro il file */
	sprintf(tmp, "%s/%s", info.base_folder, INPUTFILE);
	if ((fp = fopen(tmp, "w")) == NULL) {
		return;
	}

	fprintf(fp, "# input configuration\n\n");

	write_param((_param *) param_input_ctrl, fp, 0, param_controller[port1.type].sname);
	write_param((_param *) param_input_ctrl, fp, 1, param_controller[port2.type].sname);

	write_input_param((_param *) param_input_p1k, fp, LENGTH(param_input_p1k), port1, 1, KEYBOARD);
	write_input_param((_param *) param_input_p1j, fp, LENGTH(param_input_p1j), port1, 1, JOYSTICK);

	write_input_param((_param *) param_input_p2k, fp, LENGTH(param_input_p2k), port2, 2, KEYBOARD);
	write_input_param((_param *) param_input_p2j, fp, LENGTH(param_input_p2j), port2, 2, JOYSTICK);

	fclose(fp);
}

void set_default(void) {

#define _port_kb_default(port, button, name)\
	port.input[KEYBOARD][button] = keyvalFromName(name);
#define _port_js_default(port, button, name)\
	port.input[JOYSTICK][button] = nameToJsv(name)
#define port_kb_default(port, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10)\
	_port_kb_default(port, BUT_A,  s1);\
	_port_kb_default(port, BUT_B,  s2);\
	_port_kb_default(port, SELECT, s3);\
	_port_kb_default(port, START,  s4);\
	_port_kb_default(port, UP,     s5);\
	_port_kb_default(port, DOWN,   s6);\
	_port_kb_default(port, LEFT,   s7);\
	_port_kb_default(port, RIGHT,  s8);\
	_port_kb_default(port, TRBA,   s9);\
	_port_kb_default(port, TRBB,   s10)
#define port_js_default(port, id, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10)\
	port.joy_id = nameToJsn(id);\
	_port_js_default(port, BUT_A,  s1);\
	_port_js_default(port, BUT_B,  s2);\
	_port_js_default(port, SELECT, s3);\
	_port_js_default(port, START,  s4);\
	_port_js_default(port, UP,     s5);\
	_port_js_default(port, DOWN,   s6);\
	_port_js_default(port, LEFT,   s7);\
	_port_js_default(port, RIGHT,  s8);\
	_port_js_default(port, TRBA,   s9);\
	_port_js_default(port, TRBB,   s10)

	/* default */
	cfg_from_file.mode = AUTO;
	machine = machinedb[NTSC - 1];

	cfg_from_file.fps = FPSDEFAULT;
	cfg_from_file.frameskip = 0;
	cfg_from_file.scale = gfx.scale_before_fscreen = X2;
	cfg_from_file.oscan_default = OSCAN_OFF;
	cfg_from_file.filter = NTSC_FILTER;
	cfg_from_file.ntsc_format = COMPOSITE;
	cfg_from_file.palette = PALETTE_NTSC;

	cfg_from_file.render = RENDER_OPENGL;
	gfx_set_render(cfg_from_file.render);

	cfg_from_file.vsync = TRUE;
	cfg_from_file.fullscreen = NO_FULLSCR;
	cfg_from_file.aspect_ratio = FALSE;
	cfg_from_file.save_on_exit = FALSE;

	cfg_from_file.audio = TRUE;
	cfg_from_file.samplerate = S44100;
	cfg_from_file.channels = STEREO;
	cfg_from_file.audio_quality = AQ_HIGH;
	cfg_from_file.swap_duty = 0;
	cfg_from_file.gamegenie = FALSE;

	port1.type = STDCTRL;
	port_kb_default(port1, "S", "A", "Z", "X", "Up", "Down", "Left", "Right", "W", "Q");
	port_js_default(port1, "JOYSTICKID1", "JB1", "JB0", "JB8", "JB9", "JA1MIN", "JA1PLS", "JA0MIN",
	        "JA0PLS", "JB2", "JB3");

	port2.type = FALSE;
	port2.joy_id = nameToJsn("JOYSTICKID2");
}
void set_default_pgs(void) {
	cfg_from_file.oscan = OSCAN_DEFAULT;
}
void trim_space(char *src) {
	const char *current = src;
	char out[MAXLEN];
	unsigned int i = 0, size = strlen(src);

	strcpy(out, src);
	while (current != '\0' && i < size) {
		if (*current != ' ' && *current != '\t') {
			out[i++] = *current;
		}
		++current;
	}
	out[i] = '\0';
	strcpy(src, out);
}
void write_param(_param *prmtr, FILE *fp, BYTE prm, char *value) {
	if (prmtr[prm].comment1 != NULL) {
		fprintf(fp, "%s\n", prmtr[prm].comment1);
	}
	if (prmtr[prm].comment2 != NULL) {
		fprintf(fp, "%s\n", prmtr[prm].comment2);
	}
	fprintf(fp, "%s = %s\n\n", prmtr[prm].lname, value);
}
void write_input_param(_param *prmtr, FILE *fp, BYTE end, _port port, BYTE numport, BYTE type) {
	BYTE index;

	for (index = 0; index < end; index++) {
		if (index == 0) {
			if (type == JOYSTICK) {
				fprintf(fp, "# player %d joystick\n", numport);
				fprintf(fp, "%s = %s\n", prmtr[end - 1].lname, jsnToName(port.joy_id));
				end--;
			} else {
				fprintf(fp, "# player %d keyboard\n", numport);
			}
		}
		if (type == JOYSTICK) {
			fprintf(fp, "%s = %s\n", prmtr[index].lname, jsvToName(port.input[JOYSTICK][index]));
		} else {
			fprintf(fp, "%s = %s\n", prmtr[index].lname, keyvalToName(port.input[KEYBOARD][index]));
		}
	}
}
BYTE name_pgs_file(char *file) {
	char ext[10], *last_dot;

	/* game genie */
	if (info.mapper == GAMEGENIE_MAPPER) {
		return (EXIT_ERROR);
	}

	if (!info.rom_file[0]) {
		return (EXIT_ERROR);
	}

	sprintf(file, "%s" PERGAME_FOLDER "/%s", info.base_folder, basename(info.rom_file));
	sprintf(ext, ".pgs");

	/* rintraccio l'ultimo '.' nel nome */
	last_dot = strrchr(file, '.');
	/* elimino l'estensione */
	*last_dot = 0x00;
	/* aggiungo l'estensione */
	strcat(file, ext);

	return (EXIT_OK);
}
