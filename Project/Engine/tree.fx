#ifndef _TREE
#define _TREE

#include "register.fx"
#include "func.fx"

// ======================
// Object Render Shader
// RS_TYPE : CULL_NONE
// BS_TYPE : NONALPAHBLEN
// DS_TYPE : LESS
// DOMAIN : 

#define Atlas         g_int_0
#define UVLeftTop     g_vec2_0
#define UVSlice       g_vec2_1

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


VS_OUT VS_TreeRender(VS_IN _in)
{
    VS_OUT output = (VS_OUT)0.f;

    output.vPos = mul(float4(_in.vPos, 1.f), b[_in.iInstance].matWVP);
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vPos, 1.f), b[_in.iInstance].matWorld);
    output.iInstance = _in.iInstance;

    return output;
}

float4 PS_TreeRender(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);

    vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);

    if (0.f == vOutColor.a)
        discard;


    // ±¤¿ø Ã³¸®
 /*   tLightColor color = (tLightColor)0.f;

    for (int i = 0; i < g_iLight2DCount; ++i)
    {
        CalcLight2D(color, _in.vWorldPos, i);
    }*/

    // vOutColor *= color.vDiff;

     return vOutColor;

}
#endif