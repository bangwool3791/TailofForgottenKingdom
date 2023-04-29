#ifndef _POST_PROCESS
#define _POST_PROCESS

#include "register.fx"
#include "func.fx"

// ====================
// PostProcess Shader
// mesh : RectMesh
// BS_TYPE : Default
// DS_TYPE : NoTest NoWrite 
// ====================
struct VS_IN
{
	float3 vPos : POSITION;
	float3 vUV : TEXCOORD;
};

struct VTX_IN_INST
{
	float3 vPos : POSITION;
	float2 vUV : TEXCOORD;

	// Per Instance Data
	row_major matrix matWorld : WORLD;
	row_major matrix matWV : WV;
	row_major matrix matWVP : WVP;
	uint             iRowIndex : ROWINDEX;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
	float3 vWorldPos : POSITION;
	float2 vUV : TEXCOORD;
};


struct GS_OUT
{
	float4 vPosition : SV_Position;
	float3 vViewPos : POSITION;
	float2 vUV : TEXCOORD;
};

VS_OUT VS_3DPostProcess(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;

	output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
	output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
	output.vUV = _in.vUV;

	return output;
}

struct PS_OUT
{
	float4 vColor		: SV_Target0;
};

VS_OUT VS_3DPostProcess_Inst(VTX_IN_INST _in)
{
	VS_OUT output = (VS_OUT)0.f;

	output.vPosition = mul(float4(_in.vPos, 1.f), _in.matWVP);
	output.vUV = _in.vUV;
	return output;
}

[maxvertexcount(100)]
void GS_3DPostProcess(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
	GS_OUT output[4] = { (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f };

	float3 VTX[4] = { float3(g_SwordPosBuffer[0]), float3(g_SwordPosBuffer[1]) ,float3(g_SwordPosBuffer[2]),float3(g_SwordPosBuffer[3]) };

	for (int i = 0; i < g_int_0 / 4; ++i)
	{
		VTX[0] = g_SwordPosBuffer[i];
		VTX[1] = g_SwordPosBuffer[i + 1];
		VTX[2] = g_SwordPosBuffer[i + 2];
		VTX[3] = g_SwordPosBuffer[i + 3];

		output[0].vPosition = mul(float4(VTX[0], 1.f), mul(g_matView, g_matProj));
		output[1].vPosition = mul(float4(VTX[1], 1.f), mul(g_matView, g_matProj));
		output[2].vPosition = mul(float4(VTX[2], 1.f), mul(g_matView, g_matProj));
		output[3].vPosition = mul(float4(VTX[3], 1.f), mul(g_matView, g_matProj));

		output[0].vViewPos = mul(float4(VTX[0], 1.f), g_matView).xyz;
		output[1].vViewPos = mul(float4(VTX[1], 1.f), g_matView).xyz;
		output[2].vViewPos = mul(float4(VTX[2], 1.f), g_matView).xyz;
		output[3].vViewPos = mul(float4(VTX[3], 1.f), g_matView).xyz;

		output[0].vUV = float2(0.f, 0.f);
		output[1].vUV = float2(1.f, 0.f);
		output[3].vUV = float2(1.f, 1.f);

		_OutStream.Append(output[0]);
		_OutStream.Append(output[3]);
		_OutStream.Append(output[1]);
		_OutStream.RestartStrip();

		output[0].vUV = float2(1.f, 1.f);
		output[2].vUV = float2(1.f, 0.f);
		output[3].vUV = float2(0.f, 0.f);

		_OutStream.Append(output[0]);
		_OutStream.Append(output[2]);
		_OutStream.Append(output[3]);
		_OutStream.RestartStrip();
	}
}

float4 PS_3DPostProcess(GS_OUT _in) : SV_Target
{
	float4 vColor = (float4) 0.f;

	// VS_OUT 으로 전달한 SV_Position 값은 PixelShader 에 입력될 때 픽셀좌표로 변환해서 입력
	float2 vUV = _in.vPosition.xy / g_vRenderResolution;

	//vUV.y += cos((_in.vUV.x - g_fAccTime * 0.5f) * 10.f * 3.141592f) * 0.01f;
	float2 fAdd = float2(g_Noise.Sample(g_sam_0, _in.vUV + g_fAccTime * 0.2f).x
						 , g_Noise.Sample(g_sam_0, _in.vUV + float2(0.1f, 0.f) + g_fAccTime * 0.2f).x);
	fAdd -= fAdd / 2.f;
	vUV += fAdd * 0.05f;

	vColor = g_RTCopyTex.Sample(g_sam_0, vUV);
	vColor.z *= 3.f;
	//float4 vTexColor = g_tex_0.Sample(g_sam_0, _in.vUV);
	//vTexColor.w = 0.3f;
	//vColor += vTexColor;
	//vColor.y *= 10.f;

	return vColor;
}

#endif

