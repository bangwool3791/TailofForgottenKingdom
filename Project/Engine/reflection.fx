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
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 reflectionPosition : TEXCOORD1;
};

VS_OUT ReflectionVertexShader(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;
	row_major matrix reflectProjectWorld;

	_in.position.w = 1.0f;

	output.position = mul(_in.position, g_matWVP);
	output.tex = _in.tex;

	reflectProjectWorld = mul(g_matReflection, g_matProj);
	reflectProjectWorld = mul(g_matWorld, reflectProjectWorld);
	//WVP
	//입력 메쉬 로컬 좌표를 반사 좌표계로 전환
	output.reflectionPosition = mul(_in.position, reflectProjectWorld);

	return output;
}

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 reflectionPosition : TEXCOORD1;
};

float4 ReflectionPixelShader(PS_IN _in) : SV_Target
{
	float4 textureColor;
	float2 reflectTexCoord;
	float4 reflectionColor;
	float4 color;

	//Floor
	textureColor = g_tex_0.Sample(g_sam_0, _in.tex);

	reflectTexCoord.x = _in.reflectionPosition.x / _in.reflectionPosition.w / 2.f + 0.5f;
	reflectTexCoord.y = -_in.reflectionPosition.y / _in.reflectionPosition.w / 2.f + 0.5f;
	
	//Render Target View
	reflectionColor = g_tex_1.Sample(g_sam_0, reflectTexCoord);

	// Do a linear interpolation between the two textures for a blend effect.
	color = lerp(textureColor, reflectionColor, 0.15f);

	return color;
}
#endif