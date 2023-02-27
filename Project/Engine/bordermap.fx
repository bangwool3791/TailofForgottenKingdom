#ifndef _BORDERMAP
#define _BORDERMAP

#include "func.fx"
#include "register.fx"

// ===============
// TileMap Shader
#define CamerWorldPos       g_vec4_0.xyz
#define RenderResolution    g_vec4_1.xyz
#define CameraScale         g_float_0
#define Thickness           g_float_1
#define GridInterval        g_float_2    
#define GridColor           g_vec4_1

#define TERRAINCX              64.f
#define TERRAINCZ              64.f
// ===============

struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPos : SV_Position;
    float2 vUV : TEXCOORD;

    float3 vWorldPos : POSITION;
};

VTX_OUT VS_Border(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT)0.f;

    //float3 vWorldPos = _in.vPos.xyz * RenderResolution * CameraScale + CamerWorldPos;

    output.vPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos.xyz, 1.f), g_matWorld);
    output.vUV = _in.vUV;

    //output.vWorldPos = vWorldPos;

    return output;
}

float4 PS_Border(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 1.f, 1.f, 1.f);
    return GridColor;
    //float fThick = Thickness * CameraScale;

    //uint iWorldX = (int)_in.vWorldPos.x;
    //uint iRestX = (iWorldX + (uint) fThick / 2) % (uint) GridInterval;

    //uint iWorldY = (int)_in.vWorldPos.y;
    //uint iRestY = (iWorldY + (uint) fThick / 2) % (uint) GridInterval;

    //float		fGradient[2] = {
    //    (TERRAINCZ / 2.f) / (TERRAINCX / 2.f) * -1.f,
    //    (TERRAINCZ / 2.f) / (TERRAINCX / 2.f),
    //};

    //float x1 = -_in.vPos.z / fGradient[0] + _in.vPos.x;
    //float x2 = -_in.vPos.z / fGradient[0] + _in.vPos.x;
    //
    //if (0 <= x1 && x1 <= (uint) ((fThick / 2.f) * 2.f))
    //{
    // return GridColor;
    //}
    //discard;

    //return GridColor;
}

#endif
