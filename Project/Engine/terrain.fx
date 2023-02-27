#ifndef _TILEMAP
#define _TILEMAP

#include "func.fx"
#include "register.fx"

// ===============
// TileMap Shader
#define Size         g_vec4_0
// ===============

struct VS_IN
{
    float3 vPos         : POSITION;
    float2 vUV          : TEXCOORD;
    float4 vColor       : COLOR;
};


struct VS_OUT
{
    float4 vPos         : SV_Position;
    float2 vUV          : TEXCOORD;
    float4 vColor       : COLOR;
};

VS_OUT VS_TileMap(VS_IN _in)
{
    VS_OUT output       = (VS_OUT)0.f;
    output.vPos         = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vColor       = _in.vColor;
    output.vUV          = _in.vUV;
    return output;
}

float4 PS_TileMap(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    float4 vGreenColor = float4(0.f, 0.5f, 0.f, 1.f);
    float4 vGreenColor1 = float4(0.f, 0.8f, 0.f, 1.f);
    float4 vAlphaValue;
    float4 vBlendColor;
    tLightColor color = (tLightColor)0.f;


    if (_in.vUV.x == -1.f)
        discard;

    vOutColor = g_tex_0.Sample(g_sam_1, _in.vUV);

    if (_in.vColor.x == 1)
    {
        vAlphaValue = g_tex_32.Sample(g_sam_1, _in.vUV);
        vBlendColor = (vAlphaValue * vGreenColor) + ((1.0 - vAlphaValue) * vOutColor);
        vBlendColor = saturate(vBlendColor);
        return vBlendColor;
    }
    else if (_in.vColor.x == 2)
    {
        vAlphaValue = g_tex_32.Sample(g_sam_1, _in.vUV);
        vBlendColor = (vAlphaValue * vGreenColor1) + ((1.0 - vAlphaValue) * vOutColor);
        vBlendColor = saturate(vBlendColor);
        return vBlendColor;
    }

    if (_in.vColor.y == 1)
    {
        vOutColor = g_tex_1.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 2)
    {
        vOutColor = g_tex_2.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 3)
    {
        vOutColor = g_tex_3.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 4)
    {
        vOutColor = g_tex_4.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 5)
    {
        vOutColor = g_tex_5.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 6)
    {
        vOutColor = g_tex_6.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 7)
    {
        vOutColor = g_tex_7.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 8)
    {
        vOutColor = g_tex_8.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 9)
    {
        vOutColor = g_tex_9.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 10)
    {
        vOutColor = g_tex_10.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 11)
    {
        vOutColor = g_tex_11.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 12)
    {
        vOutColor = g_tex_12.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 13)
    {
        vOutColor = g_tex_13.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 14)
    {
        vOutColor = g_tex_14.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 15)
    {
        vOutColor = g_tex_15.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 16)
    {
        vOutColor = g_tex_16.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 17)
    {
        vOutColor = g_tex_17.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 18)
    {
        vOutColor = g_tex_18.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 19)
    {
        vOutColor = g_tex_19.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 20)
    {
        vOutColor = g_tex_20.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 21)
    {
        vOutColor = g_tex_21.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 22)
    {
        vOutColor = g_tex_22.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 23)
    {
        vOutColor = g_tex_23.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 24)
    {
        vOutColor = g_tex_24.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 25)
    {
        vOutColor = g_tex_25.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 26)
    {
        vOutColor = g_tex_26.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 27)
    {
        vOutColor = g_tex_27.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 28)
    {
        vOutColor = g_tex_28.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 29)
    {
        vOutColor = g_tex_29.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 30)
    {
        vOutColor = g_tex_30.Sample(g_sam_1, _in.vUV);
    }
    else if (_in.vColor.y == 31)
    {
        vOutColor = g_tex_31.Sample(g_sam_1, _in.vUV);
    }

    return vOutColor;
}
#endif


  //vOutColor *= color.vDiff;
