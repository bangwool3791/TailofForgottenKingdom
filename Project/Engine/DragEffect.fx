#ifndef _DRAG_EFFECT
#define _DRAG_EFFECT

#include "register.fx"

struct VTX_IN
{
    float3 vPos : POSITION;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};


struct VTX_OUT
{
    float4 vPos : SV_Position;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};


/*
* 
*/
VTX_OUT VS_DragEffect(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT)0.f;

    float4 vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);

    output.vPos = vProjPos;
    output.vColor = _in.vColor;
    output.vUV = _in.vUV;

    return output;
}

float4 PS_DragEffect(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = float4(0.f, 1.f, 0.f, 0.5f);

    return vOutColor;
}

#endif