#ifndef _LANDSCAPE
#define _LANDSCAPE

#include "register.fx"

// ==========================
// TessShader
// Domain : DOMAIN_DEFERRED
// ==========================
#define HeightMap g_tex_0
#define FACE_X  16
#define FACE_Z  16

struct VS_IN
{
    float3 vPos : POSITION;
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
};

struct VS_OUT
{
    float3 vLocalPos : POSITION;
};


VS_OUT VS_LandScape(VS_IN _in)
{
    VS_OUT output = (VS_OUT)0.f;

    output.vLocalPos = _in.vPos;

    return output;
}

// Hull Shader
struct PatchTess
{
    float EdgeFactor[4] : SV_TessFactor;
    float InsideFactor[2] : SV_InsideTessFactor;
};


// 패치 상수 함수
PatchTess PatchConstFunc(InputPatch<VS_OUT, 4> _patch, uint _patchId : SV_PrimitiveID)
{
    PatchTess factor = (PatchTess)0.f;

    float3 centerL = 0.25f * (_patch[0].vLocalPos + _patch[1].vLocalPos + _patch[2].vLocalPos + _patch[3].vLocalPos);

    float3 centerW = mul(float4(centerL, 1.0f), g_matWorld);

    float3 camW = g_vec4_0.xyz;

    float d = distance(centerW, camW);

    const float d0 = 20.f;
    const float d1 = 3000.f;

    float tess = 64.0f * saturate((d1 - d) / (d1 - d0));

    //factor.EdgeFactor[0] = g_int_0 * tess;
    //factor.EdgeFactor[1] = g_int_0 * tess;
    //factor.EdgeFactor[2] = g_int_0 * tess;
    //factor.EdgeFactor[3] = g_int_0 * tess;
    //factor.InsideFactor[0] = g_int_0 * tess;
    //factor.InsideFactor[1] = g_int_0 * tess;

    factor.EdgeFactor[0] = g_int_0 * tess;
    factor.EdgeFactor[1] = g_int_0 * tess;
    factor.EdgeFactor[2] = g_int_0 * tess;
    factor.EdgeFactor[3] = g_int_0 * tess;
    factor.InsideFactor[0] = g_int_0 * tess;
    factor.InsideFactor[1] = g_int_0 * tess;

    return factor;
}


struct HS_OUT
{
    float3 vPos : POSITION;
};


[domain("quad")]
[outputcontrolpoints(4)]
[partitioning("integer")] //partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[patchconstantfunc("PatchConstFunc")]
[maxtessfactor(64)]
HS_OUT HS_LandScape(InputPatch<VS_OUT, 4> _patch
    , uint _patchId : SV_PrimitiveID
    , uint _vtxId : SV_OutputControlPointID)
{
    HS_OUT output = (HS_OUT)0.f;

    output.vPos = _patch[_vtxId].vLocalPos;

    return output;
}



// Domain Shader
struct DS_OUT
{
    float4 vPosition : SV_Position;

    float3 vViewPos : POSITION;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
    float3 vViewNormal : NORMAL;
};

[domain("quad")]
DS_OUT DS_LandScape(PatchTess _tessFactor
    , const OutputPatch<HS_OUT, 4> _patch
    , float2 _Ratio : SV_DomainLocation)
{
    DS_OUT output = (DS_OUT)0.f;

    float3 v1 = lerp(_patch[0].vPos, _patch[1].vPos, 1 - _Ratio.y);
    float3 v2 = lerp(_patch[3].vPos, _patch[2].vPos, 1 - _Ratio.y);
    float3 p = lerp(v1, v2, _Ratio.x);

    //p.y = 0.3f * (p.z * sin(p.x) + p.x * cos(p.z));
    p.y = HeightMap.SampleLevel(g_sam_0, _Ratio, 0).x;

    output.vPosition = mul(float4(p, 1.f), g_matWVP);

    output.vViewPos = mul(float4(p, 1.f), g_matWV);

    float3 vTangent = float3(1.f, 0.f, 0.f);
    float3 vNormal = float3(0.f, 1.f, 0.f);
    float3 vBinormal = float3(0.f, 0.f, -1.f);

    output.vViewTangent = normalize(mul(float4(vTangent, 0.f), g_matWV));
    output.vViewBinormal = normalize(mul(float4(vBinormal, 0.f), g_matWV));
    output.vViewNormal = normalize(mul(float4(vNormal, 0.f), g_matWV));

    return output;
};




struct PS_OUT
{
    float4 vColor : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
    float4 vData : SV_Target3;
};

PS_OUT PS_LandScape(DS_OUT _in)
{
    PS_OUT output = (PS_OUT)0.f;

    output.vColor = float4(1.f, 0.f, 1.f, 1.f);
    output.vNormal = float4(_in.vViewNormal, 1.f);
    output.vPosition = float4(_in.vViewPos, 1.f);
    output.vData.x = g_float_0;


    return output;
}

#endif