/*
 * fps.h
 *
 *  Created on: 15/apr/2011
 *      Author: fhorse
 */

#ifndef FPS_H_
#define FPS_H_

#include "common.h"

enum fps_values {
	FPS_DEFAULT,
	FPS_60,
	FPS_59,
	FPS_58,
	FPS_57,
	FPS_56,
	FPS_55,
	FPS_54,
	FPS_53,
	FPS_52,
	FPS_51,
	FPS_50,
	FPS_49,
	FPS_48,
	FPS_47,
	FPS_46,
	FPS_45,
	FPS_44
};
enum ff_velocity_values { FF_2X = 2, FF_3X, FF_4X, FF_5X };

#define fps_machine_ms(factor)\
	machine.ms_frame = fps.ms = (1000.0f / (double) machine.fps) * factor;

#if defined (__cplusplus)
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC struct _fps {
	uint8_t fast_forward;
	int counter;
	int frames_before_skip;
	int max_frames_skipped;
	int frames_skipped;
	uint32_t total_frames_skipped;
	double ms;
	double next_frame;
	double second_start;
	double second_end;
	double avarage;
	double nominal;
} fps;
EXTERNC struct _framerate {
	uint32_t interval;
	double value;
	double last_time;
} framerate;

EXTERNC void fps_init(void);
EXTERNC void fps_fast_forward(void);
EXTERNC void fps_normalize(void);
EXTERNC void fps_frameskip(void);

#undef EXTERNC

#endif /* FPS_H_ */
