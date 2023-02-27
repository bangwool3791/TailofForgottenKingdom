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
    * 4���� RectMesh ��ǥ * �ػ� * ī�޶� ������ + ī�޶� ���� ��ġ
    * -0.5 ~ 0.5 
    * x : 1600, y : 800 -> -800,800, -400, 400
    */
    float2 vWorldPos = _in.vPos.xy * RenderResolution * CameraScale + CameraWorldPos;

    /*
    * ���� �׽�Ʈ �� 1���� less
    */
    output.vPos = float4(_in.vPos.xy * 2.f, 0.999f, 1.f);
    output.vUV = _in.vUV;

    output.vWorldPos = vWorldPos;

    return output;
}

float4 PS_Grid(VTX_OUT _in) : SV_Target
{
    /*
    �β� = �β� ��� * ī�޶� ������
    ī�޶� ������ ���� -> �ػ� ���� -> ���� ��� -> �β� ����
    ī�޶� ������ ���� -> �ػ� ���� -> ���� Ȯ�� -> �β� ����
    */
    
    float fThick = Thickness * CameraScale;
    
/*
* �β� ������ �Ҽ��� �ڸ��� ���� �ڸ��� �ø� ��
* �ٽ� �ε� �Ҽ��������� ����, �� �Ѵ�.
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