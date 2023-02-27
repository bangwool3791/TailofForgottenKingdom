#include "pch.h"
#include "CCollisionMgr.h"

#include "CTransform.h"
#include "CCollider.h"
#include "CCollider2D.h"
#include "CCollider3D.h"
#include "CGameObject.h"
#include "CLevel.h"
#include "CLevelMgr.h"

CCollisionMgr::CCollisionMgr()
{
}

CCollisionMgr::~CCollisionMgr()
{
}

/*
* 행렬의 절반 만큼 검사
* 0, 0 의경우, 
*/
void CCollisionMgr::tick()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (int iRow{ 0 }; iRow < MAX_LAYER; ++iRow)
	{
		for (int iCol{iRow}; iCol < MAX_LAYER; ++iCol)
		{
			/*
			* iRow 1, iCol 1이면 m_matrix[1] = 2
			*/
			if (m_matrix[iRow] & (1 << iCol))
			{
				CollisionBtwLayer(pCurLevel, iRow, iCol);
			}
		}
	}
}

void CCollisionMgr::CollisionBtwLayer(CLevel* _pCurScene, int _iLeft, int _iRight)
{
	const vector<CGameObject*>& vecLeft = _pCurScene->GetLayer(_iLeft)->GetObjects();
	const vector<CGameObject*>& vecRight = _pCurScene->GetLayer(_iRight)->GetObjects();

	for (int i{ 0 }; i < vecLeft.size(); ++i)
	{
		if (nullptr == vecLeft[i]->Collider3D())
			continue;

		int j{ 0 };
		
		if (_iLeft == _iRight)
			j = i + 1;

		for (; j < vecRight.size(); ++j)
		{
			if (nullptr == vecRight[j]->Collider3D())
				continue;

			if (vecLeft[i]->Collider3D()->IsPause() || vecRight[j]->Collider3D()->IsPause())
				continue;

				CollisionBtwCollider(vecLeft[i]->Collider3D(), vecRight[j]->Collider3D());
		}
	}
}

void CCollisionMgr::CollisionBtwCollider(CCollider3D* _pLeft, CCollider3D* _pRight)
{
	COLLIDER_ID id;
	id.LEFT_ID = _pLeft->GetID();
	id.RIGHT_ID = _pRight->GetID();

	auto iter = m_mapColInfo.find(id.ID);

	if (iter == m_mapColInfo.end())
	{
		m_mapColInfo.insert(make_pair(id.ID, false));
		iter = m_mapColInfo.find(id.ID);
	}

	bool IsDead = _pLeft->GetOwner()->IsDead() || _pRight->GetOwner()->IsDead();

	if (IsCollision(_pLeft, _pRight))
	{
		if (false == iter->second)
		{
			if (!IsDead)
			{
				_pLeft->BeginOverlap(_pRight);
				_pRight->BeginOverlap(_pLeft);
				iter->second = true;
			}
		}
		else
		{
			if (IsDead)
			{
				_pLeft->EndOverlap(_pRight);
				_pRight->EndOverlap(_pLeft);
				iter->second = false;
			}
			else
			{
				_pLeft->Overlap(_pRight);
				_pRight->Overlap(_pLeft);
			}
		}
	}
	else
	{
		if (iter->second)
		{
			_pLeft->EndOverlap(_pRight);
			_pRight->EndOverlap(_pLeft);
			iter->second = false;
		}
	}
}
#include "CMesh.h"
#include "CRenderComponent.h"
bool CCollisionMgr::IsCollision(CCollider3D* _pLeft, CCollider3D* _pRight)
{
	// 0 --- 1
	// |  \  | 
	// 3 --- 2	

//#ifdef true
	//정점을 읽어와서 해결
	Ptr<CMesh> leftMesh = _pLeft->GetOwner()->GetRenderComponent()->GetMesh();
	Ptr<CMesh> rightMesh = _pRight->GetOwner()->GetRenderComponent()->GetMesh();

	size_t size{};

	Vtx* leftVtx = leftMesh->GetVertices(size);
	Vtx* rightVtx = rightMesh->GetVertices(size);

	static const Vec3 arrLocalLeftPos[8]
	{
		leftVtx[20].vPos,
		leftVtx[21].vPos,
		leftVtx[22].vPos,
		leftVtx[23].vPos,

		leftVtx[16].vPos,
		leftVtx[17].vPos,
		leftVtx[18].vPos,
		leftVtx[19].vPos,
	};

	static const Vec3 arrLocalRightPos[8]
	{
		rightVtx[20].vPos,
		rightVtx[21].vPos,
		rightVtx[22].vPos,
		rightVtx[23].vPos,

		rightVtx[16].vPos,
		rightVtx[17].vPos,
		rightVtx[18].vPos,
		rightVtx[19].vPos,
	};

	Vec3 vAxis[6]{};

	const Matrix& matLeft	= _pLeft->GetWorldMat();
	const Matrix& matRight	= _pRight->GetWorldMat();

	/*
	* 마우스 드래그의 경우 월드 행렬 스케일이 없지만, 초기 위치가 플레이어 위치와 동일하여
	* 충돌 판정이라고 코드가 판단 하므로 예외 처리 조건 추가
	*/
	Vec3 vecRight = matLeft.Right();
	Vec3 vecUp = matLeft.Front();
	
	if (!vecRight.Length() && !vecUp.Length())
		return false;

	vecRight = matRight.Right();
	vecUp = matRight.Front();

	if (!vecRight.Length() && !vecUp.Length())
		return false;

	vAxis[0] = XMVector3TransformCoord(arrLocalLeftPos[1], matLeft) - XMVector3TransformCoord(arrLocalLeftPos[0], matLeft);
	vAxis[1] = XMVector3TransformCoord(arrLocalLeftPos[3], matLeft) - XMVector3TransformCoord(arrLocalLeftPos[0], matLeft);
	vAxis[2] = XMVector3TransformCoord(arrLocalLeftPos[4], matLeft) - XMVector3TransformCoord(arrLocalLeftPos[0], matLeft);

	vAxis[3] = XMVector3TransformCoord(arrLocalRightPos[1], matRight) - XMVector3TransformCoord(arrLocalRightPos[0], matRight);
	vAxis[4] = XMVector3TransformCoord(arrLocalRightPos[3], matRight) - XMVector3TransformCoord(arrLocalRightPos[0], matRight);
	vAxis[5] = XMVector3TransformCoord(arrLocalRightPos[4], matRight) - XMVector3TransformCoord(arrLocalRightPos[0], matRight);

	Vec3 vC = _pLeft->GetFinalPos() - _pRight->GetFinalPos() ;
	Vec3 vCenterDir = Vec3{ vC.x, vC.y, vC.z };
	
	float fSum{ 0.f };

	for (int i{ 0 }; i < 6; ++i)
	{
		Vec3 vA = vAxis[i];
		vA.Normalize();

		float fProjDist{ 0.f };

		for (int j{ 0 }; j < 6; ++j)
		{
			fProjDist += fabsf(vAxis[j].Dot(vA)) /2.f;
		}

		if (fProjDist < fabsf(vCenterDir.Dot(vA)))
		{
			return false;
		}
	}
	return true;
}

void CCollisionMgr::CollisionLayerCheck(int _iLeft, int _iRight)
{
	int iRow{ 0 }, iCol{ 0 };

	//iLeft 0, iRight 32
	if (_iLeft <= _iRight)
	{
		iRow = _iLeft;
		iCol = _iRight;
	}
	//iLeft 32, iRight 0
	else
	{
		iRow = _iRight;
		iCol = _iLeft;
	}

	m_matrix[iRow] |= (1 << iCol);
}

void CCollisionMgr::CollisionLayerRelease(int _iLeft, int _iRight)
{
	int iRow{ 0 }, iCol{ 0 };

	//iLeft 0, iRight 32
	if (_iLeft <= _iRight)
	{
		iRow = _iLeft;
		iCol = _iRight;
	}
	//iLeft 32, iRight 0
	else
	{
		iRow = _iRight;
		iCol = _iLeft;
	}

	m_matrix[iRow] &= ~(1 << iCol);
}

void CCollisionMgr::SaveToFile(FILE* _pFIle)
{
	// 프레임 개수, 데이터
	size_t iFrameCount = MAX_LAYER;
	fwrite(&iFrameCount, sizeof(size_t), 1, _pFIle);
	fwrite(m_matrix, sizeof(UINT), iFrameCount, _pFIle);
	
	size_t iInfoSize = m_mapColInfo.size();
	fwrite(&iInfoSize, sizeof(size_t), 1, _pFIle);
	for (auto iter{ m_mapColInfo.begin() }; iter != m_mapColInfo.end(); ++iter)
	{
		fwrite(&iter->first, sizeof(ULONGLONG), 1, _pFIle);
		fwrite(&iter->second, sizeof(bool), 1, _pFIle);
	}
}

void CCollisionMgr::LoadToFile(FILE* _pFIle)
{
	size_t iFrameCount = 0;
	fread(&iFrameCount, sizeof(size_t), 1, _pFIle);
	fread(m_matrix, sizeof(UINT), iFrameCount, _pFIle);

	size_t iInfoSize{};
	fread(&iInfoSize, sizeof(size_t), 1, _pFIle);
	ULONGLONG key{};
	bool value{};
	while (iInfoSize > 0)
	{

		fread(&key, sizeof(ULONGLONG), 1, _pFIle);
		fread(&value, sizeof(bool), 1, _pFIle);
		m_mapColInfo.insert(make_pair(key, value));
		--iInfoSize;
	}
}