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

float4 VS_DebugDraw(float3 _vPos : POSITION) : SV_Position
{
    float4 vPosition = (float4) 0.f;

    vPosition = mul(float4(_vPos, 1.f), g_matWVP);

    return vPosition;
}


float4 PS_DebugDraw(float4 _vPosition : SV_Position) : SV_Target
{
    float4 vOutColor = COLOR;

    return vOutColor;
}

#endif
