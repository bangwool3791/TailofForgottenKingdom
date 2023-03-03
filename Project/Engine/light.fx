#ifndef _LIGHT
#define _LIGHT

#include "register.fx"
#include "struct.fx"
#include "func.fx"

// =================
// Directional Light
// MRT  : MRT_TYPE::LIGHT
// mesh : RectMesh

// g_int_0 : Light Index
// g_tex_0 : Position Target Tex
// g_tex_1 : Normal Target Tex
// g_tex_2 : Data Target Tex
// =================

struct VS_IN
{
	float3 vPos : POSITION;
};

/*
* ���̴��� ���� �Է��� ���� SV_Position ����� ��� �� ���� ���� ��� �� �ϳ��� ������ �� �ֽ��ϴ�. 
* linearNoPerspective �Ǵ� linearNoPerspectiveCentroid�� ���� ���� ��Ƽ�ٸ���� �� �߽� ���� xyzw ���� �����մϴ�. 
* ���̴����� ���Ǵ� ��� SV_Position �ȼ� ��ġ�� �����մϴ�. ��� ���̴����� 0.5 ���������� �ȼ� �߽��� ������ �� �ֽ��ϴ�.
*/
struct VS_OUT
{
	float4 vPosition : SV_Position;
};

VS_OUT VS_DirLightShader(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;

	//Rect Mesh ���� ��ǥ�� ��ȯ(�ȼ�)
	output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);

	return output;
}

//Directional Light ��ü �ؽ��� ���ε� 
struct PS_OUT
{
	//Taget Texture Binding from MRT 
	float4 vDiffuse : SV_Target;
	float4 vSpecular : SV_Target1;
};

/*
* RenderMgr_init.cpp
* Position Tex, Normal Tex, Spec Tex
*/
PS_OUT PS_DirLightShader(VS_OUT _in)
{
	PS_OUT output = (PS_OUT)0.f;

	float2 vUV = _in.vPosition.xy / g_vRenderResolution;
;
	float4 vViewPos = g_tex_0.Sample(g_sam_0, vUV);

	if (0.f == vViewPos.a)
		discard;

	float4 vViewNormal = g_tex_1.Sample(g_sam_0, vUV);

	tLightColor lightcolor = (tLightColor)0.f;
	CalcLight3D(vViewPos.xyz, vViewNormal.xyz, g_int_0, lightcolor);

	float SpecCoef = g_tex_2.Sample(g_sam_0, vUV).x;

	output.vDiffuse = lightcolor.vDiff + lightcolor.vAmb;
	//�� �ݻ籤 * ���� �ݻ� ���
	output.vSpecular = lightcolor.vSpec * SpecCoef;

	output.vDiffuse.a = 1.f;
	output.vSpecular.a = 1.f;

	return output;
}

// =================
// Point Light
// MRT  : MRT_TYPE::LIGHT
// mesh : SphereMesh

// g_int_0 : Light Index
// g_tex_0 : Position Target Tex
// g_tex_1 : Normal Target Tex
// g_tex_2 : Data Target Tex
// =================

VS_OUT VS_PointLightShader(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;

	output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);

	return output;
}

PS_OUT PS_PointLightShader(VS_OUT _in)
{
    PS_OUT output = (PS_OUT)0.f;

    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    float4 vViewPos = g_tex_0.Sample(g_sam_0, vUV);

    // �������� ���� == ���� ���� ��ü�� ����
    if (0.f == vViewPos.a)
    {
        discard;
    }

    // ���� ������ ���� PositionTarget �� ��ġ���� ������ ���÷� �̵���Ų��.
    // ���ÿ������� �����޽�(Sphere) ���ο��ٸ�, ������ �������� �ȿ� �ִ� ��ǥ���ٴ� ��
    float4 vLocalPos = mul(mul(vViewPos, g_matViewInv), g_matWorldInv);
    if (length(vLocalPos.xyz) > 0.5f)
    {
        discard;
    }

    float4 vViewNormal = g_tex_1.Sample(g_sam_0, vUV);

    tLightColor lightcolor = (tLightColor)0.f;
    CalcLight3D(vViewPos.xyz, vViewNormal.xyz, g_int_0, lightcolor);

    float SpecCoef = g_tex_2.Sample(g_sam_0, vUV).x;

    output.vDiffuse = lightcolor.vDiff + lightcolor.vAmb;
    output.vSpecular = lightcolor.vSpec * SpecCoef;

    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;

    return output;
}

// =================
// Spot Light
// MRT  : MRT_TYPE::LIGHT
// mesh : ConeMesh

// g_int_0 : Light Index
// g_tex_0 : Position Target Tex
// g_tex_1 : Normal Target Tex
// g_tex_2 : Data Target Tex
// =================

VS_OUT VS_SpotLightShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT)0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);

    return output;
}

PS_OUT PS_SpotLightShader(VS_OUT _in)
{
    PS_OUT output = (PS_OUT)0.f;

    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    float4 vViewPos = g_tex_0.Sample(g_sam_0, vUV);

    // �������� ���� == ���� ���� ��ü�� ����
    if (0.f == vViewPos.a)
    {
        discard;
    }

    float4 vLocalPos = mul(mul(vViewPos, g_matViewInv), g_matWorldInv);

    if (0.f < vLocalPos.y)
        discard;

    float3 p = vLocalPos;
    float3 x = float3(0.f, 0.f, 0.f);
    float3 dir = float3(0.f, -1.f, 0.f);
    float h = 1.f;
    float r = 0.5f;
    float cone_dist = dot(vLocalPos - x, dir);
    float cone_radius = (cone_dist / h) * r;
    float orth_distance = length((p - x) - cone_dist * dir);
    bool is_point_inside_cone = (orth_distance < cone_radius);

    if (false == is_point_inside_cone)
    {
        discard;
    }

    float4 vViewNormal = g_tex_1.Sample(g_sam_0, vUV);

    tLightColor lightcolor = (tLightColor)0.f;

    CalcLight3D(vViewPos.xyz, vViewNormal.xyz, g_int_0, lightcolor);

    float SpecCoef = g_tex_2.Sample(g_sam_0, vUV).x;

    output.vDiffuse = lightcolor.vDiff + lightcolor.vAmb;
    output.vSpecular = lightcolor.vSpec * SpecCoef;

    //output.vDiffuse.a = 1.f;
    //output.vSpecular.a = 1.f;

    return output;
}
#endif