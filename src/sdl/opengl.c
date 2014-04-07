/*
 * opengl.c
 *
 *  Created on: 10/dic/2010
 *      Author: fhorse
 */

#include "opengl.h"
#include "gfx.h"
#include "ppu.h"
#include "cfg_file.h"
#include "openGL/no_effect.h"
#include "openGL/cube3d.h"
#define _SHADERS_CODE_
#include "shaders.h"
#undef  _SHADERS_CODE_

void glsl_print_log(GLuint obj, BYTE ret);
char *glsl_file2string(const char *path);

void sdl_init_gl(void) {
	opengl.rotation = 0;
	opengl.surface_gl = NULL;

	opengl.flags = SDL_HWSURFACE | SDL_OPENGL;
	opengl.factor_distance = 0;
	opengl.x_rotate = 0;
	opengl.y_rotate = 0;
	opengl.x_diff = 0;
	opengl.y_diff = 0;

	memset(&opengl.screen, 0, sizeof(_texture));
	memset(&shader, 0, sizeof(shader));

	opengl_init_effect = opengl_init_no_effect;
	opengl_set_effect = opengl_set_no_effect;
	opengl_unset_effect = opengl_unset_no_effect;
	opengl_draw_scene = opengl_draw_scene_no_effect;

	glew_init();
}
void sdl_quit_gl(void) {
	if (opengl.surface_gl) {
		SDL_FreeSurface(opengl.surface_gl);
	}

	if (opengl.screen.data) {
		glDeleteTextures(1, &opengl.screen.data);
	}

	if (opengl.text.data) {
		glDeleteTextures(1, &opengl.text.data);
	}

	if (opengl_unset_effect) {
		opengl_unset_effect();
	}

	glsl_delete_shaders(&shader);
}
void sdl_create_surface_gl(SDL_Surface *src, WORD width, WORD height, BYTE flags) {
	if (opengl.surface_gl) {
		SDL_FreeSurface(opengl.surface_gl);
		/*
		 * ripristino gli attributi opengl ai valori
		 * iniziali e li salvo nuovamente.
		 */
		glPopAttrib();
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (opengl.scale_force) {
		opengl.surface_gl = gfx_create_RGB_surface(src, SCR_ROWS * opengl.scale,
				SCR_LINES * opengl.scale);
	} else {
		opengl.surface_gl = gfx_create_RGB_surface(src, width, height);
	}

	opengl_create_texture(&opengl.screen, opengl.surface_gl->w, opengl.surface_gl->h, POWER_OF_TWO);

	opengl.texcoords.l = 0.0f;
	opengl.texcoords.r = (GLfloat) width / (opengl.screen.w * opengl.factor);
	opengl.texcoords.b = 0.0f;
	opengl.texcoords.t = (GLfloat) height / (opengl.screen.h * opengl.factor);

	{
		/* aspect ratio */
		GLfloat w_quad = (GLfloat) src->w;
		GLfloat h_quad = (GLfloat) src->h;

		opengl.quadcoords.l = 0.0f;
		opengl.quadcoords.r = w_quad;
		opengl.quadcoords.b = 0.0f;
		opengl.quadcoords.t = h_quad;

		/* con flags intendo sia il fullscreen che il futuro resize */
		if (flags && cfg->stretch) {
			GLfloat ratio_surface = w_quad / h_quad;
			GLfloat ratio_frame = (GLfloat) width / (GLfloat) height;

			/*
			 * se l'aspect ratio del frame e' maggiore di
			 * quello della superficie allora devo agire
			 * sull'altezza.
			 */
			if (ratio_frame > ratio_surface) {
				GLfloat centering_factor = 0.0f;

				h_quad = trunc((w_quad / ratio_frame) / gfx.pixel_aspect_ratio);
				centering_factor = trunc(((GLfloat) src->h - h_quad) / 2.0f);

				opengl.quadcoords.l = 0.0f;
				opengl.quadcoords.r = w_quad;
				opengl.quadcoords.b = centering_factor;
				opengl.quadcoords.t = h_quad + centering_factor;
			} else if (ratio_frame < ratio_surface) {
				/*
				 * se l'aspect ratio del frame e' minore di
				 * quello della superficie allora devo agire
				 * sulla larghezza.
				 */
				GLfloat centering_factor = 0.0f;

				w_quad = trunc((h_quad * ratio_frame) * gfx.pixel_aspect_ratio) - 1.0f;
				centering_factor = trunc(((GLfloat) src->w - w_quad) / 2.0f);

				opengl.quadcoords.l = centering_factor;
				opengl.quadcoords.r = w_quad + centering_factor;
				opengl.quadcoords.b = 0.0f;
				opengl.quadcoords.t = h_quad;
			}
		}
	}


	opengl_create_texture(&opengl.text, opengl.screen.w * opengl.factor,
	        opengl.screen.h * opengl.factor, NO_POWER_OF_TWO);

	if (opengl.glsl.enabled && opengl.glsl.shader_used) {
		glsl_shaders_init(&shader);
	}

	opengl_unset_effect();
	opengl_set_effect(src);

	glFinish();
}

void opengl_create_texture(_texture *texture, uint32_t width, uint32_t height, uint8_t pow) {
	switch (opengl.surface_gl->format->BitsPerPixel) {
		case 16:
			texture->format_internal = GL_RGB5;
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				texture->format = GL_BGR;
				texture->type = GL_UNSIGNED_SHORT_5_6_5_REV;
			} else {
				texture->format = GL_RGB;
				texture->type = GL_UNSIGNED_SHORT_5_6_5;
			}
			break;
		case 24:
			texture->format_internal = GL_RGB8;
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				texture->format = GL_BGR;
			} else {
				texture->format = GL_RGB;
			}
			texture->type = GL_UNSIGNED_BYTE;
			break;
		case 32:
		default:
			texture->format_internal = GL_RGBA8;
			texture->format = GL_BGRA;
			texture->type = GL_UNSIGNED_BYTE;
			break;
	}

	if (pow) {
		texture->w = opengl_power_of_two(width);
		texture->h = opengl_power_of_two(height);
	} else {
		texture->w = width;
		texture->h = height;
	}

	glEnable(GL_TEXTURE_2D);

	if (texture->data) {
		glDeleteTextures(1, &texture->data);
	}

	glGenTextures(1, &texture->data);
	glBindTexture(GL_TEXTURE_2D, texture->data);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	{
		GLfloat color[4] = { 0, 0, 0, 0 };

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	}

	if (opengl.glew && !GLEW_VERSION_3_0) {
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	}

	{
		/* per sicurezza creo una superficie piu' grande del necessario */
		SDL_Surface *blank = gfx_create_RGB_surface(opengl.surface_gl, texture->w * 2,
		        texture->h * 2);

		memset(blank->pixels, 0, blank->w * blank->h * blank->format->BytesPerPixel);

		glTexImage2D(GL_TEXTURE_2D, 0, texture->format_internal, texture->w, texture->h, 0,
				texture->format, texture->type, blank->pixels);

		SDL_FreeSurface(blank);
	}

	if (opengl.glew && GLEW_VERSION_3_0) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glDisable(GL_TEXTURE_2D);
}
void opengl_update_scr_texture(SDL_Surface *surface, uint8_t generate_mipmap) {
	glBindTexture(GL_TEXTURE_2D, opengl.screen.data);

	if (generate_mipmap && opengl.glew && !GLEW_VERSION_3_0) {
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	}

	glPixelStorei(GL_UNPACK_ROW_LENGTH, surface->w);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->w, surface->h, opengl.screen.format,
			opengl.screen.type, surface->pixels);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	if (generate_mipmap && opengl.glew && GLEW_VERSION_3_0) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	if (opengl.interpolation) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	if (opengl.glsl.shader_used) {
		glUseProgram(shader.prg);

		if (shader.loc.frame_counter != -1) {
			glUniform1f(shader.loc.frame_counter, (GLfloat) ppu.frames);
		}
	}
}

BYTE opengl_update_txt_texture(uint8_t generate_mipmap) {
	if (!cfg->txt_on_screen || !text.on_screen) {
		return (EXIT_ERROR);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, opengl.text.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (generate_mipmap && opengl.glew && !GLEW_VERSION_3_0) {
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	return (EXIT_OK);
}
void opengl_effect_change(BYTE mode) {
	if (input_zapper_is_connected((_port *) &port) == TRUE) {
		return;
	}

	if (opengl_unset_effect) {
		opengl_unset_effect();
	}

	opengl.rotation = mode;

	if (opengl.rotation) {
		opengl_init_effect = opengl_init_cube3d;
		opengl_set_effect = opengl_set_cube3d;
		opengl_unset_effect = opengl_unset_cube3d;
		opengl_draw_scene = opengl_draw_scene_cube3d;

		opengl.factor_distance = opengl.x_rotate = opengl.y_rotate = 0;

		if (cfg->fullscreen == FULLSCR) {
			SDL_ShowCursor(SDL_ENABLE);
		}
	} else {
		opengl_init_effect = opengl_init_no_effect;
		opengl_set_effect = opengl_set_no_effect;
		opengl_unset_effect = opengl_unset_no_effect;
		opengl_draw_scene = opengl_draw_scene_no_effect;

		if (cfg->fullscreen == FULLSCR) {
			SDL_ShowCursor(SDL_DISABLE);
		}
	}

	opengl_init_effect();

	gfx_set_screen(NO_CHANGE, NO_CHANGE, NO_CHANGE, NO_CHANGE, FALSE);
}

void opengl_text_clear(_txt_element *ele) {
	if (!ele->blank) {
		return;
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, opengl.text.data);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, ele->w);

	glTexSubImage2D(GL_TEXTURE_2D, 0, ele->x, ele->y, ele->w, ele->h,
			opengl.text.format, opengl.text.type, ele->blank->pixels);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	glDisable(GL_TEXTURE_2D);
}
void opengl_text_blit(_txt_element *ele, _rect *rect) {
	if (!cfg->txt_on_screen) {
		return;
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, opengl.text.data);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, rect->w);

	glTexSubImage2D(GL_TEXTURE_2D, 0, rect->x, rect->y, rect->w, rect->h,  opengl.text.format,
			opengl.text.type, ele->surface->pixels);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	glDisable(GL_TEXTURE_2D);
}

int opengl_flip(SDL_Surface *surface) {
	SDL_GL_SwapBuffers();

	return (0);
}
int opengl_power_of_two(int base) {
	int pot = 1;

	while (pot < base) {
		pot <<= 1;
	}
	return (pot);
}

void glew_init(void) {
	opengl.glsl.compliant = FALSE;
	/* opengl.glsl.enabled e' stato gia' settato dal file di configurazioen o
	 * dalla riga di comando.
	 * opengl.glsl.enabled = FALSE;
	 */
	opengl.glew = FALSE;

	if (opengl.supported){
		GLenum err;

		if ((err = glewInit()) != GLEW_OK) {
			fprintf(stderr, "INFO: %s\n", glewGetErrorString(err));
			opengl.glew = FALSE;
		} else {
			opengl.glew = TRUE;

			if (GLEW_VERSION_2_0) {
				fprintf(stderr, "INFO: OpenGL 2.0 supported. Glsl enabled.\n");
				opengl.glsl.compliant = TRUE;
			} else {
				fprintf(stderr, "INFO: OpenGL 2.0 not supported. Glsl disabled.\n");
				opengl.glsl.compliant = FALSE;
				opengl.glsl.enabled = FALSE;
			}

			if (GLEW_VERSION_3_0) {
				fprintf(stderr, "INFO: OpenGL 3.0 supported.\n");
			} else {
				fprintf(stderr, "INFO: OpenGL 3.0 not supported.\n");
			}
		}
	}
}

void glsl_shaders_init(_shader *shd) {
	shd->code = &shader_code[shd->id];

	/* program */
	shd->prg = glCreateProgram();

	/* vertex */
	if (shd->code->vertex != NULL) {
		shd->vrt = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shd->vrt, 1, &shd->code->vertex, NULL);
		glCompileShader(shd->vrt);
#if !defined (RELEASE)
		glsl_print_log(shd->vrt, FALSE);
#endif
		glAttachShader(shd->prg, shd->vrt);
	}

	/* fragment */
	if (shd->code->fragment != NULL) {
		shd->frg = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(shd->frg, 1, &shd->code->fragment, NULL);
		glCompileShader(shd->frg);
#if !defined (RELEASE)
		glsl_print_log(shd->frg, FALSE);
#endif
		glAttachShader(shd->prg, shd->frg);
	}

	glLinkProgram(shd->prg);
#if !defined (RELEASE)
	glsl_print_log(shd->prg, TRUE);
#endif

	glUseProgram(shd->prg);

	{
		GLfloat sse[2], svm[2], st[2], fc, param, par;

		sse[0] = (GLfloat) SCR_ROWS;
		sse[1] = (GLfloat) SCR_LINES;

		if (overscan.enabled) {
			sse[0] -= (GLfloat) (overscan.borders->left + overscan.borders->right);
			sse[1] -= (GLfloat) (overscan.borders->up + overscan.borders->down);
		}

		svm[0] = opengl.quadcoords.r - opengl.quadcoords.l;
		svm[1] = opengl.quadcoords.t - opengl.quadcoords.b;
		st[0] = opengl.screen.w;
		st[1] = opengl.screen.h;

		fc = (GLfloat) ppu.frames;
		param = (GLfloat) opengl.glsl.param;
		par = gfx.pixel_aspect_ratio;

		if ((shd->loc.size.screen_emu = glGetUniformLocation(shd->prg, "size_screen_emu")) >= 0) {
			glUniform2f(shd->loc.size.screen_emu, sse[0], sse[1]);
		}
		if ((shd->loc.size.video_mode = glGetUniformLocation(shd->prg, "size_video_mode")) >= 0) {
			glUniform2f(shd->loc.size.video_mode, svm[0], svm[1]);
		}
		if ((shd->loc.size.texture = glGetUniformLocation(shd->prg, "size_texture")) >= 0) {
			glUniform2f(shd->loc.size.texture,  st[0],  st[1]);
		}
		if ((shd->loc.frame_counter = glGetUniformLocation(shd->prg, "frame_counter")) >= 0) {
			glUniform1f(shd->loc.frame_counter, fc);
		}
		if ((shd->loc.param = glGetUniformLocation(shd->prg, "param")) >= 0) {
			glUniform1f(shd->loc.param, param);
		}
		if ((shd->loc.pixel_aspect_ratio = glGetUniformLocation(shd->prg, "pixel_aspect_ratio"))
		        >= 0) {
			glUniform1f(shd->loc.pixel_aspect_ratio, par);
		}
	}

	glEnable(GL_TEXTURE_2D);

	if ((shd->loc.texture.scr = glGetUniformLocation(shd->prg, "texture_scr")) >= 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, opengl.screen.data);
		glUniform1i(shd->loc.texture.scr, 0);
	}

	glDisable(GL_TEXTURE_2D);

	glUseProgram(0);
}
void glsl_delete_shaders(_shader *shd) {
	/* routine */
	shd->id = SHADER_NONE;
	shd->code = NULL;

	/* vertex */
	if (shd->vrt) {
		glDeleteShader(shd->vrt);
	}
	shd->vrt = 0;

	/* fragment */
	if (shd->frg) {
		glDeleteShader(shd->frg);
	}
	shd->frg = 0;

	/* program */
	if (shd->prg) {
		glDeleteProgram(shd->prg);
	}
	shd->prg = 0;
}
void glsl_print_log(GLuint obj, BYTE ret) {
	int info_log_length = 0, max_length = 0;

	if (glIsShader(obj)) {
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &max_length);
	} else {
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &max_length);
	}

	if (max_length == 0) {
		return;
	}

	{
		char info_log[max_length];

		if (glIsShader(obj)) {
			glGetShaderInfoLog(obj, max_length, &info_log_length, info_log);
		} else {
			glGetProgramInfoLog(obj, max_length, &info_log_length, info_log);
		}

		info_log[info_log_length] = 0;

		if (info_log_length > 0) {
			printf("INFO: %s", info_log);
			if (ret == TRUE) {
				printf("\n");
			}
		}
	}
}
char *glsl_file2string(const char *path) {
	FILE *fd;
	long len, r;
	char *str;

	if (!(fd = fopen(path, "r"))) {
		fprintf(stderr, "Can't open file '%s' for reading\n", path);
		return NULL;
	}

	fseek(fd, 0, SEEK_END);
	len = ftell(fd);

	printf("File '%s' is %ld long\n", path, len);

	fseek(fd, 0, SEEK_SET);

	if (!(str = (char *) malloc(len * sizeof(char)))) {
		fprintf(stderr, "Can't malloc space for '%s'\n", path);
		return NULL;
	}

	r = fread(str, sizeof(char), len, fd);

	str[r - 1] = '\0'; /* Shader sources have to term with null */

	fclose(fd);

	return str;
}




