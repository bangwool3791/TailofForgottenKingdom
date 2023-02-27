#ifndef _DEBUGDRAW
#define _DEBUGDRAW

#include "register.fx"

// ========================================================
// DebugDrawShader
// mesh                 : Rect, Circle, Cube, Sphere
// Rasterizer           : CULL_NONE 
// BlendState           : ALPHABLEND
// DepthStencilState    : NO_WRITE

// Material Scalar Parameter
#define COLOR g_vec4_0
// ========================================================

struct VS_IN
{
    float3 vPos     : POSITION;
    uint iInstance  : SV_InstanceID;
};

struct VS_OUT
{
    float4 vPos      : SV_Position;
    float4 vWorldPos : POSITION;
    uint iInstance   : SV_InstanceID;
};

VS_OUT VS_DebugDraw(VS_IN _in)
{
    VS_OUT output = (VS_OUT)0.f;

    output.vPos = mul(float4(_in.vPos, 1.f), b[_in.iInstance].matWVP);

    return output;
}


float4 PS_DebugDraw(VS_OUT _in) : SV_Target
{
    float4 vOutColor = COLOR;

    return vOutColor;
}

#endif
