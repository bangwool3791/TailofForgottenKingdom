#ifndef _REFlECTION2
#define _REFlECTION2

#include "register.fx"

struct VS_IN
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMWORLD;
};

float4 PS_EnvMappedScene_NoTexture(VS_IN _in) : SV_Target
{
	float4 PosWorld = mul(_in.Pos, g_matWorld);
	float3 E = normalize(PosWorld - 0);
	float3 N = normalize(_in.Normal);
	float3 R = reflect(E, N);
	
	float4 CubeSample = g_texcube_0.Sample(g_sam_0, R);

	return CubeSample;
}

#endif