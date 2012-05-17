/*
 * shaders.h
 *
 *  Created on: 09/mag/2012
 *      Author: fhorse
 */

#ifndef SHADERS_H_
#define SHADERS_H_

#include "sdltext.h"

enum {
	SHADER_NOFILTER,
	SHADER_SCALE2X,
	SHADER_SCALE3X,
	SHADER_NONE = 255
};
#define SHADERNONE 255

#define delete_shader()\
	if (shader.vert) {\
		glDeleteShader(shader.vert);\
		glDeleteShader(shader.frag);\
		glDeleteProgram(shader.program);\
		shader.vert = 0;\
		shader.frag = 0;\
		shader.program = 0;\
	}

typedef struct {
	const GLchar *vert;
	const GLchar *frag;
} _shader_routine;
struct {
	GLuint program;
	GLuint vert;
	GLuint frag;

	_shader_routine *routine;

	GLuint texture_text;

	GLfloat size[4];
	GLfloat param[4];

	struct {
		GLint size;
		GLint param;
		GLint s_texture_scr;
		GLint s_texture_txt;
	} loc;
} shader;

#endif /* SHADERS_H_ */

#ifdef _SHADERS_CODE_
static _shader_routine shader_routine[3] = {
	/*****************************************************************************************/
	/* NOFILTER */
	/*****************************************************************************************/
	{
		// vertex shader
		"varying vec4 v_texCoord;\n"
		"void main(void)\n"
		"{\n"
		"	v_texCoord = gl_MultiTexCoord0;\n"
		"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"}",
		// fragment shader
		"uniform sampler2D s_texture_scr;\n"
		"uniform sampler2D s_texture_txt;\n"
		"varying vec4 v_texCoord;\n"
		"void main(void)\n"
		"{\n"
		"	vec4 scr, txt;\n"
		"	/*gl_FragColor = texture2DProj(s_texture_scr, v_texCoord).bgra;*/"
		"	scr = texture2DProj(s_texture_scr, v_texCoord).bgra;\n"
		"	txt = texture2DProj(s_texture_txt, v_texCoord);\n"
		"	gl_FragColor = mix(scr, txt, txt.a);\n"
		"}"
	},
	/*****************************************************************************************/
	/* SCALE2X */
	/*****************************************************************************************/
	{
		// vertex shader
		"uniform vec4 param;\n"
		"varying vec4 v_texCoord[5];\n"
		"void main()\n"
		"{\n"
		"	vec2 dx, dy;\n"
		"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"	dx = vec2(param.x, 0);\n"
		"	dy = vec2(0, param.y);\n"
		"	v_texCoord[0]    = gl_MultiTexCoord0;     // center\n"
		"	v_texCoord[1].xy = v_texCoord[0].xy - dx; // left\n"
		"	v_texCoord[2].xy = v_texCoord[0].xy + dx; // right\n"
		"	v_texCoord[3].xy = v_texCoord[0].xy - dy; // top\n"
		"	v_texCoord[4].xy = v_texCoord[0].xy + dy; // bottom\n"
		"}",

		"uniform vec4 size;\n"
		"uniform sampler2D s_texture_scr;\n"
		"uniform sampler2D s_texture_txt;\n"
		"varying vec4 v_texCoord[5];\n"
		"void main()\n"
		"{\n"
		"	vec4 scr, txt;\n"
		"	\n"
		"	vec3 E = texture2DProj(s_texture_scr, v_texCoord[0]).xyz;\n"
		"	vec3 D = texture2DProj(s_texture_scr, v_texCoord[1]).xyz;\n"
		"	vec3 F = texture2DProj(s_texture_scr, v_texCoord[2]).xyz;\n"
		"	vec3 H = texture2DProj(s_texture_scr, v_texCoord[3]).xyz;\n"
		"	vec3 B = texture2DProj(s_texture_scr, v_texCoord[4]).xyz;\n"
		"	\n"
		"	if ((D - F) * (H - B) == vec3(0.0)) {\n"
		"		scr.xyz = E;\n"
		"	} else {\n"
		"		vec2 p = fract(v_texCoord[0].xy * size.xy);\n"
		"		vec3 tmp1 = p.x < 0.5 ? D : F;\n"
		"		vec3 tmp2 = p.y < 0.5 ? H : B;\n"
		"		scr.xyz = ((tmp1 - tmp2) != vec3(0.0)) ? E : tmp1;\n"
		"	}\n"
		"	txt = texture2DProj(s_texture_txt, v_texCoord[0]);\n"
		"	gl_FragColor = mix(scr, txt, txt.a);\n"
		"}",

		/*// fragment shader
		"uniform vec4 size;\n"
		"uniform sampler2D s_texture_scr;\n"
		"uniform sampler2D s_texture_txt;\n"
		"varying vec4 v_texCoord[5];\n"
		"void main()\n"
		"{\n"
		"	vec4 colD,colF,colB,colH,col,tmp;\n"
		"	vec4 scr, txt;\n"
		"	vec2 sel;\n"
		"	col  = texture2DProj(s_texture_scr, v_texCoord[0]);	// central (can be E0-E3)\n"
		"	colD = texture2DProj(s_texture_scr, v_texCoord[1]);	// D (left)\n"
		"	colF = texture2DProj(s_texture_scr, v_texCoord[2]);	// F (right)\n"
		"	colB = texture2DProj(s_texture_scr, v_texCoord[3]);	// B (top)\n"
		"	colH = texture2DProj(s_texture_scr, v_texCoord[4]);	// H (bottom)\n"
		"	sel  = fract(v_texCoord[0].xy * size.xy);            // where are we (E0-E3)?\n"
		"                                                       // E0 is default\n"
		"	if(sel.y >= 0.5)  {tmp=colB;colB=colH;colH=tmp;}    // E1 (or E3): swap B and H\n"
		"	if(sel.x >= 0.5)  {tmp=colF;colF=colD;colD=tmp;}    // E2 (or E3): swap D and F\n"
		"	if((colB == colD) && (colB != colF) && (colD != colH)) { // do the Scale2x rule\n"
		"		col=colD;\n"
		"	}\n"
		"	scr = col;\n"
		"	txt = texture2DProj(s_texture_txt, v_texCoord[0]);\n"
		"	gl_FragColor = mix(scr, txt, txt.a);\n"
		"}"*/
	},
	/*****************************************************************************************/
	/* SCALE3X */
	/*****************************************************************************************/
	{
		"uniform vec4 param;\n"
		"varying vec4 v_texCoord[5];\n"
		"void main() {\n"
		"	vec2 dx, dy;\n"
		"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"	dx = vec2(param.x, 0); // setup one x/y texel d\n"
		"	// we could also use \"1.0/OGL2Size.x (y)\", than it wouldn't\n"
		"	dy = vec2(0, param.y); // be dependand on the \"shader effect level\" setting...\n"
		"	// but more choice is usual better, eh?\n"
		"	v_texCoord[0]    = gl_MultiTexCoord0;         // E\n"
		"	v_texCoord[0].zw = v_texCoord[0].xy - dx - dy; // A\n"
		"	v_texCoord[1].xy = v_texCoord[0].xy - dy;      // B\n"
		"	v_texCoord[1].zw = v_texCoord[0].xy + dx - dy; // C\n"
		"	v_texCoord[2].xy = v_texCoord[0].xy - dx;      // D\n"
		"	v_texCoord[2].zw = v_texCoord[0].xy + dx;      // F\n"
		"	v_texCoord[3].xy = v_texCoord[0].xy - dx + dy; // G\n"
		"	v_texCoord[3].zw = v_texCoord[0].xy + dy;      // H\n"
		"	v_texCoord[4].xy = v_texCoord[0].xy + dx + dy; // I\n"
		"}",
		"uniform vec4 size;\n"
		"uniform sampler2D s_texture_scr;\n"
		"uniform sampler2D s_texture_txt;\n"
		"varying vec4 v_texCoord[5];\n"
		"void main() {\n"
		"	const vec2 sep = vec2(0.33333, 0.66667); // sufficient precision for HDTV (1920x1080)\n"
		"	vec4 A, B, C, D, E, F, G, H, I, X, T;\n"
		"	vec4 scr, txt;\n"
		"	vec2 sel;\n"
		"	E = texture2D(s_texture_scr, v_texCoord[0].xy); // E\n"
		"	A = texture2D(s_texture_scr, v_texCoord[0].zw); // A\n"
		"	B = texture2D(s_texture_scr, v_texCoord[1].xy); // B\n"
		"	C = texture2D(s_texture_scr, v_texCoord[1].zw); // C\n"
		"	D = texture2D(s_texture_scr, v_texCoord[2].xy); // D\n"
		"	F = texture2D(s_texture_scr, v_texCoord[2].zw); // F\n"
		"	G = texture2D(s_texture_scr, v_texCoord[3].xy); // G\n"
		"	H = texture2D(s_texture_scr, v_texCoord[3].zw); // H\n"
		"	I = texture2D(s_texture_scr, v_texCoord[4].xy); // I\n"
		"	X = vec4(1) - E; // to be sure that ((E != A) == true) in function call\n"
		"	sel = fract(v_texCoord[0].xy * size.xy);// where are we (E0-E8)?\n"
		"	// branching is very undesirable, so we make a lot of reassignments\n"
		"	// of original pixels to make sure that rule for E1 pixel will work\n"
		"	// with any other (rotate second matrix and swap some Ex)\n"
		"	//\n"
		"	// native function call --> x y --> equivalent transpose (to minimize reassignments)\n"
		"	//(E, B, X, D, E, F, H) --> 0 0 --> (E, B, X, D, E, F, H) for E0\n"
		"	//(A, B, C, D, E, F, H) --> 1 0 --> (A, B, C, D, E, F, H) for E1\n"
		"	//(E, F, X, B, E, H, D) --> 2 0 --> (X, F, E, D, E, B, H) for E2\n"
		"	//(G, D, A, H, E, B, F) --> 0 1 --> (A, D, G, B, E, H, F) for E3\n"
		"	//( , E,  ,  , E,  ,  ) --> 1 1 --> ( , E,  ,  , E,  ,  ) for E4\n"
		"	//(C, F, I, B, E, H, D) --> 2 1 --> (I, F, C, H, E, B, D) for E5\n"
		"	//(E, D, X, H, E, B, F) --> 0 2 --> (E, D, X, H, E, F, B) for E6\n"
		"	//(I, H, G, F, E, D, B) --> 1 2 --> (G, H, I, D, E, F, B) for E7\n"
		"	//(E, H, X, F, E, D, B) --> 2 2 --> (X, H, E, D, E, F, B) for E8\n"
		"	if (sel.y < sep.x) {\n"
		"		if (sel.x < sep.x) {\n"
		"			A = E;\n"
		"			C = X;\n"
		"		} else if (sel.x >= sep.y) {\n"
		"			A = X;\n"
		"			C = E;\n"
		"			T = B;\n"
		"			B = F;\n"
		"			F = T;\n"
		"		}\n"
		"	} else if (sel.y < sep.y) {\n"
		"		T = B;\n"
		"		if (sel.x < sep.x) {\n"
		"			B = D;\n"
		"			D = T;\n"
		"			C = G;\n"
		"			T = F;\n"
		"			F = H;\n"
		"		} else if (sel.x < sep.y) {\n"
		"			B = E;\n"
		"		} else {\n"
		"			A = I;\n"
		"			B = F;\n"
		"			F = T;\n"
		"			T = D;\n"
		"			D = H;\n"
		"		}\n"
		"		H = T;\n"
		"	} else {\n"
		"		T = B;\n"
		"		if (sel.x < sep.x) {\n"
		"			A = E;\n"
		"			C = X;\n"
		"			B = D;\n"
		"			D = H;\n"
		"		} else {\n"
		"			if (sel.x < sep.y) {\n"
		"				A = G;\n"
		"				C = I;\n"
		"			} else {\n"
		"				A = X;\n"
		"				C = E;\n"
		"			}\n"
		"			B = H;\n"
		"		}\n"
		"		H = T;\n"
		"	}\n"
		"	scr = ((D == B && B != F && D != H && E != C) ||"
		"		   (B == F && B != D && F != H && E != A)) ?"
		"			B : E; // Scale3x rule\n"
		"	txt = texture2D(s_texture_txt, v_texCoord[0].xy);\n"
		"	gl_FragColor = mix(scr, txt, txt.a);\n"
		"}"
	}
};
#undef _SHADERS_CODE_
#endif
