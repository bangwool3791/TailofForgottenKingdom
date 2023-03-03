#ifndef _FOG
#define _FOG

#include "register.fx"

// ===========================
// Std3DShader
// g_float_0 : fog start
// g_float_1 : fog end
// ===========================

#define fogStart g_float_0
#define fogEnd   g_float_1

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float fogFactor : FOG;
};


PixelInputType FogVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 cameraPosition;


    // ������ ��� ����� ���� ��ġ ���͸� 4 ������ �����մϴ�.
    input.position.w = 1.0f;

    // ����, �� �� ���� ��Ŀ� ���� ������ ��ġ�� ??����մϴ�.
    output.position = mul(output.position, g_matWVP);

    // �ȼ� ���̴��� �ؽ�ó ��ǥ�� �����Ѵ�.
    output.tex = input.tex;

    // ī�޶� ��ġ�� ����մϴ�.
    cameraPosition = mul(input.position, g_matWV);

    // ���� �Ȱ��� ����մϴ�.      
    output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));

    return output;
}

float4 FogPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float4 fogColor;
    float4 finalColor;

    // �� ��ġ���� �ؽ�ó �ȼ��� ���ø��մϴ�.
    textureColor = g_tex_0.Sample(g_sam_0, input.tex);

    // �Ȱ��� ���� ȸ������ �����մϴ�.
    fogColor = float4(0.5f, 0.5f, 0.5f, 1.0f);

    // �Ȱ� ȿ�� �������� ����Ͽ� ���� ������ ����մϴ�.
    finalColor = input.fogFactor * textureColor + (1.0 - input.fogFactor) * fogColor;

    return finalColor;
}

#endif