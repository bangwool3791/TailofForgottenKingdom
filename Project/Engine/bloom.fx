#ifndef _MERGE
#define _MERGE

#include "register.fx"

// ===================
// BLOOM
// MRT  : MRT_TYPE::BLOOM
// mesh : Rect Mesh

// Parameter
// g_float_0 : 1600 or 900
//
// g_tex_0 : blur tex
// g_tex_1 : hdr tex
// Target MRT
// BlurBloom Target
// Bloom Target

struct PS_OUT
{
	float4 vBlurBloom : SV_Target0;
	//float4 vBloom : SV_Target1;
};

struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
};

VS_OUT VS_Bloom(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;
	output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
	return output;
}

PS_OUT PS_Bloom(VS_OUT _in) : SV_Target
{
	PS_OUT vOutColor = (PS_OUT)0.f;
	
	float BrightColor = 0.f;

	float2 vUV = _in.vPosition.xy / g_vRenderResolution;

	float4 vViewPos = g_tex_1.Sample(g_sam_0, vUV);

	if (1.f != vViewPos.a)
		discard;

	float4 blurbloom = g_tex_0.Sample(g_sam_0, vUV);

	float brightness = dot(blurbloom.rgb, float3(g_vec4_0.x, g_vec4_0.y, g_vec4_0.z));

	if (brightness > g_float_0)
		vOutColor.vBlurBloom = float4(blurbloom.rgb, 1);


	return vOutColor;
}

#endif