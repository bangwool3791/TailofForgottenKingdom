
#include "register.fx"

struct VS_OUTPUT_CUBE_MAP
{
	float4 Pos : SV_POSITION;
};

struct GS_OUTPUT_CUBE_MAP
{
	float4 Pos : SV_POSITION;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

VS_OUTPUT_CUBE_MAP VS_CubeMap(float4 Pos: POSITION)
{
	VS_OUTPUT_CUBE_MAP result = (VS_OUTPUT_CUBE_MAP)0.f;

	result.Pos = mul(Pos, g_matWorld);

	return result;
}

/*
* ť��� ���� GS
*/
[maxvertexcount(18)]
void GS_CubeMap
	(triangle VS_OUTPUT_CUBE_MAP In[3], 
		inout TriangleStream<GS_OUTPUT_CUBE_MAP> CubeMapStream)
{
	//6���� ����Ѵ�.
	for (int surface = 0; surface < 6; ++surface)
	{
		GS_OUTPUT_CUBE_MAP result;
		result.RTIndex = surface;

		for (int vertex = 0; vertex < 3; ++vertex)
		{
			result.Pos = mul(In[vertex].Pos, g_matEnvView[surface]);
			//���� ��ǥ�� ��ȯ
			result.Pos = mul(result.Pos, g_matEvnProj);
			CubeMapStream.Append(result);
		}

		CubeMapStream.RestartStrip();
	}
}

float4 PS_CubeMap(GS_OUTPUT_CUBE_MAP _in) : SV_Target
{
	float4 vOutColor = (float4)0.f;

	float2 vUV = _in.Pos.xy / g_vRenderResolution;

	float4 vViewPos = g_tex_1.Sample(g_sam_0, vUV);

	//��ġ ������ ���� �ȼ� discard
	if (1.f != vViewPos.a)
		discard;

	//���� �ؽ��� ����
	vOutColor = g_tex_0.Sample(g_sam_0, vUV);
	vOutColor += g_tex_4.Sample(g_sam_0, vUV);

	float4 vDiffuse = g_tex_2.Sample(g_sam_0, vUV);
	float4 vSpecular = g_tex_3.Sample(g_sam_0, vUV);
	vOutColor.rgb = (vOutColor.rgb * vDiffuse.rgb) + vSpecular.rgb;
	return vOutColor;
}