#ifndef _INTERFACERENDERER
#define _INTERFACERENDERER

#include "register.fx"
#include "func.fx"

#define UVLeftTop       g_vec2_0
#define UVSlice         g_vec2_1

struct VS_IN
{
    float3 vPos     : POSITION;
    float2 vUV      : TEXCOORD;
    uint iInstance  : SV_InstanceID;
};

struct VS_OUT
{
    float4 vPos      : SV_Position;
    float2 vUV       : TEXCOORD;
    float4 vWorldPos : POSITION;
    uint iInstance   : SV_InstanceID;
};


VS_OUT VS_Opaque(VS_IN _in)
{
    VS_OUT output = (VS_OUT)0.f;

    output.vPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    return output;
}

float4 PS_Opaque(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);

    if (b[_in.iInstance].iAnim2DUse)
    {
        float2 vDiff = (b[_in.iInstance].vFullSize - b[_in.iInstance].vSlice) / 2.f;
        float2 vUV = (b[_in.iInstance].vLeftTop - vDiff - b[_in.iInstance].vOffset) + (b[_in.iInstance].vFullSize * _in.vUV);

        if (vUV.x < b[_in.iInstance].vLeftTop.x
            || b[_in.iInstance].vLeftTop.x + b[_in.iInstance].vSlice.x < vUV.x
            || vUV.y < b[_in.iInstance].vLeftTop.y
            || b[_in.iInstance].vLeftTop.y + b[_in.iInstance].vSlice.y < vUV.y)
        {
            discard;
        }

        vOutColor = g_Atals.Sample(g_sam_1, vUV);
    }
    else if (g_int_0)
    {
        float2 vUV = (UVLeftTop + UVSlice * _in.vUV);

        vOutColor = g_tex_0.Sample(g_sam_0, vUV);
    }
    else
    {
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }

    if (0.f == vOutColor.a)
        discard;

    return vOutColor;
}
#endif