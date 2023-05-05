#ifndef _FOG_ALPHABLEND
#define _FOG_ALPHABLEND

#include "register.fx"
#include "struct.fx"
#include "func.fx"

/*
* g_float_1 : fogStart
* g_float_2 : fogRange
*/

struct VS_IN
{
	float3 vPos : POSITION;
	float2 vUV : TEXCOORD;

};

struct VTX_IN_INST
{
	float3 vPos : POSITION;
	float2 vUV : TEXCOORD;

	// Per Instance Data
	row_major matrix matWorld : WORLD;
	row_major matrix matWV : WV;
	row_major matrix matWVP : WVP;
	uint             iRowIndex : ROWINDEX;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
	float4 vWorldPos : POSITION;
	float2 vUV : TEXCOORD;
};

VS_OUT VS_Fog(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;

	//output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
	//output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWV);
	output.vUV = _in.vUV;

	return output;
}

VS_OUT VS_Fog_Inst(VTX_IN_INST _in)
{
	VS_OUT output = (VS_OUT)0.f;

	output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
	//output.vPosition = mul(float4(_in.vPos, 1.f), _in.matWVP);
	//output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
	output.vUV = _in.vUV;

	return output;
}


float4 PS_Fog(VS_OUT _in) : SV_Target
{
	float4 output = (float4)0.f;

	float4 vObjColor = g_RTCopyTex.Sample(g_sam_0, _in.vUV);
	float3 vViewPos = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;

	//위치 정보가 없는 픽셀 discard
	//float3 vCamPos = g_vec4_1.xyz;
	float distToEye = length(vViewPos);

	if (g_int_0)
	{
		float fogLerp = saturate((distToEye - g_float_0) / g_float_1);
		vObjColor = lerp(vObjColor, g_vec4_0, fogLerp);
	}
	else {
		vObjColor = g_vec4_0;
	}

	if (vObjColor.a < g_float_2)
		discard;

	//float2 fAdd = float2(g_Noise.Sample(g_sam_0, _in.vUV + g_fAccTime * 0.2f).x
	//	, g_Noise.Sample(g_sam_0, _in.vUV + float2(0.1f, 0.f) + g_fAccTime * 0.2f).x);
	//vObjColor.a = fAdd.x;
	return vObjColor;
}

#endif