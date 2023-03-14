#ifndef _MERGE
#define _MERGE

#include "register.fx"

// ===================
// BLOOM
// MRT  : MRT_TYPE::SWAP CHAIN
// mesh : Rect Mesh

// Parameter
// g_float_0 : 1600 or 900
//
// g_tex_0 : blur tex
// g_tex_1 : hdr tex
// Target MRT
// BlurBloom Target
// Bloom Target

static const float Weight[13] =
{
	0.4561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231,
	1, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.4561
};

static const float Total = 6.2108;

struct PS_OUT
{
	float4 vBlurBloom : SV_Target0;
};

struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
};

VS_OUT VS_BloomBlur(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;
	output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
	return output;
}

PS_OUT PS_BloomBlur(VS_OUT _in) : SV_Target
{
	PS_OUT vOutColor = (PS_OUT)0.f;

	float BrightColor = 0.f;

	float2 vUV = _in.vPosition.xy / g_vRenderResolution;

	float4 blurbloom = g_tex_0.Sample(g_sam_0, vUV);

	float brightness = dot(blurbloom.rgb, float3(g_vec4_0.x, g_vec4_0.y, g_vec4_0.z));

	if (brightness <= g_float_0)
		discard;

	if (brightness > g_float_0)
		vOutColor.vBlurBloom = float4(blurbloom.rgb, 1);

	float2 t = _in.vPosition.xy / g_vRenderResolution;

	float2 uv = 0;

	float tu = 1.f / g_float_0;

	for (int i = -6; i < 6; ++i)
	{
		uv = t + float2(tu * i, 0);
		vOutColor.vBlurBloom += Weight[6 + i] * g_tex_0.Sample(g_sam_2, uv);
	}

	for (int i = -6; i < 6; ++i)
	{
		uv = t + float2(0.f, tu * i);
		vOutColor.vBlurBloom += Weight[6 + i] * g_tex_0.Sample(g_sam_2, uv);
	}

	return vOutColor;
}

#endif