#ifndef _TESS
#define _TESS

#include "register.fx"
// ===============
// TessShader
// Domain : DOMAIN_OPAQUE
// ===============

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};


/*
* 제어점을 입력으로 받는다.
* 정점 셰이더는 테셀레이션을 시작하기 전에 제어점들을 필요에 따라 변경하는 역할
*/
VS_OUT VS_Tess(VS_IN _in)
{
    VS_OUT output = (VS_OUT)0.f;

    output.vPos = _in.vPos;
    output.vUV = _in.vUV;

    return output;
}

// Hull Shader
struct PatchTess
{
    //제어점 3개
    float EdgeFactor[3] : SV_TessFactor;
    float InsideFactor : SV_InsideTessFactor;
};


// 패치 상수 함수
// InputPatch<VS_OUT, 3> _patch 상수 패치 정의
PatchTess PatchConstFunc(InputPatch<VS_OUT, 3> _patch, uint _patchId : SV_PrimitiveID)
{
    PatchTess factor = (PatchTess)0.f;
    //파라미터 만큼 분할
    factor.EdgeFactor[0] = g_int_0;
    factor.EdgeFactor[1] = g_int_1;
    factor.EdgeFactor[2] = g_int_2;
    factor.InsideFactor = g_int_3;

    return factor;
}


struct HS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

/*
* 다수의 제어점 변경 또는 추가 역할
* 제어점마다 한 번씩 호출
* 용도 : 표면의 표현 변경
*/
[domain("tri")]
[outputcontrolpoints(3)]
[partitioning("integer")] //partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[patchconstantfunc("PatchConstFunc")]
[maxtessfactor(32)]
HS_OUT HS_Tess(InputPatch<VS_OUT, 3> _patch
    , uint _patchId : SV_PrimitiveID
    , uint _vtxId : SV_OutputControlPointID)
{
    HS_OUT output = (HS_OUT)0.f;

    output.vPos = _patch[_vtxId].vPos;
    output.vUV = _patch[_vtxId].vUV;

    return output;
}



// Domain Shader
struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

/*
* 테셀레이션된 패치의 정점들을 동차 절단 공간으로 변환
*/
[domain("tri")]
DS_OUT DS_Tess(PatchTess _tessFactor
    , const OutputPatch<HS_OUT, 3> _patch
    , float3 _Ratio : SV_DomainLocation)
{
    DS_OUT output = (DS_OUT)0.f;

    float3 vLocalPos = (_patch[0].vPos * _Ratio[0]) + (_patch[1].vPos * _Ratio[1]) + (_patch[2].vPos * _Ratio[2]);
    float2 vUV = (_patch[0].vUV * _Ratio[0]) + (_patch[1].vUV * _Ratio[1]) + (_patch[2].vUV * _Ratio[2]);

    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;

    return output;
};


float4 PS_Tess(DS_OUT _in) : SV_Target
{
    float4 vColor = float4(1.f, 0.f, 1.f, 1.f);


    return vColor;
}

#endif