#ifndef _COMPUTE
#define _COMPUTE

#include "register.fx"

RWTexture2D<float4> tex : register(u0);

#define Color	    g_vec4_0
#define Resolution	g_vec2_0

// SV_GroupID           : 스레드가 속한 그룹의 좌표
// SV_GoupThreadID      : 그룹 내에서, 스레드의 좌표
// SV_GoupIndex         : 그룹 내에서, 스레드의 인덱스 (1차원)
// SV_DispatchThreadID  : 전체 스레드(모든 그룹 통합) 기준으로, 호출된 스레드의 좌표

[numthreads(32,32,1)]//그룹 당 스레드 개수 (최대 1024 까지 지정 가능)

void CS_Paint(uint3 _id : SV_DispatchThreadID)
{
	if ((uint) Resolution.x <= _id.x || (uint)Resolution.y <= _id.y)
	{
		return;
	}
	tex[_id.xy] = Color;
}
#endif