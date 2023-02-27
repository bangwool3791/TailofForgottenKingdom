#ifndef _TOOL_ANIMATION
#define _TOOL_ANIMATION

#include "register.fx"

#define AnimationTex    g_tex_0

struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};


struct VTX_OUT
{
    float4 vPos : SV_Position;
    float3 vWorldPos : POSITION;
    float2 vUV : TEXCOORD;
};


VTX_OUT VS_RefAni(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT)0.f;

    output.vPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    output.vUV = _in.vUV;

    return output;
}

float4 PS_RefAni(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);

    vOutColor = AnimationTex.Sample(g_sam_1, _in.vUV);

    return vOutColor;
}

#endif