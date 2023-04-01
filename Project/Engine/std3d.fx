#ifndef _STD3D
#define _STD3D

#include "func.fx"
#include "register.fx"

// ===========================
// Std3DShader
// g_float_0 : Spec Coefficent
// ===========================
struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;

    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;

};

VS_OUT VS_Std3D(VS_IN _in)
{
    VS_OUT output = (VS_OUT)0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;

    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);

    return output;
}


float4 PS_Std3D(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(0.6f, 0.6f, 0.6f, 1.f);

    tLightColor lightColor = (tLightColor)0.f;

    if (g_btex_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }

    float3 vNormal = _in.vViewNormal;

    if (g_btex_1)
    {
        vNormal = g_tex_1.Sample(g_sam_0, _in.vUV);
        vNormal = (vNormal * 2.f) - 1.f;

        float3x3 matTBN =
        {
            _in.vViewTangent,
            _in.vViewBinormal,
            _in.vViewNormal,
        };

        vNormal = normalize(mul(vNormal, matTBN));
    }

    for (int i = 0; i < g_iLight3DCount; ++i)
    {
        CalcLight3D(_in.vViewPos, vNormal, i, lightColor);
    }

    //물체의 색상에 광원을 적용
    vOutColor.rgb = (vOutColor.rgb * lightColor.vDiff.rgb)
                    + lightColor.vSpec.rgb * g_float_0
                    + (vOutColor.rgb * lightColor.vAmb.rgb);

    return vOutColor;
}

#endif