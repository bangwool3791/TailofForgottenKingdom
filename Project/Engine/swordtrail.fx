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
	float2 vUV			: TEXCOORD;
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
	float2 vUV			: TEXCOORD;
};

VS_OUT VS_3DPostProcess_Inst(VTX_IN_INST _in)
{
	VS_OUT output = (VS_OUT)0.f;

	output.vPosition = mul(float4(_in.vPos, 1.f), _in.matWVP);
	output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
	output.vUV = _in.vUV;
	return output;
}

[maxvertexcount(170)]
void GS_3DPostProcess(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
	GS_OUT output[14] = { (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f,(GS_OUT)0.f,
	(GS_OUT)0.f,(GS_OUT)0.f,(GS_OUT)0.f,(GS_OUT)0.f,(GS_OUT)0.f,(GS_OUT)0.f,(GS_OUT)0.f };

	float3 VTX[14];
	float3 vZero = float3(0.f, 0.f, 0.f);

	float iSequence = g_int_0 - 6;
	iSequence *= 0.5f;

	//if (iSequence < 0)
	//{
	//	_OutStream.Append(output[0]);
	//	_OutStream.Append(output[1]);
	//	_OutStream.Append(output[8]);
	//}

	for (int i = iSequence -1; i > 0; --i)
	{
		VTX[0] = g_SwordPosBuffer[i * 2];
		VTX[1] = cubicSpline(0.25, g_SwordPosBuffer[i * 2], g_SwordPosBuffer[i * 2 + 2], g_SwordPosBuffer[i * 2 + 4], g_SwordPosBuffer[i * 2 + 6]);
		VTX[2] = g_SwordPosBuffer[i * 2 + 2];
		VTX[3] = cubicSpline(0.5, g_SwordPosBuffer[i * 2], g_SwordPosBuffer[i * 2 + 2], g_SwordPosBuffer[i * 2 + 4], g_SwordPosBuffer[i * 2 + 6]);
		VTX[4] = g_SwordPosBuffer[i * 2 + 4];
		VTX[5] = cubicSpline(0.75, g_SwordPosBuffer[i * 2], g_SwordPosBuffer[i * 2 + 2], g_SwordPosBuffer[i * 2 + 4], g_SwordPosBuffer[i * 2 + 6]);
		VTX[6] = g_SwordPosBuffer[i * 2 + 6];
		VTX[7] = g_SwordPosBuffer[i * 2 + 1];
		VTX[8] = cubicSpline(0.25, g_SwordPosBuffer[i * 2 + 1], g_SwordPosBuffer[i * 2 + 3], g_SwordPosBuffer[i * 2 + 5], g_SwordPosBuffer[i * 2 + 7]);
		VTX[9] = g_SwordPosBuffer[i * 2 + 3];
		VTX[10] = cubicSpline(0.5, g_SwordPosBuffer[i * 2 + 1], g_SwordPosBuffer[i * 2 + 3], g_SwordPosBuffer[i * 2 + 5], g_SwordPosBuffer[i * 2 + 7]);
		VTX[11] = g_SwordPosBuffer[i * 2 + 5];
		VTX[12] = cubicSpline(0.75, g_SwordPosBuffer[i * 2 + 1], g_SwordPosBuffer[i * 2 + 3], g_SwordPosBuffer[i * 2 + 5], g_SwordPosBuffer[i * 2 + 7]);
		VTX[13] = g_SwordPosBuffer[i * 2 + 7];

		//if (VTX[0].x == vZero.x && VTX[0].y == vZero.y && VTX[0].z == vZero.z)
		//	break;
		//if (VTX[2].x == vZero.x && VTX[2].y == vZero.y && VTX[2].z == vZero.z)
		//	break;
		//if (VTX[4].x == vZero.x && VTX[4].y == vZero.y && VTX[4].z == vZero.z)
		//	break;
		//if (VTX[6].x == vZero.x && VTX[6].y == vZero.y && VTX[6].z == vZero.z)
		//	break;
		//if (VTX[7].x == vZero.x && VTX[7].y == vZero.y && VTX[7].z == vZero.z)
		//	break;
		//if (VTX[9].x == vZero.x && VTX[9].y == vZero.y && VTX[9].z == vZero.z)
		//	break;
		//if (VTX[11].x == vZero.x && VTX[11].y == vZero.y && VTX[11].z == vZero.z)
		//	break;
		//if (VTX[13].x == vZero.x && VTX[13].y == vZero.y && VTX[13].z == vZero.z)
		//	break;

		output[0].vPosition = mul(float4(VTX[0], 1.f), mul(g_matView, g_matProj));
		output[1].vPosition = mul(float4(VTX[1], 1.f), mul(g_matView, g_matProj));
		output[2].vPosition = mul(float4(VTX[2], 1.f), mul(g_matView, g_matProj));
		output[3].vPosition = mul(float4(VTX[3], 1.f), mul(g_matView, g_matProj));
		output[4].vPosition = mul(float4(VTX[4], 1.f), mul(g_matView, g_matProj));
		output[5].vPosition = mul(float4(VTX[5], 1.f), mul(g_matView, g_matProj));
		output[6].vPosition = mul(float4(VTX[6], 1.f), mul(g_matView, g_matProj));
		output[7].vPosition = mul(float4(VTX[7], 1.f), mul(g_matView, g_matProj));
		output[8].vPosition = mul(float4(VTX[8], 1.f), mul(g_matView, g_matProj));
		output[9].vPosition = mul(float4(VTX[9], 1.f), mul(g_matView, g_matProj));
		output[10].vPosition = mul(float4(VTX[10], 1.f), mul(g_matView, g_matProj));
		output[11].vPosition = mul(float4(VTX[11], 1.f), mul(g_matView, g_matProj));
		output[12].vPosition = mul(float4(VTX[12], 1.f), mul(g_matView, g_matProj));
		output[13].vPosition = mul(float4(VTX[13], 1.f), mul(g_matView, g_matProj));

		output[0].vUV = float2(0.f, 0.f);
		output[1].vUV = float2((1.f + (i * 6)) / (6.f * iSequence), 0.f);
		output[8].vUV = float2((1.f + (i * 6)) / (6.f * iSequence), 1.f);

		_OutStream.Append(output[0]);
		_OutStream.Append(output[1]);
		_OutStream.Append(output[8]);
		_OutStream.RestartStrip();


		output[0].vUV = float2(0.f, 0.f);
		output[8].vUV = float2((1.f + (i * 6)) / (6.f * iSequence), 1.f);
		output[7].vUV = float2(0.f, 1.f);

		_OutStream.Append(output[0]);
		_OutStream.Append(output[8]);
		_OutStream.Append(output[7]);
		_OutStream.RestartStrip();

		output[1].vUV = float2(0.f, 0.f);
		output[2].vUV = float2((2.f + (i * 6)) / (6.f * iSequence), 0.f);
		output[9].vUV = float2((2.f + (i * 6)) / (6.f * iSequence), 1.f);

		_OutStream.Append(output[1]);
		_OutStream.Append(output[2]);
		_OutStream.Append(output[9]);
		_OutStream.RestartStrip();


		output[1].vUV = float2(0.f, 0.f);
		output[9].vUV = float2((2.f + (i * 6)) / (6.f * iSequence), 1.f);
		output[8].vUV = float2(0.f, 1.f);

		_OutStream.Append(output[1]);
		_OutStream.Append(output[9]);
		_OutStream.Append(output[8]);
		_OutStream.RestartStrip();

		output[2].vUV = float2(0.f, 0.f);
		output[3].vUV = float2((3.f + (i * 6)) / (6.f * iSequence), 0.f);
		output[10].vUV = float2((3.f + (i * 6)) / (6.f * iSequence), 1.f);

		_OutStream.Append(output[2]);
		_OutStream.Append(output[3]);
		_OutStream.Append(output[10]);
		_OutStream.RestartStrip();


		output[2].vUV = float2(0.f, 0.f);
		output[10].vUV = float2((3.f + (i * 6)) / (6.f * iSequence), 1.f);
		output[9].vUV = float2(0.f, 1.f);

		_OutStream.Append(output[2]);
		_OutStream.Append(output[10]);
		_OutStream.Append(output[9]);
		_OutStream.RestartStrip();

		output[3].vUV = float2(0.f, 0.f);
		output[4].vUV = float2((4.f + (i * 6)) / (6.f * iSequence), 0.f);
		output[11].vUV = float2((4.f + (i * 6)) / (6.f * iSequence), 1.f);

		_OutStream.Append(output[3]);
		_OutStream.Append(output[4]);
		_OutStream.Append(output[11]);
		_OutStream.RestartStrip();


		output[3].vUV = float2(0.f, 0.f);
		output[11].vUV = float2((4.f + (i * 6)) / (6.f * iSequence), 1.f);
		output[10].vUV = float2(0.f, 1.f);

		_OutStream.Append(output[3]);
		_OutStream.Append(output[9]);
		_OutStream.Append(output[8]);
		_OutStream.RestartStrip();

		output[4].vUV = float2(0.f, 0.f);
		output[5].vUV = float2((5.f + (i * 6)) / (6.f * iSequence), 0.f);
		output[12].vUV = float2((5.f + (i * 6)) / (6.f * iSequence), 1.f);

		_OutStream.Append(output[4]);
		_OutStream.Append(output[5]);
		_OutStream.Append(output[12]);
		_OutStream.RestartStrip();


		output[4].vUV = float2(0.f, 0.f);
		output[12].vUV = float2((5.f + (i * 6)) / (6.f * iSequence), 1.f);
		output[11].vUV = float2(0.f, 1.f);

		_OutStream.Append(output[4]);
		_OutStream.Append(output[12]);
		_OutStream.Append(output[11]);
		_OutStream.RestartStrip();

		output[5].vUV = float2(0.f, 0.f);
		output[6].vUV = float2((6.f + (i * 6)) / (6.f * iSequence), 0.f);
		output[13].vUV = float2((6.f + (i * 6)) / (6.f * iSequence), 1.f);

		_OutStream.Append(output[5]);
		_OutStream.Append(output[6]);
		_OutStream.Append(output[13]);
		_OutStream.RestartStrip();


		output[5].vUV = float2(0.f, 0.f);
		output[13].vUV = float2((6.f + (i * 6)) / (6.f * iSequence), 1.f);
		output[12].vUV = float2(0.f, 1.f);

		_OutStream.Append(output[5]);
		_OutStream.Append(output[13]);
		_OutStream.Append(output[12]);
		_OutStream.RestartStrip();
	}
}

float4 PS_3DPostProcess(GS_OUT _in) : SV_Target
{
	float4 vTexColor = g_tex_0.Sample(g_sam_0, _in.vUV);
	float4 vTexAlpha = g_tex_1.Sample(g_sam_0, _in.vUV);

	if (g_btex_1)
	{
		vTexColor.r = vTexColor.r * vTexAlpha.r;
		vTexColor.g = vTexColor.g * vTexAlpha.g;
		vTexColor.b = vTexColor.b * vTexAlpha.b;
	}
//
	vTexColor.a = _in.vUV.x * 0.8f;
////if (vTexColor.r < 0.6f)
////	discard;
////if (vTexColor.g < 0.6f)
////	discard;
////if (vTexColor.b < 0.6f)
////	discard;
//
//if (vTexColor.x == 0)
//	discard;
//if (vTexColor.y == 0)
//	discard;
//if (vTexColor.z == 0)
//	discard;
////vTexColor.a = 0.1f;
	return vTexColor;
}

#endif

