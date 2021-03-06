/*
 *  Copyright (C) 2010-2017 Fabio Cavallo (aka FHorse)
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

#include <string.h>
#include "input/nsf_mouse.h"
#include "input/mouse.h"
#include "conf.h"
#include "gui.h"
#include "nsf.h"

struct _nsf_mouse {
	BYTE pressed;
} nsf_mouse;

void input_init_nsf_mouse(void) {
	memset(&nsf_mouse, 0x00, sizeof(nsf_mouse));
}
void input_add_event_nsf_mouse(BYTE index) {
	int x, y;

	if (gmouse.left) {
		if (nsf_mouse.pressed) {
			return;
		} else {
			int button;

			input_read_mouse_coords(&x, &y);

			button = nsf_controls_mouse_in_buttons(x, y);

			switch (button) {
				case NSF_PREV:
					port->data[LEFT] = PRESSED;
					break;
				case NSF_PLAY:
					if (!(nsf.state & NSF_PLAY)) {
						port->data[START] = PRESSED;
					}
					break;
				case NSF_PAUSE:
					if (nsf.state & NSF_PLAY) {
						port->data[START] = PRESSED;
					}
					break;
				case NSF_STOP:
					port->data[BUT_A] = PRESSED;
					break;
				case NSF_NEXT:
					port->data[RIGHT] = PRESSED;
					break;
			}
			nsf_mouse.pressed = TRUE;
		}
		return;
	}

	if (nsf_mouse.pressed) {
		port->data[LEFT] = RELEASED;
		port->data[START] = RELEASED;
		port->data[START] = RELEASED;
		port->data[BUT_A] = RELEASED;
		port->data[RIGHT] = RELEASED;
		nsf_mouse.pressed = FALSE;
	}
}
