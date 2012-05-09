/*
 * opengl.c
 *
 *  Created on: 10/dic/2010
 *      Author: fhorse
 */

#include "opengl.h"
#include "sdlgfx.h"
#include "openGL/no_effect.h"
#include "openGL/cube3d.h"

char *file2string(const char *path);
void printLog(GLuint obj);
GLfloat OGL2Size[4];
GLfloat OGL2Param[4];
const GLchar *vsSource;
const GLchar *fsSource;
GLuint vs, /* Vertex Shader */
	   fs, /* Fragment Shader */
	   sp; /* Shader Program */
GLint baseImageLoc;
GLint OGL2SizeLoc;
GLint OGL2ParamLoc;
typedef struct {
	GLuint program;
	GLuint vert_shader;
	GLuint frag_shader;
	const GLchar *vert_source;
	const GLchar *frag_source;
} ShaderData;
static ShaderData shaders[2] = {
	{
		0, 0, 0,
		/* vertex shader */
		"varying vec4 vTexCoord[5];\n"
		"uniform vec4 OGL2Size;\n"
		"uniform vec4 OGL2Param;\n"
		"void main()\n"
		"{\n"
		"	vec4 offsetx;\n"
		"	vec4 offsety;\n"
		"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"	offsetx.x = OGL2Param.x;	// setup one x/y texel offset\n"
		"	offsetx.y = 0.0;			// we could also use \"1.0/OGL2Size.x (y)\"\n"
		"								// than it wouldn't\n"
		"	offsetx.w = 0.0;			// be dependand on the \"shader effect level\" setting...\n"
		"	offsetx.z = 0.0;			// but more choice is usual better, eh?\n"
		"	offsety.y = OGL2Param.y;\n"
		"	offsety.x = 0.0;\n"
		"	offsety.w = 0.0;\n"
		"	offsety.z = 0.0;\n"
		"	vTexCoord[0]    = gl_MultiTexCoord0;		// center\n"
		"	vTexCoord[1]    = vTexCoord[0] - offsetx;	// left\n"
		"	vTexCoord[2]    = vTexCoord[0] + offsetx;	// right\n"
		"	vTexCoord[3]    = vTexCoord[0] - offsety;	// top\n"
		"	vTexCoord[4]    = vTexCoord[0] + offsety;	// bottom\n"
		"}",
		/* fragment shader */
		"varying vec4 vTexCoord[5];\n"
		"uniform vec4 OGL2Size;\n"
		"uniform vec4 OGL2Param;\n"
		"uniform sampler2D OGL2Texture;\n"
		"void main()\n"
		"{\n"
		"	vec4 colD,colF,colB,colH,col,tmp;\n"
		"	vec2 sel;\n"
		"	col  = texture2DProj(OGL2Texture, vTexCoord[0]);	// central (can be E0-E3)\n"
		"	colD = texture2DProj(OGL2Texture, vTexCoord[1]);	// D (left)\n"
		"	colF = texture2DProj(OGL2Texture, vTexCoord[2]);	// F (right)\n"
		"	colB = texture2DProj(OGL2Texture, vTexCoord[3]);	// B (top)\n"
		"	colH = texture2DProj(OGL2Texture, vTexCoord[4]);	// H (bottom)\n"
		"	sel=fract(vTexCoord[0].xy * OGL2Size.xy);			// where are we (E0-E3)?\n"
		"														// E0 is default\n"
		"	if(sel.y >= 0.5)  {tmp=colB;colB=colH;colH=tmp;}	// E1 (or E3): swap B and H\n"
		"	if(sel.x >= 0.5)  {tmp=colF;colF=colD;colD=tmp;}	// E2 (or E3): swap D and F\n"
		"	if((colB == colD) && (colB != colF) && (colD != colH)) {	// do the Scale2x rule\n"
		"		col=colD;\n"
		"	}\n"
		"	gl_FragColor = col;\n"
		"}"
	},
	{
		0, 0, 0,
		/* vertex shader */
		"varying vec4 vTexCoord;\n"
		"void main(void)\n"
		"{\n"
		"	vTexCoord = gl_MultiTexCoord0;\n"
		"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"}",
		/* fragment shader */
		"uniform sampler2D OGL2Texture;\n"
		"varying vec4 vTexCoord;\n"
		"void main(void)\n"
		"{\n"
		"	gl_FragColor = texture2DProj(OGL2Texture, vTexCoord).bgra;\n"
		"}"
	}
};
#define shader shaders[0]





GLint xTexturePot, yTexturePot;


void sdlInitGL(void) {
	opengl.rotation = 0;
	opengl.surfaceGL = NULL;

	opengl.flagsOpengl = SDL_HWSURFACE | SDL_OPENGL;
	opengl.factorDistance = 0;
	opengl.xRotate = 0;
	opengl.yRotate = 0;
	opengl.xDiff = 0;
	opengl.yDiff = 0;

	opengl.glew = 0;

	memset(&opengl.texture, 0, sizeof(_texture));
}
void sdlCreateSurfaceGL(SDL_Surface *src, WORD width, WORD height, BYTE flags) {
	if (opengl.rotation) {
		opengl_set = opengl_set_cube3d;
		opengl_draw_scene = opengl_draw_scene_cube3d;
	} else {
		opengl_set = opengl_set_no_effect;
		opengl_draw_scene = opengl_draw_scene_no_effect;
	}

	if (!opengl.glew){
		GLenum err;

		if ((err = glewInit()) != GLEW_OK) {
			fprintf(stderr, "INFO: %s\n", glewGetErrorString(err));
		} else {
			opengl.glew = TRUE;
		}
	}

	if (opengl.surfaceGL) {
		SDL_FreeSurface(opengl.surfaceGL);
		/*
		 * ripristino gli attributi opengl ai valori
		 * iniziali e li salvo nuovamente.
		 */
		glPopAttrib();
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	opengl.surfaceGL = gfxCreateRGBSurface(src, opengl_power_of_two(SCRROWS),
			opengl_power_of_two(SCRLINES));

	opengl_create_texture(&opengl.texture.data);

	opengl_set(src);

	/* aspect ratio */
	opengl.wTexture = src->w;
	opengl.hTexture = src->h;
	opengl.xTexture1 = 0;
	opengl.yTexture1 = 0;
	opengl.xTexture2 = src->w;
	opengl.yTexture2 = src->h;

	/* con flags intendo sia il fullscreen che il futuro resize */
	if (flags && opengl.aspectRatio) {
		float ratioSurface = (float) opengl.wTexture / opengl.hTexture;
		float ratioFrame = (float) width / height;

		//ratioFrame = (float) 4 / 3;
		//ratioFrame = (float) 16 / 9;

		//fprintf(stderr, "opengl : %f %f\n", ratioSurface, ratioFrame);

		/*
		 * se l'aspect ratio del frame e' maggiore di
		 * quello della superficie allora devo agire
		 * sull'altezza.
		 */
		if (ratioFrame > ratioSurface) {
			int centeringFactor = 0;

			opengl.hTexture = opengl.wTexture / ratioFrame;
			centeringFactor = (src->h - opengl.hTexture) / 2;

			opengl.xTexture1 = 0;
			opengl.yTexture1 = centeringFactor;
			opengl.xTexture2 = opengl.wTexture;
			opengl.yTexture2 = opengl.hTexture + centeringFactor;
			/*
			 * se l'aspect ratio del frame e' minore di
			 * quello della superficie allora devo agire
			 * sulla larghezza.
			 */
		} else if (ratioFrame < ratioSurface) {
			int centeringFactor = 0;

			opengl.wTexture = ratioFrame * opengl.hTexture;
			centeringFactor = (src->w - opengl.wTexture) / 2;

			opengl.xTexture1 = centeringFactor;
			opengl.yTexture1 = 0;
			opengl.xTexture2 = opengl.wTexture + centeringFactor;
			opengl.yTexture2 = opengl.hTexture;
		}
	}

	xTexturePot = opengl_power_of_two(width);
	yTexturePot = opengl_power_of_two(height);
	xTsh = (GLfloat) width / xTexturePot;
	yTsh = (GLfloat) height / yTexturePot;

		/*if (GLEW_VERSION_2_0) {
#ifndef RELEASE
			fprintf(stderr, "INFO: OpenGL 2.0 supported. Glsl enabled.\n");
#endif
			opengl.glsl_enabled = TRUE;

			//vsSource = file2string("/home/fhorse/Dropbox/gpuPeteOGL2.slv");
			//fsSource = file2string("/home/fhorse/Dropbox/gpuPeteOGL2.slf");

			shader.vert_shader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(shader.vert_shader, 1, &shader.vert_source, NULL);
			glCompileShader(shader.vert_shader);
			printLog(shader.vert_shader);

			shader.frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(shader.frag_shader, 1, &shader.frag_source, NULL);
			glCompileShader(shader.frag_shader);
			printLog(shader.frag_shader);

			shader.program = glCreateProgram();
			printLog(shader.program);

			//free((char *) vsSource);
			//free((char *) fsSource);

			glAttachShader(shader.program, shader.vert_shader);
			glAttachShader(shader.program, shader.frag_shader);
			glLinkProgram(shader.program);
			printLog(shader.program);

			//glDeleteObject(vs);
			//glDeleteObject(fs);
			//glDeleteObject(sp);

			OGL2Size[0] = (GLfloat) xTexturePot; //width;
			OGL2Size[1] = (GLfloat) yTexturePot; //height;
			OGL2Size[2] = 256.0; //SCRLINES;
			OGL2Size[3] = 256.0;   //SCRROWS;

			OGL2Param[0] = 1.0 / (GLfloat) xTexturePot;
			OGL2Param[1] = 1.0 / (GLfloat) yTexturePot;
			OGL2Param[2] = 0.0;
			OGL2Param[3] = 0.0;

			glUseProgram(shader.program);

			OGL2ParamLoc = glGetUniformLocation(shader.program, "OGL2Param");
			OGL2SizeLoc = glGetUniformLocation(shader.program, "OGL2Size");
			baseImageLoc = glGetUniformLocation(shader.program, "OGL2Texture");

			printf("wave parameters location: %d %d %d\n", OGL2ParamLoc, OGL2SizeLoc, baseImageLoc);

			glUniform4fv(OGL2ParamLoc, 1, OGL2Param);
			glUniform4fv(OGL2SizeLoc, 1, OGL2Size);
			glUniform1i(baseImageLoc, 0);

			glUseProgram(0);
		}*/

	glFinish();
}

int opengl_flip(SDL_Surface *surface) {
	SDL_GL_SwapBuffers();

	return (0);
}




void opengl_create_texture(GLuint *texture) {
	switch (opengl.surfaceGL->format->BitsPerPixel) {
		case 16:
			opengl.texture.format_internal = GL_RGB5;
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				opengl.texture.format = GL_BGR;
				opengl.texture.type = GL_UNSIGNED_SHORT_5_6_5_REV;
			} else {
				opengl.texture.format = GL_RGB;
				opengl.texture.type = GL_UNSIGNED_SHORT_5_6_5;
			}
			break;
		case 24:
			opengl.texture.format_internal = GL_RGB8;
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				opengl.texture.format = GL_BGR;
			} else {
				opengl.texture.format = GL_RGB;
			}
			opengl.texture.type = GL_UNSIGNED_BYTE;
			break;
		case 32:
		default:
			opengl.texture.format_internal = GL_RGBA8;
			opengl.texture.format = GL_BGRA;
			opengl.texture.type = GL_UNSIGNED_BYTE;
			break;
	}

	if (texture) {
		glDeleteTextures(1, texture);
	}

	glGenTextures(1, texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, (*texture));

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (opengl.glew && !GLEW_VERSION_3_1) {
#ifndef RELEASE
		fprintf(stderr, "INFO: OpenGL 3.1 not supported.\n");
#endif
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, opengl.texture.format_internal, opengl.surfaceGL->w,
	        opengl.surfaceGL->h, 0, opengl.texture.format, opengl.texture.type, NULL);

	if (opengl.glew && GLEW_VERSION_3_1) {
#ifndef RELEASE
		fprintf(stderr, "INFO: OpenGL 3.1 supported.\n");
#endif
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}
int opengl_power_of_two(int base) {
	int pot = 1;

	while (pot < base) {
		pot <<= 1;
	}
	return (pot);
}



char *file2string(const char *path)
{
	FILE *fd;
	long len,
		 r;
	char *str;

	if (!(fd = fopen(path, "r")))
	{
		fprintf(stderr, "Can't open file '%s' for reading\n", path);
		return NULL;
	}

	fseek(fd, 0, SEEK_END);
	len = ftell(fd);

	printf("File '%s' is %ld long\n", path, len);

	fseek(fd, 0, SEEK_SET);

	if (!(str = malloc(len * sizeof(char))))
	{
		fprintf(stderr, "Can't malloc space for '%s'\n", path);
		return NULL;
	}

	r = fread(str, sizeof(char), len, fd);

	str[r - 1] = '\0'; /* Shader sources have to term with null */

	fclose(fd);

	return str;
}
void printLog(GLuint obj)
{
	int infologLength = 0;
	int maxLength;

	if(glIsShader(obj))
		glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
	else
		glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&maxLength);

	char infoLog[maxLength];

	if (glIsShader(obj))
		glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);

	if (infologLength > 0)
		printf("%s\n",infoLog);
}
