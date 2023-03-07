#ifndef _MERGE
#define _MERGE

#include "register.fx"

// ===================
// BLUR
// MRT  : MRT_TYPE::BLUR
// mesh : Rect Mesh

// Parameter
// g_float_0 : 1600 or 900
//
// g_tex_0 : blur target
// 
// Target MRT
// Blur Target

static const float Weight[13] =
{
	0.4561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 
	1, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.4561
};

static const float Total = 6.2108;

struct PS_OUT
{
	float4 vBlur : SV_Target0;
};

struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
};

VS_OUT VS_Blur(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;
	output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
	return output;
}

float4 PS_Blurx(VS_OUT _in) : SV_Target
{
	float4 vOutColor = (float4)0.f;

	float2 t = _in.vPosition.xy / g_vRenderResolution;

	float4 vViewPos = g_tex_1.Sample(g_sam_0, t);

	//위치 정보가 없는 픽셀 discard
	if (1.f != vViewPos.a)
		discard;

	float2 uv = 0;
	
	float tu = 1.f / g_float_0;

	for (int i = -6; i < 6; ++i)
	{
		uv = t + float2(tu * i, 0);
		vOutColor += Weight[6 + i] * g_tex_0.Sample(g_sam_2, uv);
	}

	return vOutColor;
}

float4 PS_Blury(VS_OUT _in) : SV_Target
{
	float4 vOutColor = (float4)0.f;

	float2 t = _in.vPosition.xy / g_vRenderResolution;

	float4 vViewPos = g_tex_1.Sample(g_sam_0, t);

	//위치 정보가 없는 픽셀 discard
	if (1.f != vViewPos.a)
		discard;

	float2 uv = 0;

	float tu = 1.f / g_float_0;

	for (int i = -6; i < 6; ++i)
	{
		uv = t + float2(0.f, tu * i);
		vOutColor += Weight[6 + i] * g_tex_0.Sample(g_sam_2, uv);
	}

	return vOutColor;
}
#endif