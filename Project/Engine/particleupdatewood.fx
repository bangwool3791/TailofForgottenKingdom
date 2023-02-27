#ifndef _PARTICLEUPDATE
#define _PARTICLEUPDATE

#include "register.fx"
#include "func.fx"

RWStructuredBuffer<tParticle> ParticleWoodBuffer : register(u0);
RWStructuredBuffer<tParticleShare> ParticleWoodShare : register(u1);

/*
* 시작 위치, 방향, 폭발 위치
*/
#define PARTICLE_MAX	g_int_0
#define Particle		ParticleWoodBuffer[_id.x]
#define ShareData		ParticleWoodShare[0]

#define SpawnRange      g_float_0
#define IsWorldSpawn    g_int_1
#define	ParticleType	g_int_2
#define ObjectWorldPos  g_vec4_0
#define	MinSpeed		g_vec2_0.x
#define	MaxSpeed		g_vec2_0.y
#define	MinLife			g_vec2_1.x
#define	MaxLife			g_vec2_1.y

[numthreads(128, 1, 1)]
void CS_ParticleUpdate(uint _id : SV_DispatchThreadID)
{
	if (PARTICLE_MAX <= _id.x)
		return;

	if (0 == Particle.iActive)
	{
		while (0 < ShareData.iAliveCount)
		{
			int iOriginValue = ShareData.iAliveCount;
			int iExchange = iOriginValue - 1;

			//InterlockedExchange(ShareData.iAliveCount, iExchange, iExchange);
			InterlockedCompareExchange(ShareData.iAliveCount, iOriginValue, iExchange, iExchange);

			if (iOriginValue == iExchange)
			{
				Particle.iActive = 1;
				break;
			}
		}

		if (Particle.iActive)
		{
			float4 vRandom = (float4)0.f;

			float2 vUV = float2((float)_id.x / PARTICLE_MAX, 0.5f);
			vUV.x += g_fAccTime;
			vUV.y += sin((vUV.x + g_fAccTime) * 3.141592f * 2.f * 10.f) * 0.5f;

			vRandom = float4(GaussianSample(vUV + float2(0.0f, 0.f)).x,
				GaussianSample(vUV + float2(0.1f, 0.f)).x,
				GaussianSample(vUV + float2(0.2f, 0.f)).x,
				GaussianSample(vUV + float2(0.3f, 0.f)).x);

			float fTheta = vRandom.x * 3.141592f;
			Particle.vRelativePos.xyz = ObjectWorldPos.xyz;
			Particle.vDir.xy = float2(cos(fTheta), sin(fTheta));
			Particle.vRelativePos.z = 0.f;

			Particle.fCurTime = 0.f;
			Particle.fMaxTime = vRandom.w * (MaxLife - MinLife) + MinLife;
		}
	}
	else
	{
		Particle.fCurTime += g_fDT;

		if (Particle.fMaxTime < Particle.fCurTime)
		{
			Particle.iActive = 0;
		}
		else
		{
			Particle.vRelativePos += Particle.vDir * Particle.fSpeed * g_fDT;
			Particle.vRelativePos.y -= Particle.fCurTime * 0.1f;
		}
	}
}

#endif