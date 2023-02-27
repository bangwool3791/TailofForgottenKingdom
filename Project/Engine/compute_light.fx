#ifndef _PARTICLEUPDATE
#define _PARTICLEUPDATE

#include "register.fx"
#include "struct.fx"
#include "func.fx"

RWStructuredBuffer<tLightShare>      LightBuffer : register(u0);
/*
* 시작 위치, 방향, 폭발 위치
*/
#define LightData		LightBuffer[0]

//https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/dx-graphics-hlsl-to-samplelevel
//https://gamedev.stackexchange.com/questions/116392/sampling-in-the-compute-shader-not-working
[numthreads(32, 32, 1)]
void CS_Light(uint2 _id : SV_DispatchThreadID)
{
	if (g_int_0 < _id.x * _id.y)
		return;

	if (g_btex_0)
	{
		float2 vUV = float2(_id.xy) / float2(1600, 900);
		uint4 vResult = g_tex_0.SampleLevel(g_sam_0, vUV, 0);
		uint iResult = (0xff000000 & vResult.x << 24) | (0x00ff0000 & (vResult.y << 16)) | (0x0000ff00 & vResult.z << 8) | (0x000000ff & vResult.w << 0);
		bool bCheck = LightData.iMax < iResult;
		if (bCheck)
		{
			uint iOrigin = LightData.iMax;
			uint iEx = iResult;
			InterlockedCompareExchange(LightData.iMax, iOrigin, iEx, iEx);
			//InterlockedCompareExchange(LightData.vMax.y, vOrigin.y, vEx.y, vEx.y);
			//InterlockedCompareExchange(LightData.vMax.z, vOrigin.z, vEx.z, vEx.z);
			//InterlockedCompareExchange(LightData.vMax.w, vOrigin.w, vEx.w, vEx.w);
		}
	}
}

#endif