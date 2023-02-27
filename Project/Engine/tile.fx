#ifndef _STD2D
#define _STD2D

#include "register.fx"
#include "func.fx"

// HLSL 로 VertexShader 작성하기
struct VTX_IN
{
    float3 vPos         : POSITION;
    float2 vUV          : TEXCOORD;
};


struct VTX_OUT
{
    float4 vPos         : SV_Position;
    float3 vWorldPos    : POSITION;
    float2 vUV          : TEXCOORD;
};


VTX_OUT VS_Tile(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT)0.f;

    output.vPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    output.vUV = _in.vUV;

    return output;
}

float4 PS_Tile(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);

    vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);

    return vOutColor;
}


#endif


