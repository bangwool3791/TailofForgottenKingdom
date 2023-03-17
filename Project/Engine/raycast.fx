#ifndef _RAYCAST
#define _RAYCAST

#include "register.fx"
#include "func.fx"

RWStructuredBuffer<tRaycastOut> OUTPUT : register(u0);

#define HEIGHT_MAP      g_tex_0

#define CAM_POS         g_vec4_0
#define CAM_DIR         g_vec4_1

#define FACE_X          g_int_0
#define FACE_Z          g_int_1

[numthreads(32, 32, 1)]
void CS_Raycast(int3 _iThreadID : SV_DispatchThreadID)
{
    int2 id = _iThreadID.xy;

    /*
    * 스레드 그룹 x 지형 facecountx * 2
    * 스레드 그룹 y 지형 facecounty / 2 
    * facecount는 항상 동일해야한다.
    */
    if (FACE_X * 2 <= id.x || FACE_Z <= id.y)
    {
        return;
    }

    float3 vPos[3] = { (float3) 0.f, (float3) 0.f, (float3) 0.f };

    /*
    * 그룹 id 비례 좌표 생성
    * 아랫쪽 삼각형 윗쪽 삼각형 계산법 분기
    */
    if (0 == id.x % 2)
    {
        // 아래쪽 삼각형        
        // 2
        // | \
        // 0--1        
        vPos[0].x = id.x / 2;
        vPos[0].z = id.y;

        vPos[1].x = vPos[0].x + 1;
        vPos[1].z = id.y;

        vPos[2].x = vPos[0].x;
        vPos[2].z = id.y + 1;
    }
    else
    {
        // 윗쪽 삼각형
        // 1--0
        //  \ |
        //    2  
        vPos[0].x = (id.x / 2) + 1;
        vPos[0].z = id.y + 1;

        vPos[1].x = vPos[0].x - 1;
        vPos[1].z = id.y + 1;

        vPos[2].x = vPos[0].x;
        vPos[2].z = id.y;
    }
    
    /*
    * uv좌표 계산
    * 높이맵에서 좌표 읽는다.
    */
    for (int i = 0; i < 3; ++i)
    {
        float2 uv = float2(saturate(vPos[i].x / (float)FACE_X), saturate(1.f - vPos[i].z / (float)FACE_Z));
        vPos[i].y = HEIGHT_MAP.SampleLevel(g_sam_0, uv, 0).x;
    }
    //높이 값을 가져와서.

    float3 vCrossPoint = (float3) 0.f;
    float fDist = 0.f;

    if (IntersectsLay(vPos, CAM_POS.xyz, CAM_DIR.xyz, vCrossPoint, fDist))
    {
        int iDist = (int)(10000.f * fDist);
        int iDistOut = 0;

        /*
        * void InterlockedMin(in  R dest, in  T value, out T original_value);
        */

        InterlockedMin(OUTPUT[0].iDist, iDist, iDistOut);

        if (iDistOut < iDist)
        {
            // 실패
            return;
        }

        /*
        * 계산 Ray가 더 작다면 
        * 충돌 위치 구조화 버퍼 전달
        */
        OUTPUT[0].vUV = float2(saturate(vCrossPoint.x / (float)FACE_X), saturate(1.f - vCrossPoint.z / (float)FACE_Z));
        OUTPUT[0].success = 1;
    }
}





#endif