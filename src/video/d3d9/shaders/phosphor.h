/*
 *  Copyright (C) 2010-2016 Fabio Cavallo (aka FHorse)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*****************************************************************************************/
/* Phosphor                                                                              */
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
	"float2 size_texture;\n"
	"float2 size_screen_emu;\n"
	"float2 size_video_mode;\n"
	"float param;\n"
	"float pixel_aspect_ratio;\n"
	"float full_interpolation;\n"

	"texture texture_scr;\n"
	"sampler2D s0 = sampler_state { Texture = <texture_scr>; };\n"

	"float mod(float x, float y) {\n"
	"	return x - y * floor(x / y);\n"
	"}\n"

	"float3 to_focus(float pixel, float green) {\n"
	"	pixel = mod(pixel + 3.0, 3.0);\n"
	"	if (pixel >= 2.0) {        //  Blue\n"
	"		return float3(pixel - 2.0, 0.0, 3.0 - pixel);\n"
	"	} else if (pixel >= 1.0) { // Green\n"
	"		return float3(0.0, green - pixel, pixel - 1.0);\n"
	"	} else {                   //  Red\n"
	"		return float3(1.0 - pixel, pixel, 0.0);\n"
	"	}\n"
	"}\n"

	"float4 Ps(float2 texCoord : TEXCOORD0) : COLOR {\n"
	"	float green;"
	"	float2 aspect1;\n"
	"	float2 aspect2;\n"

	"	if (param == 0.0) {\n"
	"		aspect1 = float2(size_video_mode.x / pixel_aspect_ratio, size_video_mode.y);\n"
	"		aspect2 = size_video_mode * (256.0 / size_screen_emu) * 0.5;\n"
	"		green = 2.50;\n"
	"	} else {\n"
	"		aspect1 = size_screen_emu * pixel_aspect_ratio;\n"
	"		aspect2 = 256.0 * (size_video_mode / size_screen_emu);\n"
	"		green = 2.10;"
	"	}\n"

#include "interpolation.h"

	"	float y = mod(texCoord.y, 1.0);\n"
	"	float intensity = exp(-0.2 * y);\n"

	"	float2 one_x = float2((1.0 / (aspect1.x * 3.0)), 0.0);\n"

	"	float3 color = tex2D(s0, pnt).rgb;\n"
	"	float3 color_prev = tex2D(s0, pnt - one_x).rgb;\n"
	"	float3 color_prev_prev = tex2D(s0, pnt - (2.0 * one_x)).rgb;\n"

	"	float pixel_x = 3.0 * (texCoord.x * aspect2.x);\n"

	"	float3 focus = to_focus(pixel_x - 0.0, green);\n"
	"	float3 focus_prev = to_focus(pixel_x - 1.0, green);\n"
	"	float3 focus_prev_prev = to_focus(pixel_x - 2.0, green);\n"

	"	float3 result = 0.8 * color * focus +"
	"					0.6 * color_prev * focus_prev +"
	"					0.3 * color_prev_prev * focus_prev_prev;\n"

	"	result = 2.3 * pow(result, float3(1.4, 1.4, 1.4));\n"

	"	float4 scr = float4(intensity * result, 1.0);\n"

	"	return scr;\n"
	"}"
},
