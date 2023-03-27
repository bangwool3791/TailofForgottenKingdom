#ifndef _HEIGHTMAP
#define _HEIGHTMAP

#include "register.fx"

RWTexture2D<float> HEIGHT_MAP : register(u0); // unordered acess
StructuredBuffer<tRaycastOut> LOCATION : register(t16); // 브러쉬 위치(좌상단 기준)

#define WIDTH       g_int_0
#define HEIGHT      g_int_1

//#define BRUSH_TEX   g_texarr_0
#define BRUSH_TEX   g_tex_0
#define SCALE       g_vec2_0   // 브러쉬 크기
#define BRUSH_IDX   g_int_2    // 브러쉬 인덱스
#define DIR         g_int_3

[numthreads(32, 32, 1)]
void CS_HeightMap(int3 _iThreadID : SV_DispatchThreadID)
{
    //HEIGHT_MAP[_iThreadID.xy] += 10.f;
    //return;

    if (WIDTH <= _iThreadID.x || HEIGHT <= _iThreadID.y || !LOCATION[0].success)
    {
        return;
    }

    /*
    * UV 좌표 -> 텍스쳐 참조 좌표 변환
    */
    int2 vCenterPos = float2(WIDTH, HEIGHT) * LOCATION[0].vUV;
    /*
    * UV좌표 스케일 0.1
    * 전체 텍스쳐 크기 * 0.1 
    */
    int2 vScale = float2(WIDTH, HEIGHT) * SCALE;

    /*
    * CenterPos x 값 0 - 1900 사이 값
    *           y 값 0 - 800 사이 값
    */
    /*
    * 스레드 ID가 음수인가 ? 라는 경우도 생각
    * 하지만 스레드가 음수인 경우 미 발생
    * 아래 코드는 중점으로 부터 스케일 보다 떨어진 스레드 ID는 무시한다.
    */
    if (_iThreadID.x < vCenterPos.x - (vScale.x / 2) || vCenterPos.x + (vScale.x / 2) < _iThreadID.x
        || _iThreadID.y < vCenterPos.y - (vScale.y / 2) || vCenterPos.y + (vScale.y / 2) < _iThreadID.y)
    {
        return;
    }

    // brush texture 에서 샘플링 할 UV 계산
    // 좌상단을 구한다.
    int2 vLTPos = vCenterPos - (vScale / 2);
    //피킹 지점으로부터 브러쉬 크기만큼 스레드가 들어온다.

    //vCenterPos - (vScale / 2) -> 0.0
    float2 vUV = float2(_iThreadID.xy - vLTPos) / float2(vScale);

    // 브러쉬로 부터 알파값 샘플링
    //float4 vBrushColor = BRUSH_TEX.SampleLevel(g_sam_0, float3(vUV, BRUSH_IDX), 0);
    float4 vBrushColor = BRUSH_TEX.SampleLevel(g_sam_0, vUV, 0);
    HEIGHT_MAP[_iThreadID.xy] += g_fDT * vBrushColor.a * 0.2f * DIR; // 브러쉬 알파값으로 높이 설정

    // cos 그래프로 높이 설정
    //float vDist = (distance(vCenterPos, _iThreadID.xy) / vScale) * 3.1415926535f;        
    //HEIGHT_MAP[_iThreadID.xy] += saturate(fDT * cos(vDist) * 0.2f);    
}


#endif