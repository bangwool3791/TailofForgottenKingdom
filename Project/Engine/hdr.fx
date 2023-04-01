#ifndef _HDR
#define _HDR

#include "register.fx"
// ===================
// Merge
// MRT  : MRT_TYPE::SWAPCHAIN
// mesh : Rect Mesh


//g_float_0 : avg lum
//g_float_1 : middle grey
//g_float_2 : g_fLumWhiteSqr

#define LUM_FACTOR float3(5.f, 5.f, 5.f)
float3 ToneMapping(float3 vHDRColor)
{
	// 현재 픽셀에 대한 휘도 스케일 계산
	float fLScale = dot(vHDRColor, LUM_FACTOR);
	fLScale *= g_float_1 / g_float_0;
	fLScale = (fLScale + fLScale * fLScale / g_float_2) / (1.f + fLScale);

	// 휘도 스케일을 픽셀 색상에 적용
	return vHDRColor * fLScale;
}

struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
};

VS_OUT VS_Hdr(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;
	/*
	* -1 ~ 1사이 값으로 변환
	*/
	output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
	return output;
}

/*
Swap chain 후 처리
*/
float4 PS_Hdr(VS_OUT _in) : SV_Target
{
	float4 vOutColor = (float4)0.f;

	vOutColor.rgb = ToneMapping(vOutColor.rgb);

	return vOutColor;

}
#endif