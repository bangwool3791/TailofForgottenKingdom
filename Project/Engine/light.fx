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

#define DepthMap                 g_tex_3
#define LightVP                  g_mat_0

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
    float4 vViewPos = g_tex_0.Sample(g_sam_0, vUV);

    if (0.f == vViewPos.a)
        discard;

    float4 vViewNormal = g_tex_1.Sample(g_sam_0, vUV);

    tLightColor lightcolor = (tLightColor)0.f;
    CalcLight3D(vViewPos.xyz, vViewNormal.xyz, g_int_0, lightcolor);

    float3 vWorldPos = mul(float4(vViewPos.xyz, 1.f), g_matViewInv).xyz;

    float4 vLightProj = mul(float4(vWorldPos, 1.f), LightVP);

    vLightProj.xyz /= vLightProj.w;

    float percentLit = 0.0f;

    float2 vDepthMapUV = float2((vLightProj.x / 2.f) + 0.5f, -(vLightProj.y / 2.f) + 0.5f);

    // Texel size.
    const float dx = SMAP_DX;

    const float2 offsets[9] =
    {
        float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += DepthMap.SampleCmpLevelZero(g_sam_3,
            vDepthMapUV.xy + offsets[i], vLightProj.z).r;
    }
    //�׸��ڿ� ������ ����
    percentLit /= 9.f;

    float SpecCoef = g_tex_2.Sample(g_sam_1, vUV).x;
    float4 vSpec = decode(SpecCoef);

    output.vDiffuse = lightcolor.vDiff * percentLit + lightcolor.vAmb;

    output.vSpecular.xyz = lightcolor.vSpec.xyz * percentLit;

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
        discard;

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

    float SpecCoef = g_tex_2.Sample(g_sam_1, vUV).x;
    float4 vSpec = decode(SpecCoef);

    output.vDiffuse = lightcolor.vDiff * lightcolor.vAmb;
    output.vSpecular.xyz = lightcolor.vSpec.xyz * vSpec.xyz;

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

    float SpecCoef = g_tex_2.Sample(g_sam_1, vUV).x;
    float4 vSpec = decode(SpecCoef);

    output.vDiffuse = lightcolor.vDiff * lightcolor.vAmb;
    output.vSpecular.xyz = lightcolor.vSpec.xyz * vSpec.xyz;

    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;

    return output;
}


// ===============
// DepthMap Shader
// MRT : ShadowMap MRT
// RS : CULL_BACK
// BS : Default
// DS : Less
// ===============
struct VS_DEPTH_IN
{
    float3 vPos : POSITION;
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_DEPTH_OUT
{
    float4 vPosition : SV_Position;
    float4 vProjPos : POSITION;
};

VS_DEPTH_OUT VS_DepthMap(VS_DEPTH_IN _in)
{
    VS_DEPTH_OUT output = (VS_DEPTH_OUT)0.f;

    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vWeights, _in.vIndices, 0);
    }

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vProjPos = output.vPosition;

    return output;
}

float PS_DepthMap(VS_DEPTH_OUT _in) : SV_Target
{
    float fOut = 0.f;
    fOut = _in.vProjPos.z / _in.vProjPos.w;
    return fOut;
}

#endif