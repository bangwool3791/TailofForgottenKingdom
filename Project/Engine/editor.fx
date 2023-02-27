#ifndef _EDITOR
#define	_EDITOR

#include "register.fx"

// ================
// Grid Shader
// mesh : RectMesh
// g_vec2_0 : Render Resolution
// g_vec4_0 : Camera World Position
// g_float_0 : Camera Scale
// ================

#define RenderResolution    g_vec2_0
#define CameraWorldPos      g_vec4_0.xy
#define CameraScale         g_float_0
#define Thickness           g_float_1
#define GridInterval        g_float_2
#define GridColor           g_vec4_1

struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPos : SV_Position;
    float2 vUV : TEXCOORD;

    float2 vWorldPos : POSITION;
};

VTX_OUT VS_Grid(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT)0.f;
    /*
    * 4개의 RectMesh 좌표 * 해상도 * 카메라 스케일 + 카메라 월드 위치
    * -0.5 ~ 0.5 
    * x : 1600, y : 800 -> -800,800, -400, 400
    */
    float2 vWorldPos = _in.vPos.xy * RenderResolution * CameraScale + CameraWorldPos;

    /*
    * 깊이 테스트 시 1보다 less
    */
    output.vPos = float4(_in.vPos.xy * 2.f, 0.999f, 1.f);
    output.vUV = _in.vUV;

    output.vWorldPos = vWorldPos;

    return output;
}

float4 PS_Grid(VTX_OUT _in) : SV_Target
{
    /*
    두께 = 두께 상수 * 카메라 스케일
    카메라 스케일 증가 -> 해상도 증가 -> 월드 축소 -> 두께 증가
    카메라 스케일 감소 -> 해상도 감소 -> 월드 확대 -> 두께 감소
    */
    
    float fThick = Thickness * CameraScale;
    
/*
* 두께 정도를 소수점 자리를 정수 자리로 올린 후
* 다시 부동 소수형식으로 만들어서, 비교 한다.
*/

    uint iWorldX = (int)_in.vWorldPos.x;
    uint iResetX = (iWorldX + (uint)fThick /2) % (uint)GridInterval;

    uint iWorldY = (int)_in.vWorldPos.y;
    uint iResetY = (iWorldY + (uint)fThick / 2) % (uint)GridInterval;

    if (0.f <= iResetX && iResetX <= (uint)fThick
        || 0.f <= iResetY && iResetY <= (uint)fThick)
    {
        return GridColor;
    }

    discard;

    return GridColor;

}

#endif