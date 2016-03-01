{
"// GLSL shader autogenerated by cg2glsl.py.\n"
"#if defined(VERTEX)\n"
"\n"
"#if __VERSION__ >= 130\n"
"#define COMPAT_VARYING out\n"
"#define COMPAT_ATTRIBUTE in\n"
"#define COMPAT_TEXTURE texture\n"
"#else\n"
"#define COMPAT_VARYING varying \n"
"#define COMPAT_ATTRIBUTE attribute \n"
"#define COMPAT_TEXTURE texture2D\n"
"#endif\n"
"\n"
"#ifdef GL_ES\n"
"#define COMPAT_PRECISION mediump\n"
"#else\n"
"#define COMPAT_PRECISION\n"
"#endif\n"
"COMPAT_VARYING     float _frame_rotation;\n"
"struct input_dummy {\n"
"    vec2 _video_size;\n"
"    vec2 _texture_size;\n"
"    vec2 _output_dummy_size;\n"
"    float _frame_count;\n"
"    float _frame_direction;\n"
"    float _frame_rotation;\n"
"};\n"
"vec4 _oPosition1;\n"
"vec4 _r0006;\n"
"COMPAT_ATTRIBUTE vec4 VertexCoord;\n"
"COMPAT_ATTRIBUTE vec4 COLOR;\n"
"COMPAT_VARYING vec4 COL0;\n"
"COMPAT_ATTRIBUTE vec4 TexCoord;\n"
"COMPAT_VARYING vec4 TEX0;\n"
" \n"
"uniform mat4 MVPMatrix;\n"
"uniform int FrameDirection;\n"
"uniform int FrameCount;\n"
"uniform COMPAT_PRECISION vec2 OutputSize;\n"
"uniform COMPAT_PRECISION vec2 TextureSize;\n"
"uniform COMPAT_PRECISION vec2 InputSize;\n"
"void main()\n"
"{\n"
"    vec4 _oColor;\n"
"    vec2 _oTex;\n"
"    _r0006 = VertexCoord.x*MVPMatrix[0];\n"
"    _r0006 = _r0006 + VertexCoord.y*MVPMatrix[1];\n"
"    _r0006 = _r0006 + VertexCoord.z*MVPMatrix[2];\n"
"    _r0006 = _r0006 + VertexCoord.w*MVPMatrix[3];\n"
"    _oPosition1 = _r0006;\n"
"    _oColor = COLOR;\n"
"    _oTex = TexCoord.xy;\n"
"    gl_Position = _r0006;\n"
"    COL0 = COLOR;\n"
"    TEX0.xy = TexCoord.xy;\n"
"} \n"
"#elif defined(FRAGMENT)\n"
"\n"
"#if __VERSION__ >= 130\n"
"#define COMPAT_VARYING in\n"
"#define COMPAT_TEXTURE texture\n"
"out vec4 FragColor;\n"
"#else\n"
"#define COMPAT_VARYING varying\n"
"#define FragColor gl_FragColor\n"
"#define COMPAT_TEXTURE texture2D\n"
"#endif\n"
"\n"
"#ifdef GL_ES\n"
"#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
"precision highp float;\n"
"#else\n"
"precision mediump float;\n"
"#endif\n"
"#define COMPAT_PRECISION mediump\n"
"#else\n"
"#define COMPAT_PRECISION\n"
"#endif\n"
"COMPAT_VARYING     float _frame_rotation;\n"
"struct input_dummy {\n"
"    vec2 _video_size;\n"
"    vec2 _texture_size;\n"
"    vec2 _output_dummy_size;\n"
"    float _frame_count;\n"
"    float _frame_direction;\n"
"    float _frame_rotation;\n"
"};\n"
"float _TMP19;\n"
"float _TMP18;\n"
"float _TMP17;\n"
"float _TMP16;\n"
"vec4 _TMP15;\n"
"vec4 _TMP14;\n"
"vec4 _TMP13;\n"
"vec4 _TMP12;\n"
"vec4 _TMP11;\n"
"vec4 _TMP10;\n"
"vec4 _TMP9;\n"
"vec4 _TMP8;\n"
"vec4 _TMP7;\n"
"vec4 _TMP6;\n"
"vec4 _TMP5;\n"
"vec4 _TMP4;\n"
"vec4 _TMP3;\n"
"vec4 _TMP2;\n"
"vec4 _TMP1;\n"
"vec4 _TMP0;\n"
"uniform sampler2D Texture;\n"
"vec2 _c0025;\n"
"vec2 _c0037;\n"
"vec2 _c0049;\n"
"vec2 _c0061;\n"
"vec2 _c0073;\n"
"vec2 _c0085;\n"
"vec2 _c0097;\n"
"vec2 _c0109;\n"
"COMPAT_VARYING vec4 TEX0;\n"
" \n"
"uniform int FrameDirection;\n"
"uniform int FrameCount;\n"
"uniform COMPAT_PRECISION vec2 OutputSize;\n"
"uniform COMPAT_PRECISION vec2 TextureSize;\n"
"uniform COMPAT_PRECISION vec2 InputSize;\n"
"void main()\n"
"{\n"
"    vec4 _big;\n"
"    _big = COMPAT_TEXTURE(Texture, TEX0.xy);\n"
"    _c0025 = vec2(TEX0.x, TEX0.y + 9.99999931E-04);\n"
"    _TMP0 = COMPAT_TEXTURE(Texture, _c0025);\n"
"    _TMP16 = pow(_TMP0.x, 2.20000005E+00);\n"
"    _TMP17 = pow(_TMP0.y, 2.20000005E+00);\n"
"    _TMP18 = pow(_TMP0.z, 2.20000005E+00);\n"
"    _TMP19 = pow(_TMP0.w, 2.20000005E+00);\n"
"    _TMP1 = vec4(_TMP16, _TMP17, _TMP18, _TMP19);\n"
"    _big = _big + _TMP1;\n"
"    _c0037 = vec2(TEX0.x, TEX0.y - 9.99999931E-04);\n"
"    _TMP2 = COMPAT_TEXTURE(Texture, _c0037);\n"
"    _TMP16 = pow(_TMP2.x, 2.20000005E+00);\n"
"    _TMP17 = pow(_TMP2.y, 2.20000005E+00);\n"
"    _TMP18 = pow(_TMP2.z, 2.20000005E+00);\n"
"    _TMP19 = pow(_TMP2.w, 2.20000005E+00);\n"
"    _TMP3 = vec4(_TMP16, _TMP17, _TMP18, _TMP19);\n"
"    _big = _big + _TMP3;\n"
"    _c0049 = vec2(TEX0.x, TEX0.y + 1.99999986E-03);\n"
"    _TMP4 = COMPAT_TEXTURE(Texture, _c0049);\n"
"    _TMP16 = pow(_TMP4.x, 2.20000005E+00);\n"
"    _TMP17 = pow(_TMP4.y, 2.20000005E+00);\n"
"    _TMP18 = pow(_TMP4.z, 2.20000005E+00);\n"
"    _TMP19 = pow(_TMP4.w, 2.20000005E+00);\n"
"    _TMP5 = vec4(_TMP16, _TMP17, _TMP18, _TMP19);\n"
"    _big = _big + _TMP5;\n"
"    _c0061 = vec2(TEX0.x, TEX0.y - 1.99999986E-03);\n"
"    _TMP6 = COMPAT_TEXTURE(Texture, _c0061);\n"
"    _TMP16 = pow(_TMP6.x, 2.20000005E+00);\n"
"    _TMP17 = pow(_TMP6.y, 2.20000005E+00);\n"
"    _TMP18 = pow(_TMP6.z, 2.20000005E+00);\n"
"    _TMP19 = pow(_TMP6.w, 2.20000005E+00);\n"
"    _TMP7 = vec4(_TMP16, _TMP17, _TMP18, _TMP19);\n"
"    _big = _big + _TMP7;\n"
"    _c0073 = vec2(TEX0.x, TEX0.y + 3.99999972E-03);\n"
"    _TMP8 = COMPAT_TEXTURE(Texture, _c0073);\n"
"    _TMP16 = pow(_TMP8.x, 2.20000005E+00);\n"
"    _TMP17 = pow(_TMP8.y, 2.20000005E+00);\n"
"    _TMP18 = pow(_TMP8.z, 2.20000005E+00);\n"
"    _TMP19 = pow(_TMP8.w, 2.20000005E+00);\n"
"    _TMP9 = vec4(_TMP16, _TMP17, _TMP18, _TMP19);\n"
"    _big = _big + _TMP9;\n"
"    _c0085 = vec2(TEX0.x, TEX0.y - 3.99999972E-03);\n"
"    _TMP10 = COMPAT_TEXTURE(Texture, _c0085);\n"
"    _TMP16 = pow(_TMP10.x, 2.20000005E+00);\n"
"    _TMP17 = pow(_TMP10.y, 2.20000005E+00);\n"
"    _TMP18 = pow(_TMP10.z, 2.20000005E+00);\n"
"    _TMP19 = pow(_TMP10.w, 2.20000005E+00);\n"
"    _TMP11 = vec4(_TMP16, _TMP17, _TMP18, _TMP19);\n"
"    _big = _big + _TMP11;\n"
"    _c0097 = vec2(TEX0.x, TEX0.y + 7.99999945E-03);\n"
"    _TMP12 = COMPAT_TEXTURE(Texture, _c0097);\n"
"    _TMP16 = pow(_TMP12.x, 2.20000005E+00);\n"
"    _TMP17 = pow(_TMP12.y, 2.20000005E+00);\n"
"    _TMP18 = pow(_TMP12.z, 2.20000005E+00);\n"
"    _TMP19 = pow(_TMP12.w, 2.20000005E+00);\n"
"    _TMP13 = vec4(_TMP16, _TMP17, _TMP18, _TMP19);\n"
"    _big = _big + _TMP13;\n"
"    _c0109 = vec2(TEX0.x, TEX0.y - 7.99999945E-03);\n"
"    _TMP14 = COMPAT_TEXTURE(Texture, _c0109);\n"
"    _TMP16 = pow(_TMP14.x, 2.20000005E+00);\n"
"    _TMP17 = pow(_TMP14.y, 2.20000005E+00);\n"
"    _TMP18 = pow(_TMP14.z, 2.20000005E+00);\n"
"    _TMP19 = pow(_TMP14.w, 2.20000005E+00);\n"
"    _TMP15 = vec4(_TMP16, _TMP17, _TMP18, _TMP19);\n"
"    _big = _big + _TMP15;\n"
"    _big = _big/vec4( 9.00000000E+00, 9.00000000E+00, 9.00000000E+00, 9.00000000E+00);\n"
"    _TMP16 = pow(_big.x, 4.54545438E-01);\n"
"    _TMP17 = pow(_big.y, 4.54545438E-01);\n"
"    _TMP18 = pow(_big.z, 4.54545438E-01);\n"
"    _TMP19 = pow(_big.w, 4.54545438E-01);\n"
"    _big = vec4(_TMP16, _TMP17, _TMP18, _TMP19);\n"
"    FragColor = _big;\n"
"    return;\n"
"} \n"
"#endif\n"
},