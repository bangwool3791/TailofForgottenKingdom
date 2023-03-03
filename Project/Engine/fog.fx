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


    // 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;

    // 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ??계산합니다.
    output.position = mul(output.position, g_matWVP);

    // 픽셀 쉐이더의 텍스처 좌표를 저장한다.
    output.tex = input.tex;

    // 카메라 위치를 계산합니다.
    cameraPosition = mul(input.position, g_matWV);

    // 선형 안개를 계산합니다.      
    output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));

    return output;
}

float4 FogPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float4 fogColor;
    float4 finalColor;

    // 이 위치에서 텍스처 픽셀을 샘플링합니다.
    textureColor = g_tex_0.Sample(g_sam_0, input.tex);

    // 안개의 색을 회색으로 설정합니다.
    fogColor = float4(0.5f, 0.5f, 0.5f, 1.0f);

    // 안개 효과 방정식을 사용하여 최종 색상을 계산합니다.
    finalColor = input.fogFactor * textureColor + (1.0 - input.fogFactor) * fogColor;

    return finalColor;
}

#endif