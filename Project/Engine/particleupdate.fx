#ifndef _PARTICLEUPDATE
#define _PARTICLEUPDATE

#include "register.fx"
#include "func.fx"

RWStructuredBuffer<tParticle> ParticleBuffer : register(u0);
RWStructuredBuffer<tParticleShare> ParticleShare : register(u1);


#define PARTICLE_MAX    g_int_0
#define Particle        ParticleBuffer[_id.x]
#define ShareData       ParticleShare[0]

#define SpawnRange      g_float_0
#define Acceleration    g_float_1
#define Velocity        g_float_2
#define IsWorldSpawn    g_int_1
#define ObjectWorldPos  g_vec4_0
#define MinSpeed        g_vec2_0.x
#define MaxSpeed        g_vec2_0.y
#define MinLife         g_vec2_1.x
#define MaxLife         g_vec2_1.y



[numthreads(128, 1, 1)]
void CS_ParticleUpdate(uint3 _id : SV_DispatchThreadID)
{
    if ((uint) PARTICLE_MAX <= _id.x)
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
            // 랜덤값으로 위치와 방향을 설정한다.
            // 샘플링을 시도할 UV 를 계산한다.

            float4 vRandom = (float4) 0.f;

            float2 vUV = float2((float)_id.x / PARTICLE_MAX, 0.5f);
            vUV.x += g_fAccTime;
            vUV.y += sin((vUV.x + g_fAccTime) * 3.141592f * 2.f * 10.f) * 0.5f;

            vRandom = float4(GaussianSample(vUV + float2(0.f, 0.f)).x, GaussianSample(vUV + float2(0.1f, 0.f)).x, GaussianSample(vUV + float2(0.2f, 0.f)).x, GaussianSample(vUV + float2(0.3f, 0.f)).x);


            // 사각형범위로 스폰
            // Particle.vRelativePos.xyz = vRandom.xyz * SpawnRange - SpawnRange / 2.f;

            // 원형 범위로 스폰
            float fTheta = vRandom.x * 3.141592f * 2.f;
            /*           if (3.141592f * 0.5 > fTheta)
                       {
                           fTheta += 3.141592f * 0.5;
                       }
                       else if (3.141592f * 1.5 < fTheta)
                       {
                           fTheta -= 3.141592f * 0.5;
                       }*/

            Particle.vRelativePos.xz = float2(cos(fTheta), sin(fTheta)) * 1.f;
            Particle.vRelativePos.y = vRandom.z * 1.f;

            // 파티클 방향
            Particle.vDir.xyz = -normalize(float3(Particle.vRelativePos.xyz));
            Particle.vDir.y *= -1.f;
            if (IsWorldSpawn)
            {
                Particle.vRelativePos.xyz += ObjectWorldPos.xyz;
            }

            // 파티클 속력
            Particle.fSpeed = vRandom.z * (MaxSpeed - MinSpeed) + MinSpeed;

            // 파티클 Life
            Particle.fCurTime = 0.f;
            Particle.fMaxTime = vRandom.w * (MaxLife - MinLife) + MinLife;
        }
    }

    // 활성화 된 파티클을 업데이트
    else
    {
        Particle.fCurTime += g_fDT;
        if (Particle.fMaxTime < Particle.fCurTime)
        {
            Particle.iActive = 0;
        }
        else
        {
            Particle.vRelativePos += ParticleBuffer[_id.x].vDir * ParticleBuffer[_id.x].fSpeed * g_fDT;
        }
    }
}

#endif
