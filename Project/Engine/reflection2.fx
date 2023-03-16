#ifndef _REFlECTION2
#define _REFlECTION2

#include "register.fx"

struct VS_IN
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
};

VS_OUT VS_EnvMappedScene_NoTexture(VS_IN _in) 
{
	VS_OUT output = (VS_OUT)0.f;
	output.Pos = _in.Pos;
	output.Normal = _in.Normal;

	return output;
}

struct PS_OUT
{
	float4 vColor : SV_Target0;
};

PS_OUT PS_EnvMappedScene_NoTexture(VS_OUT _in) : SV_Target
{
	PS_OUT CubeSample = (PS_OUT)0.f;
	float4 PosWorld = mul(_in.Pos, g_matWorld);
	float3 E = normalize(PosWorld - 0);
	float3 N = normalize(_in.Normal);
	float3 R = reflect(E, N);
	
	CubeSample.vColor = g_texcube_0.Sample(g_sam_0, _in.Normal.xyz);

	return CubeSample;
}

#endif