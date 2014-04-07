/*****************************************************************************************/
/* Scanline                                                                              */
/*****************************************************************************************/
{
	// vertex shader
	"struct VsOutput {\n"
	"	float4 Position : POSITION;\n"
	"	float2 TexCoord : TEXCOORD0;\n"
	"};\n"

	"float4x4 m_world_view_projection : WORLDVIEWPROJECTION;\n"

	"VsOutput Vs(float4 position : POSITION, float2 texCoord : TEXCOORD0) {\n"
	"	VsOutput output;\n"
	"	output.Position = mul(position, m_world_view_projection);\n"
	"	output.TexCoord = texCoord;\n"
	"	return output;\n"
	"}",
	// pixel shader
	"float2 size_screen_emu;\n"
	"float2 size_video_mode;\n"
	"float2 size_texture;\n"

	"texture texture_scr;\n"
	"sampler2D s0 = sampler_state { Texture = <texture_scr>; };\n"

	"static const float base_brightness = 0.95;\n"
	"static const float2 sine_comp = float2(0.05, 0.15);\n"

	"float4 Ps(float2 texCoord : TEXCOORD0) : COLOR {\n"
	"	float2 omega = float2(3.1415 * size_video_mode.x * size_texture.x / size_screen_emu.x,"
	"							2.0 * 3.1415 * size_texture.y);\n"

	"	float4 c11 = tex2D(s0, texCoord);\n"

	"	float4 scanline = c11 * (base_brightness + dot(sine_comp * sin(texCoord * omega),"
	"			float2(1.0, 1.0)));\n"

	"	float4 scr = saturate(scanline);\n"

	"	return scr;\n"
	"}"
},
