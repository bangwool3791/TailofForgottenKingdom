#ifndef _STD3D_DEFERRED
#define _STD3D_DEFERRED

#include "register.fx"
#include "struct.fx"
#include "func.fx"

/*
* g_float_1 : fogStart
* g_float_2 : fogRange
*/
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
};

VS_OUT VS_Std3D_Deferred(VS_IN _in)
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

VS_OUT VS_Std3D_Deferred_Inst(VTX_IN_INST _in)
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

	return output;
}

struct PS_OUT
{
	float4 vColor		: SV_Target0;
	float4 vNormal		: SV_Target1;
	float4 vPosition	: SV_Target2;
	float4 vData		: SV_Target3;
};


PS_OUT PS_Std3D_Deferred(VS_OUT _in) : SV_Target
{
    PS_OUT output = (PS_OUT)0.f;

	float4 vObjColor = float4(1.f, 0.f, 1.f, 1.f);
	float4 vDecalColor = float4(1.f, 0.f, 1.f, 1.f);

	if (g_btex_0)
	{
		float2 derivX = ddx(_in.vUV); // 인접픽셀과 x축 편미분값을 구한다
		float2 derivY = ddy(_in.vUV); // 인접픽셀과 y축 편미분값을 구한다

		vObjColor = g_tex_0.SampleGrad(g_sam_0, _in.vUV, derivX, derivY);

		//vObjColor = g_tex_0.Sample(g_sam_0, _in.vUV);
	}
	clip(vObjColor.a - 0.1f);

	float3 vNormal = _in.vViewNormal;

	if (g_btex_1)
	{
		float2 derivX = ddx(_in.vUV); // 인접픽셀과 x축 편미분값을 구한다
		float2 derivY = ddy(_in.vUV); // 인접픽셀과 y축 편미분값을 구한다

		vNormal = g_tex_1.SampleGrad(g_sam_0, _in.vUV, derivX, derivY);

		//vNormal = g_tex_1.Sample(g_sam_0, _in.vUV);
		/*
		* Normal map -1 ~ 1
		*/
		vNormal = (vNormal * 2.f) - 1.f;

		float3x3 matTBN =
		{
			_in.vViewTangent,
			_in.vViewBinormal,
			_in.vViewNormal,
		};

		vNormal = normalize(mul(vNormal, matTBN));
	}

	float3 toEye = _in.vViewPos;
	float distToEye = length(toEye);

	toEye /= distToEye;

	if (g_int_0)
	{
		float fogLerp = saturate((distToEye - g_float_1) / g_float_2);
		vObjColor = lerp(vObjColor, g_vec4_0, fogLerp);
	}

	output.vColor = vObjColor; //* g_vDiff;
	output.vNormal = float4(vNormal, 1.f);
	output.vPosition = float4(_in.vViewPos, 1.f);

	float4 vSpecCoeff = float4(g_float_0, g_float_0, g_float_0, 1.f);

	// Spec 맵이 있으면
	if (g_btex_2)
	{
		vSpecCoeff *= g_tex_2.Sample(g_sam_0, _in.vUV);
	}
	else
	{
		vSpecCoeff *= g_vSpec;
	}

	output.vData.x = encode(vSpecCoeff);

	return output;
}

#endif