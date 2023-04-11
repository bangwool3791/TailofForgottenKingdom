#ifndef _COPYBONE
#define _COPYBONE

#include "register.fx"

//u0 레지스터 뼈 행렬 누적
//std_defferdShader t61 레지스터로 바인딩
RWStructuredBuffer<matrix> g_arrDestMat : register(u0); // 복사받을 목적지 버퍼
StructuredBuffer<matrix> g_srcMat : register(t16); // 원본 버퍼

// ======================
// BoneMatrix 복사 쉐이더
#define BoneCount   g_int_0
#define RowIndex    g_int_1
// ======================


[numthreads(1024, 1, 1)]
void CS_CopyBoneMatrix(int3 _iThreadIdx : SV_DispatchThreadID)
{
    if (_iThreadIdx.x >= g_int_0)
        return;

    //한줄당 뼈 구조화 버퍼 한개씩
    g_arrDestMat[BoneCount * RowIndex + _iThreadIdx.x] = g_srcMat[_iThreadIdx.x];
}

#endif