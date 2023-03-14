#ifndef _FUNC
#define _FUNC

#include "matrix.fx"
#include "struct.fx"
#include "register.fx"

#define PI 3.141592f

void CalcLight2D(inout tLightColor _LightColor, float3 _vWorldPos, int iLightIdx)
{
    // Directional Light
    if (0 == g_Light2DBuffer[iLightIdx].iLightType)
    {
        _LightColor.vDiff += g_Light2DBuffer[iLightIdx].color.vDiff;
    }

    // Point Light
    else if (1 == g_Light2DBuffer[iLightIdx].iLightType)
    {
        float fDistance = distance(_vWorldPos.xy, g_Light2DBuffer[iLightIdx].vWorldPos.xy);

        if (fDistance < g_Light2DBuffer[iLightIdx].fRadius)
        {
            float fRatio = 1.f - fDistance / g_Light2DBuffer[iLightIdx].fRadius;

            _LightColor.vDiff += g_Light2DBuffer[iLightIdx].color.vDiff * fRatio;
        }
    }

    // Spot Light
    else if (2 == g_Light2DBuffer[iLightIdx].iLightType)
    {
        float3 _vDir = _vWorldPos - g_Light2DBuffer[iLightIdx].vWorldPos.xyz;

        float fDistance = distance(_vWorldPos.xy, g_Light2DBuffer[iLightIdx].vWorldPos.xy);

        float fRatio = clamp(exp(2 * (dot(g_Light2DBuffer[iLightIdx].vWorldDir, _vDir / fDistance) - cos(g_Light2DBuffer[iLightIdx].fAngle * 0.5))), 0, 10);
        _LightColor.vDiff += g_Light2DBuffer[iLightIdx].color.vDiff * fRatio;

    }
}

void CalcLight3D(float3 _vViewPos, float3 _vViewNormal, int _iLightIdx, inout tLightColor _lightcolor)
{
    // 광원의 방향을 월드 좌표계에서 뷰 좌표계로 변환
    tLightInfo lightinfo = g_Light3DBuffer[_iLightIdx];    

    float4x4 InvView = inverse(g_matView);

    float3 vLightViewPos;
    float3 vWorldPos = mul(float4(_vViewPos, 1), InvView);

    float3 ViewLightDir = (float3)0.f;
    float  fDiffPow = (float)0.f;
    float fSpecPow = (float)0.f;
    //Directional Light
    if (0 == lightinfo.iLightType)
    {
        ViewLightDir = normalize(mul(float4(lightinfo.vWorldDir.xyz, 0.f), g_matView));

        fDiffPow = saturate(dot(-ViewLightDir, _vViewNormal));
        fSpecPow = 0.f;

        // 반사광 세기를 구함
        // 표면의 빛의 반사벡터
        float3 vViewReflect = normalize(ViewLightDir + 2.f * dot(-ViewLightDir, _vViewNormal) * _vViewNormal);

        // 시점에서 표면을 향하는 벡터
        float3 vEye = normalize(_vViewPos);

        // 시선 벡터와 반사벡터를 내적해서 반사광의 세기를 구함
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);
    }
    ///Point Light
    else if (1 == lightinfo.iLightType)
    {
        vLightViewPos = mul(float4(lightinfo.vWorldPos.xyz, 1.f), g_matView);
        
        ViewLightDir = normalize(_vViewPos - vLightViewPos);

        fDiffPow = saturate(dot(-ViewLightDir, _vViewNormal));

        float fDistance = abs(vWorldPos.y - lightinfo.vWorldPos.y);

        float fDiff = fDistance / lightinfo.fRadius;

        if (0 == fDiff)
            fDiff = 1.f;

        fDiffPow /= fDiff;
        
        float3 vViewReflect = normalize(ViewLightDir + 2.f * dot(-ViewLightDir, _vViewNormal) * _vViewNormal);

        // 시점에서 표면을 향하는 벡터
        float3 vEye = normalize(_vViewPos);

        // 시선 벡터와 반사벡터를 내적해서 반사광의 세기를 구함
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);
    }    
    // Point Light
    else if (2 == lightinfo.iLightType)
    {
        // ViewSpace 상에서 광원의 위치를 알아낸다.
        float3 vLightViewPos = mul(float4(lightinfo.vWorldPos.xyz, 1.f), g_matView);

        // 광원의 위치에서 표면을 향하는 벡터를 구한다.
        ViewLightDir = _vViewPos - vLightViewPos;

        // 광원에서 표면까지의 거리를 구한다.
        float fDist = length(ViewLightDir);

        // 광원에서 표면을 향하는 단위벡터를 구한다.        
        ViewLightDir = normalize(ViewLightDir);

        // 반경대비 거리에 따른 빛의 세기 비율
        //float fRatio = saturate(1.f - (fDist / lightinfo.fRadius));        
        float fRatio = cos(saturate(fDist / lightinfo.fRadius) * 3.1415926535f * 0.5f);

        // View 스페이스 상에서 표면의 빛의 세기를 구함        
        fDiffPow = saturate(dot(-ViewLightDir, _vViewNormal)) * fRatio;

        // 반사광 세기를 구함
        // 표면의 빛의 반사벡터       
        float3 vViewReflect = normalize(ViewLightDir + 2.f * dot(-ViewLightDir, _vViewNormal) * _vViewNormal);

        // 시점에서 표면을 향하는 벡터
        float3 vEye = normalize(_vViewPos);

        // 시선 벡터와 반사벡터를 내적해서 반사광의 세기를 구함
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);
    }
    //Spot Light
    else if (3 == lightinfo.iLightType)
    {
        float3 vWorldDir = normalize(vWorldPos);
        float3 vLightDir = normalize(lightinfo.vWorldPos);

        float fDot = dot(vWorldDir.xyz , vLightDir.xyz);
        float fAngle = acos(fDot);

        if (vWorldPos.y > lightinfo.vWorldPos.y)
            fAngle = PI - fAngle;

        float fDistance = length(lightinfo.vWorldPos.xyz - vWorldPos.xyz);

        if (fDistance > lightinfo.fRadius)
        {
            fDiffPow = 0.f;

            fSpecPow = 0.f;

            lightinfo.color.vAmb = 0.f;
        }
        else
        {
            fDiffPow = lightinfo.fRadius * 0.4 / fDistance;
        }
    }
    else if (4 == lightinfo.iLightType)
    {
        float3 _vDir = vWorldPos - lightinfo.vWorldPos.xyz;

        float fDistance = distance(vWorldPos.xyz, lightinfo.vWorldPos.xyz);

        if (fDistance > lightinfo.fRadius)
        {
            fDiffPow = 0.f;

            fSpecPow = 0.f;

            lightinfo.color.vAmb = 0.f;
        }
        else
        {
            float fRatio = clamp(exp(2 * (dot(lightinfo.vWorldDir, _vDir / fDistance) - cos(lightinfo.fAngle * 0.5))), 0, 100);
            fDiffPow = fRatio / pow(fDistance, 0.1);
        }

        // View 스페이스 상에서 표면의 빛의 세기를 구함        
    }

    // 최종 난반사광
    _lightcolor.vDiff += lightinfo.color.vDiff * fDiffPow;
    // 최종 반사광
    _lightcolor.vSpec += lightinfo.color.vSpec * fSpecPow;
    // 환경광
    _lightcolor.vAmb += lightinfo.color.vAmb;
}

static float GaussianFilter[5][5] =
{
    0.003f  , 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f , 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f , 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f , 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f  , 0.0133f, 0.0219f, 0.0133f, 0.003f,
};

float4 GaussianSample(float2 _vUV)
{
    float4 vOutColor = (float4) 0.f;

    if (1.f < _vUV.x)
    {
        _vUV.x = frac(_vUV.x);
    }
    else if (_vUV.x < 0.f)
    {
        _vUV.x = 1 + frac(_vUV.x);
    }

    if (1.f < _vUV.y)
    {
        _vUV.y = frac(_vUV.y);
    }
    else if (_vUV.y < 0.f)
    {
        _vUV.y = 1 + frac(_vUV.y);
    }

    int2 iUV = _vUV * g_vNoiseResolution;
    iUV += int2(-2, -2);
    
    for (int j = 0; j < 5; ++j)
    {
        for (int i = 0; i < 5; ++i)
        {
            int2 idx = int2(iUV.y + i, iUV.x + j);
            vOutColor += g_Noise[idx] * GaussianFilter[j][i];
        }
    }

    return vOutColor;
}

uint encode(float4 _value)
{
    uint rgba = (uint(_value.x * 255.f) << 24) + (uint(_value.y * 255.f) << 16) + (uint(_value.z * 255.f) << 8) + uint(_value.w * 255.f);
    return rgba;
}

float4 decode(float _value)
{
    uint rgba = asint(_value);

    float r = (float) (rgba >> 24) / 255.f;
    float g = (float) ((rgba & 0x00ff0000) >> 16) / 255.f;
    float b = (float) ((rgba & 0x0000ff00) >> 8) / 255.f;
    float a = (float) (rgba & 0x000000ff) / 255.f;

    return float4(r, g, b, a);
}

float GetTessFactor(float3 _vPos, int _iMinLevel, int _iMaxLevel, float _MinDistance, float _MaxDistance)
{
    float fDistance = length(_vPos);

    if (_MaxDistance < fDistance)
        return _iMinLevel;
    if (fDistance < _MinDistance)
        return _iMaxLevel;

    float fLevel = _iMaxLevel - (_iMaxLevel - _iMinLevel) * ((fDistance - _MinDistance) / (_MaxDistance - _MinDistance));

    return fLevel;
}

#endif


