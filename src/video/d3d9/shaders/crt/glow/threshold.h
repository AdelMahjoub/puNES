{
"void main_vertex\n"
"(\n"
"	float4 position : POSITION,\n"
"	float4 color    : COLOR,\n"
"	float2 texCoord : TEXCOORD0,\n"
"\n"
"   uniform float4x4 modelViewProj,\n"
"\n"
"	out float4 oPosition : POSITION,\n"
"	out float2 otexCoord : TEXCOORD\n"
")\n"
"{\n"
"	oPosition = mul(modelViewProj, position);\n"
"	otexCoord = texCoord;\n"
"}\n"
"\n"
"struct input\n"
"{\n"
"   float2 video_size;\n"
"   float2 texture_size;\n"
"   float2 output_size;\n"
"   float  frame_count;\n"
"   float  frame_direction;\n"
"   float  frame_rotation;\n"
"};\n"
"\n"
"#pragma parameter GLOW_WHITEPOINT \"Glow Whitepoint\" 1.0 0.5 1.1 0.02\n"
"#pragma parameter GLOW_ROLLOFF \"Glow Rolloff\" 3.0 1.2 6.0 0.1\n"
"#ifdef PARAMETER_UNIFORM\n"
"uniform float GLOW_WHITEPOINT;\n"
"uniform float GLOW_ROLLOFF;\n"
"#else\n"
"#define GLOW_WHITEPOINT 1.0\n"
"#define GLOW_ROLLOFF 3.0\n"
"#endif\n"
"\n"
"float4 main_fragment(float2 tex : TEXCOORD0,\n"
"   uniform sampler2D s0 : TEXUNIT0, uniform input IN) : COLOR\n"
"{\n"
"   float3 color = 1.15 * tex2D(s0, tex).rgb;\n"
"   float3 factor = saturate(color / GLOW_WHITEPOINT);\n"
"   return float4(pow(factor, GLOW_ROLLOFF), 1.0);\n"
"}\n"
"\n"
},