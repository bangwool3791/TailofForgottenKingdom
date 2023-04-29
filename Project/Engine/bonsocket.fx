#ifndef _STD3D_BONESOCKET
#define _STD3D_BONESOCKET

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
};

struct VTX_IN_INST
{
	float3 vPos : POSITION;
	float2 vUV : TEXCOORD;

	float3 vTangent : TANGENT;
	float3 vNormal : NORMAL;
	float3 vBinormal : BINORMAL;

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

/*
* 	VS_OUT output = (VS_OUT)0.f;
	
	row_major matrix matBone = g_matBone;
	row_major matrix matWorld = g_matWorld;
	row_major matrix matView = g_matWorld;
	row_major matrix matProj = g_matProj;

	matBone = transpose(matBone);
	matWorld = transpose(matWorld);
	matView = transpose(matView);
	matProj = transpose(matProj);

	//mat = mul(mat, g_matBone);
	matBone = mul(matWorld, matBone);
	matBone = mul(matView, matBone);

	output.vViewPos = mul(matBone, float4(_in.vPos, 1.f)).xyz;
	output.vViewTangent = normalize(mul(matBone, float4(_in.vTangent, 0.f)).xyz);
	output.vViewNormal = normalize(mul(matBone, float4(_in.vNormal, 0.f)).xyz);
	output.vViewBinormal = normalize(mul(matBone, float4(_in.vBinormal, 0.f)).xyz);

	matBone = mul(matProj, matBone);
	
	output.vPosition = mul(matBone, float4(_in.vPos, 1.f));

	return output;
*/

/*
* 	VS_OUT output = (VS_OUT)0.f;
	
	row_major matrix mat = g_matWorld;
	row_major matrix calmat = 0.f;
	//mat = mul(mat, g_matWorld);

	float3 vRight = normalize(mul(float4(1.f, 0.f, 0.f, 0.f), g_matBone).xyz);

	float3 vUp = normalize(mul(float4(0.f, 1.f, 0.f, 0.f), g_matBone).xyz);

	float3 vLook = normalize(mul(float4(0.f, 0.f, 1.f, 0.f), g_matBone).xyz);
	
	calmat[0][0] = vRight.x;
	calmat[0][1] = vUp.x;
	calmat[0][2] = vLook.x;
	calmat[0][3] = 0.f;

	calmat[1][0] = vRight.y;
	calmat[1][1] = vUp.y;
	calmat[1][2] = vLook.y;
	calmat[1][3] = 0.f;

	calmat[2][0] = vRight.z;
	calmat[2][1] = vUp.z;
	calmat[2][2] = vLook.z;
	calmat[2][3] = 0.f;

	calmat[3][0] = 0.f;
	calmat[3][1] = 0.f;
	calmat[3][2] = 0.f;
	calmat[3][3] = 1.f;

	mat[3][0] = 0.f;
	mat[3][1] = 0.f;
	mat[3][2] = 0.f;
	mat[3][3] = 1.f;

	mat = mul(mat, calmat);
	mat[3][0] += g_matBone[3][0];
	mat[3][1] += g_matBone[3][1];
	mat[3][2] += g_matBone[3][2];

	mat = mul(mat, g_matView);
	
	output.vViewPos = mul(float4(_in.vPos, 1.f), mat).xyz;
	output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), mat).xyz);
	output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), mat).xyz);
	output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), mat).xyz);

	mat = mul(mat, g_matProj);
	
	output.vPosition = mul(float4(_in.vPos, 1.f), mat);
*/
VS_OUT VS_Std3D_BoneSocket(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;

	//row_major matrix mat = g_matWorld;
	//row_major matrix calmat = 0.f;
	////mat = mul(mat, g_matWorld);

	//float3 vRight = normalize(mul(float4(1.f, 0.f, 0.f, 0.f), g_matBone).xyz);

	//float3 vUp = normalize(mul(float4(0.f, 1.f, 0.f, 0.f), g_matBone).xyz);

	//float3 vLook = normalize(mul(float4(0.f, 0.f, 1.f, 0.f), g_matBone).xyz);

	//calmat[0][0] = vRight.x;
	//calmat[0][1] = vUp.x;
	//calmat[0][2] = vLook.x;
	//calmat[0][3] = 0.f;

	//calmat[1][0] = vRight.y;
	//calmat[1][1] = vUp.y;
	//calmat[1][2] = vLook.y;
	//calmat[1][3] = 0.f;

	//calmat[2][0] = vRight.z;
	//calmat[2][1] = vUp.z;
	//calmat[2][2] = vLook.z;
	//calmat[2][3] = 0.f;

	//calmat[3][0] = 0.f;
	//calmat[3][1] = 0.f;
	//calmat[3][2] = 0.f;
	//calmat[3][3] = 1.f;

	//mat[3][0] = 0.f;
	//mat[3][1] = 0.f;
	//mat[3][2] = 0.f;
	//mat[3][3] = 1.f;

	//mat = mul(mat, calmat);
	//mat[3][0] += g_matBone[3][0];
	//mat[3][1] += g_matBone[3][1];
	//mat[3][2] += g_matBone[3][2];

	//mat = mul(mat, g_matView);

	//output.vViewPos = mul(float4(_in.vPos, 1.f), mat).xyz;
	//output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), mat).xyz);
	//output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), mat).xyz);
	//output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), mat).xyz);

	//mat = mul(mat, g_matProj);

	//output.vPosition = mul(float4(_in.vPos, 1.f), mat);
	
	return output;
}

VS_OUT VS_Std3D_BoneSocket_Inst(VTX_IN_INST _in)
{
	VS_OUT output = (VS_OUT)0.f;
	//row_major matrix mat = g_matBone;

	//mat = mul(mat, g_matWorld);
	//mat = mul(mat, g_matView);

	//output.vViewPos = mul(float4(_in.vPos, 1.f), mat).xyz;
	//output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), mat).xyz);
	//output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), mat).xyz);
	//output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), mat).xyz);

	//mat = mul(mat, g_matProj);

	//output.vPosition = mul(float4(_in.vPos, 1.f), mat);

	return output;
}

struct PS_OUT
{
	float4 vColor		: SV_Target0;
	float4 vNormal		: SV_Target1;
	float4 vPosition	: SV_Target2;
	float4 vData		: SV_Target3;
};


PS_OUT PS_Std3D_BoneSocket(VS_OUT _in) : SV_Target
{
	PS_OUT output = (PS_OUT)0.f;

	float4 vObjColor = float4(1.f, 0.f, 1.f, 1.f);
	float4 vDecalColor = float4(1.f, 0.f, 1.f, 1.f);

	output.vColor = vObjColor; //* g_vDiff;
	output.vNormal = float4(_in.vViewNormal, 1.f);
	output.vPosition = float4(_in.vViewPos, 1.f);

	return output;
}

#endif