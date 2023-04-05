#ifndef _CSSHADOW
#define _CSSHADOW

#include "register.fx"

RWTexture2D<float4> tex : register(u0);

#define Resolution	g_vec2_0

// SV_GroupID           : 스레드가 속한 그룹의 좌표
// SV_GoupThreadID      : 그룹 내에서, 스레드의 좌표
// SV_GoupIndex         : 그룹 내에서, 스레드의 인덱스 (1차원)
// SV_DispatchThreadID  : 전체 스레드(모든 그룹 통합) 기준으로, 호출된 스레드의 좌표

[numthreads(32, 32, 1)]//그룹 당 스레드 개수 (최대 1024 까지 지정 가능)

void CS_ShadowMap(uint3 _id : SV_DispatchThreadID)
{
	if ((uint) Resolution.x <= _id.x || (uint)Resolution.y <= _id.y)
	{
		return;
	}

	float2 vUV = float2(_id.xy) / Resolution;
	float fDepth = g_tex_1.SampleLevel(g_sam_0, vUV, 0).r;

	const float dx = SMAP_DX;
	
	tex[vUV.xy] = float4(0.f, 0.f, 0.f, 1.f);

	const float2 offsets[9] =
	{
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		float2 vUV = _id.xy + offsets[i];
		if (1 != fDepth)
		{
			if (0 <= vUV.x && vUV.x < Resolution.x && 0 <= vUV.y && vUV.y < Resolution.y)
			{
				tex[vUV.xy] = float4(fDepth, 0.f, 0.f, 1.f);
			}
		}
	}
}
#endif