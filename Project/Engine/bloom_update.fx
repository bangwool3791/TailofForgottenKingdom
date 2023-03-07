#ifndef _MERGE
#define _MERGE

#include "register.fx"

// ===================
// BLOOM
// MRT  : MRT_TYPE::Swap Chain
// mesh : Rect Mesh

// Parameter
//
// g_tex_0 : hdr tex
// g_tex_1 : blurbloom tex
// g_tex_2 : bloom tex
// Target MRT
// Swap Chain

struct PS_OUT
{
	float4 vOutput : SV_Target0;
};

struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
};

VS_OUT VS_BloomUpdate(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;
	output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
	return output;
}

float4 PS_BloomUpdate(VS_OUT _in) : SV_Target
{
	float4 vOutColor;
	float2 vUV = _in.vPosition.xy / g_vRenderResolution;

	//float4 vViewPos = g_tex_3.Sample(g_sam_0, vUV);
	//
	//if (1.f != vViewPos.a)
	//	discard;

	float4 vHDRColor = g_tex_0.Sample(g_sam_0, vUV);
	float4 vBloomColor = g_tex_1.Sample(g_sam_2, vUV);

	float4 Out = vHDRColor + vBloomColor;

	return Out;
}

#endif