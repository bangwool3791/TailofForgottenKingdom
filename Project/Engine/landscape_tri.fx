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
    float3 vUV : TEXCOORD;

    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
};

struct VS_OUT
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};


VS_OUT VS_LandScape(VS_IN _in)
{
    VS_OUT output = (VS_OUT)0.f;

    output.vLocalPos = _in.vPos;
    output.vUV = _in.vUV;

    return output;
}

// Hull Shader
struct PatchTess
{
    float EdgeFactor[3] : SV_TessFactor;
    float InsideFactor : SV_InsideTessFactor;
};


// 패치 상수 함수
PatchTess PatchConstFunc(InputPatch<VS_OUT, 3> _patch, uint _patchId : SV_PrimitiveID)
{
    PatchTess factor = (PatchTess)0.f;

    float3 centerL;
    centerL.x = (_patch[0].vLocalPos.x + _patch[1].vLocalPos.x) * 0.5f;
    centerL.y = _patch[0].vLocalPos.y;
    centerL.z = (_patch[0].vLocalPos.z + _patch[2].vLocalPos.z) * 0.5f;

    float3 centerW = mul(float4(centerL, 1.0f), g_matWorld);

    float3 camW = g_vec4_0.xyz;

    float d = distance(centerW, camW);

    const float d0 = 20.f;
    const float d1 = 1000.f;

    float tess = 32.0f * saturate((d1 - d) / (d1 - d0));

    factor.EdgeFactor[0] = g_int_0 * tess;
    factor.EdgeFactor[1] = g_int_0 * tess;
    factor.EdgeFactor[2] = g_int_0 * tess;
    factor.InsideFactor = g_int_0 * tess;

    return factor;
}


struct HS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};


[domain("tri")]
[outputcontrolpoints(3)]
[partitioning("integer")] //partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[patchconstantfunc("PatchConstFunc")]
[maxtessfactor(32)]
HS_OUT HS_LandScape(InputPatch<VS_OUT, 3> _patch
    , uint _patchId : SV_PrimitiveID
    , uint _vtxId : SV_OutputControlPointID)
{
    HS_OUT output = (HS_OUT)0.f;

    output.vPos = _patch[_vtxId].vLocalPos;
    output.vUV = _patch[_vtxId].vUV;

    return output;
}



// Domain Shader
struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;

    float3 vViewPos : POSITION;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
    float3 vViewNormal : NORMAL;
};

[domain("tri")]
DS_OUT DS_LandScape(PatchTess _tessFactor
    , const OutputPatch<HS_OUT, 3> _patch
    , float3 _Ratio : SV_DomainLocation)
{
    DS_OUT output = (DS_OUT)0.f;

    float3 vLocalPos = (_patch[0].vPos * _Ratio[0]) + (_patch[1].vPos * _Ratio[1]) + (_patch[2].vPos * _Ratio[2]);
    float2 vUV = (_patch[0].vUV * _Ratio[0]) + (_patch[1].vUV * _Ratio[1]) + (_patch[2].vUV * _Ratio[2]);

    float2 FullUV = vUV / float2(FACE_X, FACE_Z);

    vLocalPos.y = HeightMap.SampleLevel(g_sam_0, FullUV, 0).x;

    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;

    output.vViewPos = mul(float4(vLocalPos, 1.f), g_matWV);

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