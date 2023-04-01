#ifndef _REGISTER
#define _REGISTER

#include "struct.fx"
// 상수 레지스터
cbuffer TRANSFORM : register(b0)
{
    row_major matrix g_matWorld;
    row_major matrix g_matWorldInv;
    row_major matrix g_matView;
    row_major matrix g_matViewInv;
    row_major matrix g_matProj;
    row_major matrix g_matProjInv;

    row_major matrix g_matWV;
    row_major matrix g_matWVP;

    row_major matrix g_matReflection;
};


cbuffer MATERIAL : register(b1)
{
    float4 g_vDiff;
    float4 g_vSpec;
    float4 g_vAmb;
    float4 g_vEmv;

    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;

    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;

    float2 g_vec2_0;
    float2 g_vec2_1;
    float2 g_vec2_2;
    float2 g_vec2_3;

    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;

    row_major matrix g_mat_0;
    row_major matrix g_mat_1;
    row_major matrix g_mat_2;
    row_major matrix g_mat_3;

    int g_btex_0;
    int g_btex_1;
    int g_btex_2;
    int g_btex_3;
    int g_btex_4;
    int g_btex_5;
    int g_btex_6;
    int g_btex_7;
    int g_btex_8;
    int g_btex_9;
    int g_btex_10;
    int g_btex_11;
    int g_btex_12;
    int g_btex_13;
    int g_btex_14;
    int g_btex_15;
    int g_btex_16;
    int g_btex_17;
    int g_btex_18;
    int g_btex_19;
    int g_btex_20;
    int g_btex_21;
    int g_btex_22;
    int g_btex_23;
    int g_btex_24;
    int g_btex_25;
    int g_btex_26;
    int g_btex_27;
    int g_btex_28;
    int g_btex_29;
    int g_btex_30;
    int g_btex_31;
    int g_btex_32;

    int g_btexcube_0;
    int g_btexcube_1;

    int g_btexarr_0;
    int g_btexarr_1;
    int g_padding_0;
    int g_padding_1;
    int g_padding_2;
};

cbuffer GLOBAL : register(b2)
{
    float2  g_vRenderResolution;
    float2  g_vNoiseResolution;
    int     g_iLight2DCount;
    int     g_iLight3DCount;

    float   g_fAccTime;
    float   g_fDT;
}

cbuffer ANIM2DINFO : register(b3)
{
    float2  g_vLeftTop;
    float2  g_vSlice;
    float2  g_vOffset;
    float2  g_vFullSize;

    int     g_iAnim2DUse;
    int3    iPadding;
}

// 텍스쳐 레지스터
Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);
Texture2D g_tex_6 : register(t6);
Texture2D g_tex_7 : register(t7);
Texture2D g_tex_8 : register(t8);
Texture2D g_tex_9 : register(t9);
Texture2D g_tex_10 : register(t10);
Texture2D g_tex_11 : register(t11);
Texture2D g_tex_12 : register(t12);
Texture2D g_tex_13 : register(t13);
Texture2D g_tex_14 : register(t14);
Texture2D g_tex_15 : register(t15);
Texture2D g_tex_16 : register(t16);
Texture2D g_tex_17 : register(t17);
Texture2D g_tex_18 : register(t18);
Texture2D g_tex_19 : register(t19);
Texture2D g_tex_20 : register(t20);
Texture2D g_tex_21 : register(t21);
Texture2D g_tex_22 : register(t22);
Texture2D g_tex_23 : register(t23);
Texture2D g_tex_24 : register(t24);
Texture2D g_tex_25 : register(t25);
Texture2D g_tex_26 : register(t26);
Texture2D g_tex_27 : register(t27);
Texture2D g_tex_28 : register(t28);
Texture2D g_tex_29 : register(t29);
Texture2D g_tex_30 : register(t30);
Texture2D g_tex_31 : register(t31);
Texture2D g_tex_32 : register(t32);
TextureCube g_texcube_0 : register(t33);
TextureCube g_texcube_1 : register(t34);
Texture2DArray g_texarr_0 : register(t35);
Texture2DArray g_texarr_1 : register(t36);

//Light Buffer
StructuredBuffer<tLightInfo> g_Light2DBuffer : register(t55);
StructuredBuffer<tLightInfo> g_Light3DBuffer : register(t56);


StructuredBuffer<tTile>       g_TileBuffer     : register(t56);

/*
* Instancing object
*/
StructuredBuffer<tRenderInfo> g_RenderInfo   : register(t57);
// Noise Texture
Texture2D g_Noise : register(t58);

// Animation2D Atlas 
Texture2D g_Atals : register(t59);
// RenderTarget CopyTex
Texture2D g_RTCopyTex : register(t60);

#define b g_RenderInfo
// 샘플러 레지스터
SamplerState g_sam_0 : register(s0); // anisotropic sampler
SamplerState g_sam_1 : register(s1); // point sampler
SamplerState g_sam_2 : register(s2); // linear sampler
SamplerComparisonState g_sam_3 : register(s3); // shadow
static const float SMAP_SIZE = 4096.f * 2.f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;
#endif

