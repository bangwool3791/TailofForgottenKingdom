#ifndef _MERGE
#define _MERGE

#include "register.fx"

// ===================
// Merge
// MRT  : MRT_TYPE::SWAPCHAIN
// mesh : Rect Mesh

// Parameter
// g_float_0 : AvgLum
// ===================

struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
};

VS_OUT VS_Merge(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;
	output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
	return output;
}

/*
* Color
* Position
* Diffuse
* Specular
* Draw Swap Chain Render Target
*/
float4 PS_Merge(VS_OUT _in) : SV_Target
{
	float4 vOutColor = (float4)0.f;

	float2 vUV = _in.vPosition.xy / g_vRenderResolution;

	float4 vViewPos = g_tex_1.Sample(g_sam_0, vUV);

	//위치 정보가 없는 픽셀 discard
	if (1.f != vViewPos.a)
		discard;

	//색상 텍스쳐 참조
	vOutColor = g_tex_0.Sample(g_sam_0, vUV);
	vOutColor += g_tex_4.Sample(g_sam_0, vUV);

	float4 vDiffuse = g_tex_2.Sample(g_sam_0, vUV);
	float4 vSpecular = g_tex_3.Sample(g_sam_0, vUV);
	vOutColor.rgb = (vOutColor.rgb * vDiffuse.rgb) + vSpecular.rgb;
	return vOutColor;

}
#endif