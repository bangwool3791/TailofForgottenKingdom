#ifndef _PARTICLEUPDATE
#define _PARTICLEUPDATE

#include "register.fx"
#include "struct.fx"
#include "func.fx"

RWStructuredBuffer<tLightShare>      LightBuffer : register(u0);

#define LightData		LightBuffer[0]
#define Resolution	    g_vec2_0
//https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/dx-graphics-hlsl-to-samplelevel
//https://gamedev.stackexchange.com/questions/116392/sampling-in-the-compute-shader-not-working
[numthreads(32, 32, 1)]
void CS_Light(uint2 _id : SV_DispatchThreadID)
{
	if ((uint) Resolution.x <= _id.x || (uint)Resolution.y <= _id.y)
	{
		return;
	}

	float2 vUV = float2(_id.xy) / float2(1600, 900);
	float4 vTex = g_tex_1.SampleLevel(g_sam_1, vUV, 0);

	uint R = asint(vTex.r * 100.f); 
	uint G = asint(vTex.g * 100.f); 
	uint B = asint(vTex.z * 100.f);
	uint A = asint(vTex.w * 100.f);

	InterlockedMax(LightData.R, R, R);
	InterlockedMax(LightData.G, G, G);
	InterlockedMax(LightData.B, B, B);
	InterlockedMax(LightData.A, A, A);
}

#endif
//LightData.iMax = 1;
//uint4 vResult = g_tex_0.SampleLevel(g_sam_0, vUV, 0);
//uint iResult = (0xff000000 & vResult.x << 24) | (0x00ff0000 & (vResult.y << 16)) | (0x0000ff00 & vResult.z << 8) | (0x000000ff & vResult.w << 0);