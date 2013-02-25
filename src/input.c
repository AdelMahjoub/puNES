/*
 * input.c
 *
 *  Created on: 01/nov/2011
 *      Author: fhorse
 */

#include <stdlib.h>
#include "input.h"
#include "clock.h"
#include "ppu.h"
#include "palette.h"
#include "opengl.h"
#include "gui.h"
#include "sdlgfx.h"
#include "apu.h"
#include "overscan.h"
#include "fps.h"
#include "tas.h"
#include "cfg_file.h"

void input_init(void) {
	switch (port1.type) {
		case CTRLDISABLED:
		default:
			input_port1 = NULL;
			SET_RD_REG1(input_rd_reg_disabled);
			break;
		case STDCTRL:
			SET_PORT1(input_port_standard);
			SET_RD_REG1(input_rd_reg_standard);
			break;
		case ZAPPER:
			input_port1 = NULL;
			SET_RD_REG1(input_rd_reg_zapper);
			break;
	}

	switch (port2.type) {
		case CTRLDISABLED:
		default:
			input_port2 = NULL;
			SET_RD_REG2(input_rd_reg_disabled);
			break;
		case STDCTRL:
			SET_PORT2(input_port_standard);
			SET_RD_REG2(input_rd_reg_standard);
			break;
		case ZAPPER:
			input_port2 = NULL;
			SET_RD_REG2(input_rd_reg_zapper);
			break;
	}

	{
		BYTE i;

		r4016.value = 0;
		port1.index = port2.index = 0;
		port1.zapper = port2.zapper = 0;

		if (port1.changed) {
			memset(&port1.turbo, 0, sizeof(_turbo_button) * LENGTH(port1.turbo));
			port1.changed = FALSE;
		}

		if (port2.changed) {
			memset(&port2.turbo, 0, sizeof(_turbo_button) * LENGTH(port2.turbo));
			port1.changed = FALSE;
		}

		input_turbo_buttons_frequency();

		for (i = 0; i < 24; i++) {
			if (i < 8) {
				port1.data[i] = port2.data[i] = RELEASED;
			} else {
				port1.data[i] = port2.data[i] = PRESSED;
			}
		}
	}
}
void input_turbo_buttons_frequency(void){
	port1.turbo[TURBOA].frequency = port1.turbo[TURBOB].frequency = fps.nominal / 15;
	port2.turbo[TURBOA].frequency = port2.turbo[TURBOB].frequency = fps.nominal / 15;
}

BYTE input_rd_reg_disabled(BYTE openbus, WORD **screen_index, _port *port) {
	return (openbus);
	/*return (0x80);*/
}

BYTE input_port_standard(BYTE mode, DBWORD event, BYTE type, _port *port) {
	if (tas.type) {
		return (EXIT_OK);
	} else if (event == port->input[type][BUT_A]) {
		if (!port->turbo[TURBOA].active) {
			port->data[BUT_A] = mode;
		}
		return (EXIT_OK);
	} else if (event == port->input[type][BUT_B]) {
		if (!port->turbo[TURBOB].active) {
			port->data[BUT_B] = mode;
		}
		return (EXIT_OK);
	} else if (event == port->input[type][SELECT]) {
		port->data[SELECT] = mode;
		return (EXIT_OK);
	} else if (event == port->input[type][START]) {
		port->data[START] = mode;
		return (EXIT_OK);
	} else if (event == port->input[type][UP]) {
		port->data[UP] = mode;
		/* non possono essere premuti contemporaneamente */
		if (mode == PRESSED) {
			port->data[DOWN] = RELEASED;
		}
		return (EXIT_OK);
	} else if (event == port->input[type][DOWN]) {
		port->data[DOWN] = mode;
		/* non possono essere premuti contemporaneamente */
		if (mode == PRESSED) {
			port->data[UP] = RELEASED;
		}
		return (EXIT_OK);
	} else if (event == port->input[type][LEFT]) {
		port->data[LEFT] = mode;
		/* non possono essere premuti contemporaneamente */
		if (mode == PRESSED) {
			port->data[RIGHT] = RELEASED;
		}
		return (EXIT_OK);
	} else if (event == port->input[type][RIGHT]) {
		port->data[RIGHT] = mode;
		/* non possono essere premuti contemporaneamente */
		if (mode == PRESSED) {
			port->data[LEFT] = RELEASED;
		}
		return (EXIT_OK);
	} else if (event == port->input[type][TRBA]) {
		if (mode == PRESSED) {
			/*if (!(port->turbo[TURBOA].active = !port->turbo[TURBOA].active)) {
				port->data[BUT_A] = RELEASED;
				port->turbo[TURBOA].counter = 0;
			}*/
			port->turbo[TURBOA].active = TRUE;
		} else {
			port->turbo[TURBOA].active = FALSE;
			port->data[BUT_A] = RELEASED;
			port->turbo[TURBOA].counter = 0;
		}
		return (EXIT_OK);
	} else if (event == port->input[type][TRBB]) {
		if (mode == PRESSED) {
			/*if (!(port->turbo[TURBOB].active = !port->turbo[TURBOB].active)) {
				port->data[BUT_B] = RELEASED;
				port->turbo[TURBOB].counter = 0;
			}*/
			port->turbo[TURBOB].active = TRUE;
		} else {
			port->turbo[TURBOB].active = FALSE;
			port->data[BUT_B] = RELEASED;
			port->turbo[TURBOB].counter = 0;
		}
		return (EXIT_OK);
	}
	return (EXIT_ERROR);
}
BYTE input_rd_reg_standard(BYTE openbus, WORD **screen_index, _port *port) {
	BYTE value;

	/*
	 * Se $4016 e' a 1 leggo solo lo stato del primo pulsante
	 * del controller. Quando verra' scritto 0 nel $4016
	 * riprendero' a leggere lo stato di tutti i pulsanti.
	 */
	value = port->data[port->index];
	if (!r4016.value) {
		port->index++;
	}
	/*
 	 * se avviene un DMA del DMC all'inizio
 	 * dell'istruzione di lettura del registro,
 	 * avverrano due letture.
 	 */
	if (!info.r4016_dmc_double_read_disabled && (DMC.dma_cycle == 2)) {
		value = port->data[port->index];
		if (!r4016.value) {
			port->index++;
		}
	}

	return(value);
}

BYTE input_rd_reg_zapper(BYTE openbus, WORD **screen_index, _port *port) {
	int x_zapper = -1, y_zapper = -1;
	int x_rect, y_rect;
	int gx = gui.x, gy = gui.y;
	int count = 0;

	port->zapper &= ~0x10;

	if (gui.left_button) {
		port->zapper |= 0x10;
	}

	if (!gui.right_button) {
		if (gfx.opengl) {
			gx -= opengl.xTexture1;
			gy -= opengl.yTexture1;
		}

		x_zapper = gx / gfx.wPr;
		y_zapper = gy / gfx.hPr;

		if (overscan.enabled) {
			x_zapper += overscan.left;
			y_zapper += overscan.up;
			/*
			 * il filtro NTSC necessita di un'aggiustatina sia con
			 * l'overscan abilitato che senza.
			 */
			if (cfg->filter == RGBNTSC) {
				x_zapper += 1;
			}
		} else {
			if (cfg->filter == RGBNTSC) {
				x_zapper -= 1;
			}
		}
	}

	//fprintf(stderr, "x : %d (%d)    %d (%d)   \r", x_zapper, gui.x, y_zapper, gui.y);

	if ((x_zapper <= 0) || (x_zapper >= SCRROWS) || (y_zapper <= 0) || (y_zapper >= SCRLINES)) {
		return (port->zapper |= 0x08);
	}

	if (!r2002.vblank && r2001.visible && (ppu.frameY > machine.vint_lines)
	        && (ppu.screenY < SCRLINES)) {
		for (y_rect = (y_zapper - 8); y_rect < (y_zapper + 8); y_rect++) {
			if (y_rect < 0) {
				continue;
			}
			if (y_rect <= (ppu.screenY - 18)) {
				continue;
			}
			if (y_rect >= ppu.screenY) {
				break;
			}
			for (x_rect = (x_zapper - 8); x_rect < (x_zapper + 8); x_rect++) {
				if (x_rect < 0) {
					continue;
				}
				if (x_rect > 255) {
					break;
				}
				{
					int brightness;
					_colorRGB color = paletteRGB[screen_index[y_rect][x_rect]];

					brightness = (color.r * 0.299) + (color.g * 0.587) + (color.b * 0.114);
					if (brightness > 0x80) {
						count++;
					}
				}
			}
		}
	}

	port->zapper &= ~0x08;

	if (count < 0x40) {
		port->zapper |= 0x08;
	}

	return (port->zapper);
}
