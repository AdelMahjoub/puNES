/*
 * hqx.c
 *
 *  Created on: 04/gen/2012
 *      Author: fhorse
 */

#include "video/filters/hqx.h"
#include "overscan.h"

uint32_t RGBtoYUV[NUM_COLORS];
uint32_t YUV1, YUV2;

void hqx_init(void) {
	/* Initalize RGB to YUV lookup table */
	uint32_t i, r, g, b, y, u, v;

	for (i = 0; i < NUM_COLORS; i++) {
		r = palette_RGB[i].r;
		g = palette_RGB[i].g;
		b = palette_RGB[i].b;
		y = (uint32_t) (0.299 * r + 0.587 * g + 0.114 * b);
		u = (uint32_t) (-0.169 * r - 0.331 * g + 0.5 * b) + 128;
		v = (uint32_t) (0.5 * r - 0.419 * g - 0.081 * b) + 128;
		RGBtoYUV[i] = (y << 16) + (u << 8) + v;
	}
}
gfx_filter_function(hqNx) {
	hqnx.sx = 0;
	hqnx.sy = 0;
	hqnx.lines = lines;
	hqnx.rows = rows;
	hqnx.dst_rows = width / factor;
	hqnx.startx = 0;

	if (overscan.enabled) {
		hqnx.sy += overscan.borders->up;
		hqnx.lines += overscan.borders->up;
		hqnx.rows += overscan.borders->left;
		hqnx.startx = overscan.borders->left;
		screen += (SCR_ROWS * overscan.borders->up);
	}

	if (factor == 1) {
		return;
	} else if (factor == 2) {
		hq2x_32_rb(screen, pix, palette);
	} else if (factor == 3) {
		hq3x_32_rb(screen, pix, palette);
	} else if (factor == 4) {
		hq4x_32_rb(screen, pix, palette);
	}
}
