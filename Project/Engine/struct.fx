#ifndef _STRUCT
#define _STRUCT

//LightInfo

struct tLightColor
{
    float4 vDiff; // 난반사 광 
    //해당 광원을 통해서 만들어 낼 수 있는 최대 빛의 크기
    float4 vSpec;
    float4 vAmb; // 환경광
};

struct tLightInfo
{
	tLightColor color;
	float4		vWorldPos; //광원 월드 위치
	float4		vWorldDir; //광원 월드 방향
	int			iLightType;//광원 타입
	float		fRadius; //광원 반경 (POINT, SPOT)
	float		fAngle;
	int			pad;
};

/*
* Particle System Texture 버퍼 사용 구조체
*/
struct tParticle
{
	float4 vRelativePos;
	float4 vDir;

	float fMaxTime;
	float fCurTime;
	float fSpeed;
	uint  iActive;
}; 

struct tParticleShare
{
    uint    iAliveCount;
    uint3   iPadding;
};

struct tLightShare
{
    dword R;
    dword G;
    dword B;
    dword A;
};

struct tRenderInfo
{
    /*
    * Transform Info
    */
	row_major matrix matWorld;
	row_major matrix matView;
	row_major matrix matProj;

	row_major matrix matWV;
	row_major matrix matWVP;

    /*
    * Material Info
    */
    int int_0;
    int int_1;
    int int_2;
    int int_3;

    float float_0;
    float float_1;
    float float_2;
    float float_3;

    float2 vec2_0;
    float2 vec2_1;
    float2 vec2_2;
    float2 vec2_3;

    float4 vec4_0;
    float4 vec4_1;
    float4 vec4_2;
    float4 vec4_3;

    row_major matrix mat_0;
    row_major matrix mat_1;
    row_major matrix mat_2;
    row_major matrix mat_3;

    int arr[40];
    /*
    * Animation Info
    */
    float2  vLeftTop;
    float2  vSlice;
    float2  vOffset;
    float2  vFullSize;

    int     iAnim2DUse;
    int3    iPadding;
};

struct tTile
{
    float3      vPos;
    int         iIndex;
    int         iInfo;
};
#endif