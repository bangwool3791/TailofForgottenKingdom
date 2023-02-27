
//#ifndef _INTERFACERENDERER
//#define _INTERFACERENDERER
//
//#include "register.fx"
//#include "func.fx"
//
//#define Atlas           g_int_0
//#define UVLeftTop       g_vec2_0
//#define UVSlice         g_vec2_1
//
//#define UVLeftTop2     g_vec2_2
//#define UVSlice2       g_vec2_3
//
//struct VS_IN
//{
//    float3 vPos     : POSITION;
//    float2 vUV      : TEXCOORD;
//    uint iInstance  : SV_InstanceID;
//};
//
//struct VS_OUT
//{
//    float4 vPos      : SV_Position;
//    float2 vUV       : TEXCOORD;
//    float4 vWorldPos : POSITION;
//    uint iInstance   : SV_InstanceID;
//};
//
//
//VS_OUT VS_UI(VS_IN _in)
//{
//    VS_OUT output = (VS_OUT)0.f;
//
//    output.vPos = mul(float4(_in.vPos, 1.f), b[_in.iInstance].matWVP);
//    output.vUV = _in.vUV;
//    output.vWorldPos = mul(float4(_in.vPos, 1.f), b[_in.iInstance].matWorld);
//    output.iInstance = _in.iInstance;
//
//    return output;
//}
//
//float4 PS_UI(VS_OUT _in) : SV_Target
//{
//    float4 vOutColor = float4(1.f, 1.f, 1.f, 1.f);
//    float4 color1 = float4(1.f, 1.f, 1.f, 0.5f);
//    float4 color2 = float4(1.f, 1.f, 1.f, 0.5f);
//    float4 alphaValue = float4(0.5f, 0.5f, 0.5f, 0.5f);
//
//    float2 vUV = (b[_in.iInstance].vec2_0 + b[_in.iInstance].vec2_1 * _in.vUV);
//
//    if (b[_in.iInstance].iAnim2DUse)
//    {
//        float2 vDiff = (b[_in.iInstance].vFullSize - b[_in.iInstance].vSlice) / 2.f;
//        float2 vUV = (b[_in.iInstance].vLeftTop - vDiff - b[_in.iInstance].vOffset) + (b[_in.iInstance].vFullSize * _in.vUV);
//
//        if (vUV.x < b[_in.iInstance].vLeftTop.x || b[_in.iInstance].vLeftTop.x + b[_in.iInstance].vSlice.x < vUV.x
//            || vUV.y < b[_in.iInstance].vLeftTop.y || b[_in.iInstance].vLeftTop.y + b[_in.iInstance].vSlice.y < vUV.y)
//        {
//            discard;
//        }
//
//        float2 vUV2 = (b[_in.iInstance].vec2_2 + b[_in.iInstance].vec2_3 * _in.vUV);
//
//        color1 = g_Atals.Sample(g_sam_0, vUV);
//        color2 = g_tex_1.Sample(g_sam_0, vUV2);
//        alphaValue = g_tex_2.Sample(g_sam_0, _in.vUV);
//
//        float4 blendColor = (alphaValue * color1) + ((1.0 - alphaValue) * color2);
//        return blendColor;
//    }
//    else if (0 < b[_in.iInstance].vec2_3.x && 0 < b[_in.iInstance].vec2_3.y)
//    {
//        float2 vUV2 = (b[_in.iInstance].vec2_2 + b[_in.iInstance].vec2_3 * _in.vUV);
//        if (vUV.x < UVLeftTop.x
//            || UVLeftTop.x + UVSlice.x < vUV.x
//            || vUV.y < UVLeftTop.y
//            || UVLeftTop.y + UVSlice.y < vUV.y)
//        {
//            discard;
//        }
//
//        color1 = g_tex_0.Sample(g_sam_0, vUV);
//        color2 = g_tex_1.Sample(g_sam_0, vUV2);
//        alphaValue = g_tex_2.Sample(g_sam_0, _in.vUV);
//
//        float4 blendColor = (alphaValue * color1) + ((1.0 - alphaValue) * color2);
//        return blendColor;
//    }
//    else
//    {
//        vOutColor = g_tex_0.Sample(g_sam_0, vUV);
//    }
//
//    if (0.f == vOutColor.a)
//        discard;
//
//
//    //堡盔 贸府
///*   tLightColor color = (tLightColor)0.f;
//
//   for (int i = 0; i < g_iLight2DCount; ++i)
//   {
//       CalcLight2D(color, _in.vWorldPos, i);
//   }*/
//
//    //vOutColor *= color.vDiff;
//
//   return vOutColor;
//}
//#endif

#ifndef _INTERFACERENDERER
#define _INTERFACERENDERER

#include "register.fx"
#include "func.fx"

#define Atlas           g_int_0
#define UVLeftTop       g_vec2_0
#define UVSlice         g_vec2_1

#define UVLeftTop2     g_vec2_2
#define UVSlice2       g_vec2_3

struct VS_IN
{
    float3 vPos     : POSITION;
    float2 vUV      : TEXCOORD;
};

struct VS_OUT
{
    float4 vPos      : SV_Position;
    float2 vUV       : TEXCOORD;
    float4 vWorldPos : POSITION;
};


VS_OUT VS_UI(VS_IN _in)
{
    VS_OUT output = (VS_OUT)0.f;

    output.vPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
 

    return output;
}

float4 PS_UI(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 1.f, 1.f, 1.f);
    float4 color1 = float4(1.f, 1.f, 1.f, 0.5f);
    float4 color2 = float4(1.f, 1.f, 1.f, 0.5f);
    float4 alphaValue = float4(0.5f, 0.5f, 0.5f, 0.5f);

    float2 vUV = (UVLeftTop + UVSlice * _in.vUV);

    if (g_iAnim2DUse)
    {
        float2 vDiff = (g_vFullSize - g_vSlice) / 2.f;
        float2 vUV = (g_vLeftTop - vDiff - g_vOffset) + (g_vFullSize * _in.vUV);

        if (vUV.x < g_vLeftTop.x || g_vLeftTop.x + g_vSlice.x < vUV.x
            || vUV.y < g_vLeftTop.y || g_vLeftTop.y + g_vSlice.y < vUV.y)
        {
            discard;
        }

        float2 vUV2 = (UVLeftTop2 + UVSlice2 * _in.vUV);

        color1 = g_Atals.Sample(g_sam_0, vUV);
        color2 = g_tex_1.Sample(g_sam_0, vUV2);
        alphaValue = g_tex_2.Sample(g_sam_0, _in.vUV);

        float4 blendColor = (alphaValue * color1) + ((1.0 - alphaValue) * color2);
        return blendColor;
    }
    else if (0 < UVSlice2.x && 0 < UVSlice2.y)
    {
        float2 vUV2 = (UVLeftTop2 + UVSlice2 * _in.vUV);
        //if (vUV.x < UVLeftTop.x
        //    || UVLeftTop.x + UVSlice.x < vUV.x
        //    || vUV.y < UVLeftTop.y
        //    || UVLeftTop.y + UVSlice.y < vUV.y)
        //{
        //    discard;
        //}

        color1 = g_tex_0.Sample(g_sam_0, vUV);
        color2 = g_tex_1.Sample(g_sam_0, vUV2);
        alphaValue = g_tex_2.Sample(g_sam_0, _in.vUV);

        float4 blendColor = (alphaValue * color1) + ((1.0 - alphaValue) * color2);
        return blendColor;
    }
    else
    {
        vOutColor = g_tex_0.Sample(g_sam_0, vUV);
    }

    if (0.f == vOutColor.a)
        discard;


    // 堡盔 贸府
 /*   tLightColor color = (tLightColor)0.f;

    for (int i = 0; i < g_iLight2DCount; ++i)
    {
        CalcLight2D(color, _in.vWorldPos, i);
    }*/

    // vOutColor *= color.vDiff;

     return vOutColor;

}
#endif

