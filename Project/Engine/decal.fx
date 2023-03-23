#ifndef _DECAL
#define _DECAL

#include "register.fx"
// =================
// DecalShader
// MRT  : MRT_TYPE::DECAL
// mesh : CubeMesh
// Blend        : AlphaBlend
// DepthStencil : No_Test_NoWrite

// g_tex_0 : Position Target Tex
// g_tex_1 : Decal Texture
// =================

struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
};

VS_OUT VS_Decal(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;

	/*
	* Cube 6면체 Position Texture
	* 0~1 읽는다.
	*/
	output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);

	return output;
}

struct PS_OUT
{
	float4 vOutColor : SV_Target;
};


PS_OUT PS_Decal(VS_OUT _in)
{
	PS_OUT output = (PS_OUT)0.f;

	// 데칼용 텍스쳐가 세팅되어있지 않으면
	if (g_btex_1 == 0)
	{
		discard;
	}

	float2 vUV = _in.vPosition.xy / g_vRenderResolution;

	float4 vViewPos = g_tex_0.Sample(g_sam_0, vUV);

	if (vViewPos.a == 0.f)
		discard;

	// 로컬 스페이스에서 큐브내에 있는지 판정한다.
	float4 vLocalPos = mul(mul(vViewPos, g_matViewInv), g_matWorldInv);
	if (abs(vLocalPos.x) > 0.5f || abs(vLocalPos.y) > 0.5f || abs(vLocalPos.z) > 0.5f)
		discard;

	float2 vDecalUV = float2(vLocalPos.x + 0.5f, 1.f - (vLocalPos.y + 0.5f));
	float4 vDecalColor = g_tex_1.Sample(g_sam_0, vDecalUV);

	output.vOutColor = vDecalColor;

	return output;
}

struct VS_IN2
{
	float3 vPos : POSITION;
	float3 vUV : TEXCOORD;

	float3 vTangent : TANGENT;
	float3 vNormal : NORMAL;
	float3 vBinormal : BINORMAL;
};

struct VS_OUT2
{
	float4 vPosition : SV_Position;
	float2 vUV : TEXCOORD;

	float3 vViewPos : POSITION;

	float3 vViewTangent : TANGENT;
	float3 vViewNormal : NORMAL;
	float3 vViewBinormal : BINORMAL;
};

VS_OUT2 VS_Decal_Defferd(VS_IN2 _in)
{
	VS_OUT2 output = (VS_OUT2)0.f;

	/*
	* Cube 6면체 Position Texture
	* 0~1 읽는다.
	*/
	output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
	output.vUV = _in.vUV;
	output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
	output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
	output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);

	return output;
}

struct PS_OUT2
{
	float4 vDecal		: SV_Target0;
};


PS_OUT2 PS_Decal_Defferd(VS_OUT2 _in)
{
	PS_OUT2 output = (PS_OUT2)0.f;

	// 데칼용 텍스쳐가 세팅되어있지 않으면
	if (g_btex_1 == 0)
		discard;

	float2 vUV = _in.vPosition.xy / g_vRenderResolution;

	float4 vViewPos = g_tex_0.Sample(g_sam_0, vUV);

	if (vViewPos.a == 0.f)
		discard;

	// 로컬 스페이스에서 큐브내에 있는지 판정한다.
	float4 vLocalPos = mul(mul(vViewPos, g_matViewInv), g_matWorldInv);
	if (abs(vLocalPos.x) > 0.5f || abs(vLocalPos.y) > 0.5f || abs(vLocalPos.z) > 0.5f)
		discard;

	float2 vDecalUV = float2(vLocalPos.x + 0.5f, 1.f - (vLocalPos.y + 0.5f));
	float4 vDecalColor = g_tex_1.Sample(g_sam_0, vDecalUV);

	vDecalColor.r = 1.f;

	output.vDecal = vDecalColor;

	return output;
}
#endif