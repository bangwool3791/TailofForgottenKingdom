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
    // ������ ������ ���� ��ǥ�迡�� �� ��ǥ��� ��ȯ
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

        // �ݻ籤 ���⸦ ����
        // ǥ���� ���� �ݻ纤��
        float3 vViewReflect = normalize(ViewLightDir + 2.f * dot(-ViewLightDir, _vViewNormal) * _vViewNormal);

        // �������� ǥ���� ���ϴ� ����
        float3 vEye = normalize(_vViewPos);

        // �ü� ���Ϳ� �ݻ纤�͸� �����ؼ� �ݻ籤�� ���⸦ ����
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

        // �������� ǥ���� ���ϴ� ����
        float3 vEye = normalize(_vViewPos);

        // �ü� ���Ϳ� �ݻ纤�͸� �����ؼ� �ݻ籤�� ���⸦ ����
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);
    }    
    // Point Light
    else if (2 == lightinfo.iLightType)
    {
        // ViewSpace �󿡼� ������ ��ġ�� �˾Ƴ���.
        float3 vLightViewPos = mul(float4(lightinfo.vWorldPos.xyz, 1.f), g_matView);

        // ������ ��ġ���� ǥ���� ���ϴ� ���͸� ���Ѵ�.
        ViewLightDir = _vViewPos - vLightViewPos;

        // �������� ǥ������� �Ÿ��� ���Ѵ�.
        float fDist = length(ViewLightDir);

        // �������� ǥ���� ���ϴ� �������͸� ���Ѵ�.        
        ViewLightDir = normalize(ViewLightDir);

        // �ݰ��� �Ÿ��� ���� ���� ���� ����
        //float fRatio = saturate(1.f - (fDist / lightinfo.fRadius));        
        float fRatio = cos(saturate(fDist / lightinfo.fRadius) * 3.1415926535f * 0.5f);

        // View �����̽� �󿡼� ǥ���� ���� ���⸦ ����        
        fDiffPow = saturate(dot(-ViewLightDir, _vViewNormal)) * fRatio;

        // �ݻ籤 ���⸦ ����
        // ǥ���� ���� �ݻ纤��       
        float3 vViewReflect = normalize(ViewLightDir + 2.f * dot(-ViewLightDir, _vViewNormal) * _vViewNormal);

        // �������� ǥ���� ���ϴ� ����
        float3 vEye = normalize(_vViewPos);

        // �ü� ���Ϳ� �ݻ纤�͸� �����ؼ� �ݻ籤�� ���⸦ ����
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

        // View �����̽� �󿡼� ǥ���� ���� ���⸦ ����        
    }

    // ���� ���ݻ籤
    _lightcolor.vDiff += lightinfo.color.vDiff * fDiffPow;
    // ���� �ݻ籤
    _lightcolor.vSpec += lightinfo.color.vSpec * fSpecPow;
    // ȯ�汤
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

// 0 ~ 1 ���� ���� ��ȯ
float Random(int key)
{
    float fValue = 0.f;

    return fValue;
}

bool IsBinding(in Texture2D _tex)
{
    uint width, height;
    _tex.GetDimensions(width, height);

    if (width == 0 || height == 0)
        return false;

    return true;
}

float encode(float4 _value)
{
    uint rgba = (uint(_value.x * 255.f) << 24) + (uint(_value.y * 255.f) << 16) + (uint(_value.z * 255.f) << 8) + uint(_value.w * 255.f);
    return asfloat(rgba);
}

float4 decode(float _value)
{
    uint rgba = asint(_value);

    float r = float((rgba & 0xff000000) >> 24) / 255.f;
    float g = float((rgba & 0x00ff0000) >> 16) / 255.f;
    float b = float((rgba & 0x0000ff00) >> 8) / 255.f;
    float a = float((rgba & 0x000000ff) >> 0) / 255.f;

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

float GetCameraTessFactor(float3 vViewPos, float3 _vPos, int _iMinLevel, int _iMaxLevel, float _MinDistance, float _MaxDistance)
{
    float fDistance = length(_vPos - vViewPos);

    if (_MaxDistance < fDistance)
        return _iMinLevel;
    if (fDistance < _MinDistance)
        return _iMaxLevel;

    float fLevel = _iMaxLevel - (_iMaxLevel - _iMinLevel) * ((fDistance - _MinDistance) / (_MaxDistance - _MinDistance));

    return fLevel;
}

matrix GetBoneMat(int _iBoneIdx, int _iRowIdx)
{
   return g_arrBoneMat[(g_iBoneCount * _iRowIdx) + _iBoneIdx];
}

void Skinning(inout float3 _vPos, inout float3 _vTangent, inout float3 _vBinormal, inout float3 _vNormal, inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo)0.f;

    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int)_vIndices[i], _iRowIdx);
        
        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
        info.vTangent += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
        info.vBinormal += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
        info.vNormal += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
    }

    _vPos = info.vPos;
    _vTangent = normalize(info.vTangent);
    _vBinormal = normalize(info.vBinormal);
    _vNormal = normalize(info.vNormal);
}

void Skinning(inout float3 _vPos, inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo)0.f;

    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int)_vIndices[i], _iRowIdx);
        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
    }
    _vPos = info.vPos;
}

int IntersectsLay(float3 _vertices[3], float3 _vStart, float3 _vDir, out float3 _vCrossPoint, out float _fResult)
{
    float3 edge[2] = { (float3) 0.f, (float3) 0.f };
    /*
    * �ﰢ���� �� �� ����
    */
    edge[0] = _vertices[1].xyz - _vertices[0].xyz;
    edge[1] = _vertices[2].xyz - _vertices[0].xyz;

    /*
    * �ﰢ�� ���� ���� ����
    * 
    */
    float3 normal = normalize(cross(edge[0], edge[1]));
    /*
    * ���� -> �������� �Ÿ� ���ϴ� ���
    * 
    * ������ ���� ���� ����
    * ���� ���� +  b -> 0
    * ���� ���� -  b -> 1
    */
    float b = dot(normal, _vDir);

    /*
    * �ﰢ������ �ٶ󺸴� ���� ���� ����(���� ����)
    */
    float3 w0 = _vStart - _vertices[0].xyz;
    /*
    * �ﰢ�� ������ ���������� ������Ų��.
    */
    float a = -dot(normal, w0);
    float t = a / b;
    //�� ���Ѵ�.
    _fResult = t;

    float3 p = _vStart + t * _vDir;

    _vCrossPoint = p;

    float uu, uv, vv, wu, wv, inverseD;
    uu = dot(edge[0], edge[0]);
    uv = dot(edge[0], edge[1]);
    vv = dot(edge[1], edge[1]);

    float3 w = p - _vertices[0].xyz;
    wu = dot(w, edge[0]);
    wv = dot(w, edge[1]);
    inverseD = uv * uv - uu * vv;
    inverseD = 1.0f / inverseD;

    float u = (uv * wv - vv * wu) * inverseD;
    if (u < 0.0f || u > 1.0f)
    {
        return 0;
    }

    float v = (uv * wu - uu * wv) * inverseD;
    if (v < 0.0f || (u + v) > 1.0f)
    {
        return 0;
    }

    return 1;
}

float3 cubicSpline(float t, float3 p0, float3 p1, float3 p2, float3 p3)
{
    float3 a = p3 - 3.0 * p2 + 3.0 * p1 - p0;
    float3 b = 3.0 * p2 - 6.0 * p1 + 3.0 * p0;
    float3 c = 3.0 * p1 - 3.0 * p0;
    float3 d = p0;

    float3 interpolatedPoint = a * t * t * t + b * t * t + c * t + d;

    return interpolatedPoint;
}
#endif


