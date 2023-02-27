#include "pch.h"
#include "CFrustum.h"

#include "CCamera.h"

CFrustum::CFrustum(CCamera* _Owner)
	: m_Owner(_Owner)
{
	// 투영공간 좌표
	//     4 ------ 5
	//     |        |  Far
	//   / |        |
	//	/  7 ------ 6	
	// /      /
	// 0 -- 1     /
	// |    |    / Near
	// 3 -- 2
	m_ProjPos[0] = Vec3(-1.f, 1.f, 0.f);
	m_ProjPos[1] = Vec3(1.f, 1.f, 0.f);
	m_ProjPos[2] = Vec3(1.f, -1.f, 0.f);
	m_ProjPos[3] = Vec3(-1.f, -1.f, 0.f);

	m_ProjPos[4] = Vec3(-1.f, 1.f, 1.f);
	m_ProjPos[5] = Vec3(1.f, 1.f, 1.f);
	m_ProjPos[6] = Vec3(1.f, -1.f, 1.f);
	m_ProjPos[7] = Vec3(-1.f, -1.f, 1.f);
}

CFrustum::~CFrustum()
{
}

void CFrustum::finaltick()
{
	// 카메라 (투영행렬 역) * (뷰행렬 역)
	Matrix matInv = m_Owner->GetProjMatInv() * m_Owner->GetViewMatInv();

	for (int i = 0; i < 8; ++i)
	{
		m_WorldPos[i] = XMVector3TransformCoord(m_ProjPos[i], matInv);
	}

	// 월드공간 좌표 vWorldPos
	//     4 ------ 5
	//     |        |  Far
	//   / |        |
	//	/  7 ------ 6	
	// /      /
	// 0 -- 1     /
	// |    |    / Near
	// 3 -- 2

	// 월드 정점을 이용해서 평면 6개 구성
	m_Plane[(UINT)FACE_TYPE::FT_NEAR] = XMPlaneFromPoints(m_WorldPos[0], m_WorldPos[1], m_WorldPos[2]);
	m_Plane[(UINT)FACE_TYPE::FT_FAR] = XMPlaneFromPoints(m_WorldPos[7], m_WorldPos[6], m_WorldPos[5]);

	m_Plane[(UINT)FACE_TYPE::FT_UP] = XMPlaneFromPoints(m_WorldPos[4], m_WorldPos[5], m_WorldPos[1]);
	m_Plane[(UINT)FACE_TYPE::FT_DOWN] = XMPlaneFromPoints(m_WorldPos[3], m_WorldPos[2], m_WorldPos[6]);

	m_Plane[(UINT)FACE_TYPE::FT_LEFT] = XMPlaneFromPoints(m_WorldPos[4], m_WorldPos[0], m_WorldPos[3]);
	m_Plane[(UINT)FACE_TYPE::FT_RIGHT] = XMPlaneFromPoints(m_WorldPos[1], m_WorldPos[5], m_WorldPos[6]);
}

bool CFrustum::CheckFrustum(Vec3 _vPos)
{
	for (UINT i = 0; i < (UINT)FACE_TYPE::END; ++i)
	{
		double fDot = _vPos.Dot(m_Plane[i]);
		if (fDot + m_Plane[i].w > 0)
		{
			return false;
		}
	}

	return true;
}

bool CFrustum::CheckFrustumRadius(Vec3 _vPos, float _fRadius)
{
	for (UINT i = 0; i < (UINT)FACE_TYPE::END; ++i)
	{
		float fDot = _vPos.Dot(m_Plane[i]);
		if (fDot + m_Plane[i].w > _fRadius)
		{
			return false;
		}
	}

	return true;
}
