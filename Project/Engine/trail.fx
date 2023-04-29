#ifndef _STD3D_DEFERRED
#define _STD3D_DEFERRED

#include "register.fx"
#include "struct.fx"
#include "func.fx"

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vUV : TEXCOORD;

	float3 vTangent : TANGENT;
	float3 vNormal : NORMAL;
	float3 vBinormal : BINORMAL;

	float4 vWeights : BLENDWEIGHT;
	float4 vIndices : BLENDINDICES;
};
//
struct VTX_IN_INST
{
	float3 vPos : POSITION;
	float2 vUV : TEXCOORD;

	float3 vTangent : TANGENT;
	float3 vNormal : NORMAL;
	float3 vBinormal : BINORMAL;

	float4 vWeights : BLENDWEIGHT;
	float4 vIndices : BLENDINDICES;

	// Per Instance Data
	row_major matrix matWorld : WORLD;
	row_major matrix matWV : WV;
	row_major matrix matWVP : WVP;
	uint             iRowIndex : ROWINDEX;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
	float2 vUV : TEXCOORD;

	float3 vViewPos : POSITION;

	float3 vViewTangent : TANGENT;
	float3 vViewNormal : NORMAL;
	float3 vViewBinormal : BINORMAL;
	uint   iRowIndex : ROWINDEX;
};

VS_OUT VS_Trail(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;

	if (g_iAnim)
	{
		Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal, _in.vWeights, _in.vIndices, 0);
	}

	output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
	output.vUV = _in.vUV;

	output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
	output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
	output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
	output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);
	return output;
}

VS_OUT VS_Trail_Inst(VTX_IN_INST _in)
{
	VS_OUT output = (VS_OUT)0.f;

	if (g_iAnim)
	{
		Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal, _in.vWeights, _in.vIndices, _in.iRowIndex);
	}

	output.vPosition = mul(float4(_in.vPos, 1.f), _in.matWVP);
	output.vUV = _in.vUV;

	output.vViewPos = mul(float4(_in.vPos, 1.f), _in.matWV).xyz;
	output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), _in.matWV).xyz);
	output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), _in.matWV).xyz);
	output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), _in.matWV).xyz);
	output.iRowIndex = _in.iRowIndex;
	return output;
}

struct PS_OUT
{
	float4 vColor		: SV_Target0;
};


PS_OUT PS_Trail(VS_OUT _in) : SV_Target
{
	PS_OUT output = (PS_OUT)0.f;

	float4 vObjColor = float4(0.f, 5.f, 5.f, 1.f);
	float4 vDecalColor = float4(1.f, 0.f, 1.f, 1.f);

	float3 vNormal = _in.vViewNormal;

	float3 vViewLook = normalize(_in.vViewPos);
	float fCos = dot(vNormal.xyz, vViewLook.xyz);

	if (-1 == fCos)
		discard;

	vObjColor.a = (1 - abs(fCos)) * g_trailTime[_in.iRowIndex];

	output.vColor = vObjColor; //* g_vDiff;

	return output;
}

#endif