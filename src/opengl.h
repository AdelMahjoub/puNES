/*
 * opengl.h
 *
 *  Created on: 23/lug/2011
 *      Author: fhorse
 */

#ifndef OPENGL_H_
#define OPENGL_H_

#include "glew/glew.h"
#include <SDL.h>
#include "common.h"
#include "sdlgfx.h"
#include "openGL/shaders.h"

#define slowFactor	3

enum {
	NO_POWER_OF_TWO,
	POWER_OF_TWO,
	NO_GLEW
};

typedef struct {
	GLuint data;
	GLenum format;
	GLenum type;
	GLint format_internal;

	GLfloat x;
	GLfloat y;
} _texture;
struct _opengl {
	BYTE aspectRatio;
	BYTE rotation;

	BYTE glew;
	BYTE glsl;
	BYTE shader;

	GLfloat scale;
	GLfloat factor;
	GFX_EFFECT_ROUTINE

	SDL_Surface *surfaceGL;

	GLint wTexture;
	GLint hTexture;
	GLint xTexture1;
	GLint yTexture1;
	GLint xTexture2;
	GLint yTexture2;

	Uint32 flagsOpengl;
	BYTE factorDistance;

	float xRotate;
	float yRotate;
	float xDiff;
	float yDiff;

	_texture texture;
} opengl;

void sdlInitGL(void);
void sdlQuitGL(void);
void sdlCreateSurfaceGL(SDL_Surface *src, WORD width, WORD height, BYTE flags);
int opengl_flip(SDL_Surface *surface);

void glew_init(void);
void glsl_shaders_init(void);

void opengl_create_texture(GLuint *texture, uint32_t width, uint32_t height, uint8_t pow);
void opengl_update_texture(SDL_Surface *surface);
int opengl_power_of_two(int base);

void text_blit_opengl(_txt_element *ele, SDL_Rect *dst_rect);

/* funzioni virtuali */
void (*opengl_set)(SDL_Surface *src);
void (*opengl_draw_scene)(SDL_Surface *surface);

#endif /* OPENGL_H_ */
