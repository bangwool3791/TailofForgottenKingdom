#ifndef _TILEMAP
#define _TILEMAP

#include "func.fx"
#include "register.fx"

#define NOTUSED 0
#define EMPTY 1
#define BUILD 2
#define COLLISION 3
#define USED 4
#define NOT_FOREST 5
#define BEFORE_HUNTED 6
#define HUNTED  7
#define CRYSTAL 8
#define BEFORE_CRYSTAL 9
#define WOOD 10
#define BEFROE_WOOD 11
#define HARVEST 12

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

VS_OUT VS_UiTileMap(VS_IN _in)
{
    VS_OUT output = (VS_OUT)0.f;
    output.vPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    output.vColor = _in.vColor;
    return output;
}

float4 PS_UiTileMap(VS_OUT _in) : SV_Target
{
    float4 color1;
    float4 color2;
    float4 alphaValue;
    float4 blendColor;

    if (0 == g_int_0)
        discard;

    if (g_TileBuffer[_in.vColor.x].iInfo == NOTUSED)
    {
        discard;
    }
    else if (g_TileBuffer[_in.vColor.x].iInfo == EMPTY)
    {
        color1 = float4(0.0f, 0.7f, 0.0f, 1.f);
        color2 = float4(0.0f, 0.7f, 0.0f, 1.f);
    }
    else if (g_TileBuffer[_in.vColor.x].iInfo == BUILD)
    {
        color1 = float4(0.2f, 0.5f, 0.2f, 1.f);
        color2 = float4(0.2f, 0.5f, 0.2f, 1.f);
    }
    else if (g_TileBuffer[_in.vColor.x].iInfo == COLLISION)
    {
        color1 = float4(0.2f, 0.0f, 0.0f, 1.0f);
        color2 = float4(0.2f, 0.0f, 0.0f, 1.0f);
    }
    else if (g_TileBuffer[_in.vColor.x].iInfo == USED)
    {
        discard;
    }
    else if (g_TileBuffer[_in.vColor.x].iInfo == BEFORE_HUNTED)
    {
        color1 = float4(0.0f, 2.f, 0.0f, 1.f);
        color2 = float4(0.0f, 2.f, 0.0f, 1.f);
    }
    else if (g_TileBuffer[_in.vColor.x].iInfo == HUNTED)
    {
        color1 = float4(0.0f, 0.7f, 0.0f, 1.f);
        color2 = float4(0.0f, 0.7f, 0.0f, 1.f);
    }
    else if (g_TileBuffer[_in.vColor.x].iInfo == CRYSTAL)
    {
        discard;
    }
    else if (g_TileBuffer[_in.vColor.x].iInfo == BEFORE_CRYSTAL)
    {
        color1 = float4(0.0f, 2.f, 0.0f, 1.f);
        color2 = float4(0.0f, 2.f, 0.0f, 1.f);
    }
    else if (g_TileBuffer[_in.vColor.x].iInfo == WOOD)
    {
        discard;
    }
    else if (g_TileBuffer[_in.vColor.x].iInfo == HARVEST)
    {
        discard;
    }

    alphaValue = g_tex_0.Sample(g_sam_1, _in.vUV);
    blendColor = (alphaValue * color1) + ((1.0 - alphaValue) * color2);
    blendColor = saturate(blendColor);
   return blendColor;

}

#endif
