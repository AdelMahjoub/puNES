/*
 * text.h
 *
 *  Created on: 04/feb/2012
 *      Author: fhorse
 */

#ifndef TEXT_H_
#define TEXT_H_

#if defined (SDL)
#include <SDL.h>
#elif defined (D3D9)
#include <time.h>
#endif
#include "common.h"

enum txt_type { TXT_INFO, TXT_SINGLE };
enum txt_fonts { FONT_8X10, FONT_12X10 };
enum txt_misc { TXT_MAX_MSG = 256, TXT_MAX_LINES = 10 };
enum txt_position {
	TXT_CENTER = 65000,
	TXT_LEFT,
	TXT_RIGHT,
	TXT_UP,
	TXT_DOWN
};
enum txt_tgs {
	TXT_NORMAL,
	TXT_RED,
	TXT_YELLOW,
	TXT_GREEN,
	TXT_CYAN,
	TXT_BROWN,
	TXT_BLUE,
	TXT_GRAY,
	TXT_BLACK,
	TXT_FONT_8,
	TXT_FONT_12,
	TXT_BUTTON_LEFT,
	TXT_BUTTON_RIGHT,
	TXT_BUTTON_UP,
	TXT_BUTTON_DOWN,
	TXT_BUTTON_SELECT,
	TXT_BUTTON_START,
	TXT_BUTTON_A,
	TXT_BUTTON_B,
	TXT_FLOPPY,
	TXT_TAGS
};

#define text_add_line_info(factor, ...)\
	text_add_line(TXT_INFO, factor, FONT_12X10, 255, 0, 0, 0, 0, __VA_ARGS__)
#define text_add_line_single(factor, font, alpha, start_x, start_y, x, y, ...)\
	text_add_line(TXT_SINGLE, factor, font, alpha, start_x, start_y, x, y, __VA_ARGS__)

#if defined (SDL)
#define _rect SDL_Rect
#else
typedef struct {
	int16_t x, y;
	uint16_t w, h;
} _rect;
#endif
typedef struct {
	uint8_t enabled;
	uint8_t bck;
	uint8_t bck_color;
	uint8_t font;
	uint8_t factor;
	int start_x;
	int start_y;
	int x;
	int y;
	int w;
	int h;
	int alpha_start_fade;
	int alpha[3];
	time_t start;
	char text[TXT_MAX_MSG];
	int length;

	int index;

#if defined (SDL)
	SDL_Surface *surface;
	SDL_Surface *blank;
#elif defined (D3D9)
	void *surface;
#endif
} _txt_element;

#if defined (__cplusplus)
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC struct _text {
#if defined (SDL)
	SDL_Surface *surface;
#endif
	BYTE on_screen;
	uint32_t w;
	uint32_t h;

	struct _txt_info {
		uint8_t index;
		uint8_t count;
		_txt_element buffers[TXT_MAX_LINES];
		_txt_element *lines[2][TXT_MAX_LINES];
	} info;
	struct _txt_single {
		uint8_t count;
		_txt_element *lines[TXT_MAX_LINES];
	} single;
	struct _txt_tas {
		_txt_element counter_frames;
		_txt_element controllers[4];
	} tas;
	struct _txt_fds {
		_txt_element floppy;
	} fds;
	struct _txt_save_slot {
		BYTE operation;
		_txt_element slot;
	} save_slot;
} text;

EXTERNC uint32_t txt_table[TXT_BLACK + 1];

EXTERNC void text_init(void);
EXTERNC void text_save_slot(BYTE operation);
EXTERNC void text_add_line(int type, int factor, int font, int alpha, int start_x, int start_y,
		int x, int y, const char *fmt, ...);
EXTERNC void text_rendering(BYTE render);
EXTERNC void text_quit(void);

EXTERNC void (*text_clear)(_txt_element *ele);
EXTERNC void (*text_blit)(_txt_element *ele, _rect *rect);

#undef EXTERNC

#endif /* TEXT_H_ */
