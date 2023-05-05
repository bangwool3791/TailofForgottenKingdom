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

bool CCollisionMgr::IsCollision(CCollider3D* _pLeft, CCollider3D* _pRight)
{
	// 0 --- 1
	// |  \  | 
	// 3 --- 2	

//#ifdef true

	static const Vec3 arrLocalPos[8]
		=
	{
		Vec3(-0.5f, 0.5f, -0.5f),
		Vec3(0.5f, 0.5f, -0.5f),
		Vec3(0.5f, 0.5f, 0.5f),
		Vec3(-0.5f, 0.5f, 0.5f),
		Vec3(-0.5f, -0.5f, -0.5f),
		Vec3(0.5f, -0.5f, -0.5f),
		Vec3(0.5f, -0.5f, 0.5f),
		Vec3(-0.5f, -0.5f, 0.5f),
	};

	if (_pLeft->GetCollider3DType() == COLLIDER3D_TYPE::COLLIDER3D_CUBE &&_pRight->GetCollider3DType() == COLLIDER3D_TYPE::COLLIDER3D_CUBE)
	{
		// 분리축 구하기
		static Vec3 vAxis[24] = {};

		const Matrix& matLeft = _pLeft->GetWorldMat();
		const Matrix& matRight = _pRight->GetWorldMat();

		// 분리축 벡터 == 투영벡터
		//left cube
		vAxis[0] = XMVector3TransformCoord(arrLocalPos[0], matLeft) - XMVector3TransformCoord(arrLocalPos[3], matLeft);
		vAxis[1] = XMVector3TransformCoord(arrLocalPos[1], matLeft) - XMVector3TransformCoord(arrLocalPos[3], matLeft);
		vAxis[2] = XMVector3TransformCoord(arrLocalPos[7], matLeft) - XMVector3TransformCoord(arrLocalPos[3], matLeft);

		vAxis[3] = XMVector3TransformCoord(arrLocalPos[0], matLeft) - XMVector3TransformCoord(arrLocalPos[1], matLeft);
		vAxis[4] = XMVector3TransformCoord(arrLocalPos[2], matLeft) - XMVector3TransformCoord(arrLocalPos[1], matLeft);
		vAxis[5] = XMVector3TransformCoord(arrLocalPos[5], matLeft) - XMVector3TransformCoord(arrLocalPos[1], matLeft);

		vAxis[6] = XMVector3TransformCoord(arrLocalPos[2], matLeft) - XMVector3TransformCoord(arrLocalPos[6], matLeft);
		vAxis[7] = XMVector3TransformCoord(arrLocalPos[5], matLeft) - XMVector3TransformCoord(arrLocalPos[6], matLeft);
		vAxis[8] = XMVector3TransformCoord(arrLocalPos[7], matLeft) - XMVector3TransformCoord(arrLocalPos[6], matLeft);

		vAxis[9] = XMVector3TransformCoord(arrLocalPos[0], matLeft) - XMVector3TransformCoord(arrLocalPos[4], matLeft);
		vAxis[10] = XMVector3TransformCoord(arrLocalPos[5], matLeft) - XMVector3TransformCoord(arrLocalPos[4], matLeft);
		vAxis[11] = XMVector3TransformCoord(arrLocalPos[7], matLeft) - XMVector3TransformCoord(arrLocalPos[4], matLeft);

		//right cube
		vAxis[12] = XMVector3TransformCoord(arrLocalPos[0], matRight) - XMVector3TransformCoord(arrLocalPos[3], matRight);
		vAxis[13] = XMVector3TransformCoord(arrLocalPos[1], matRight) - XMVector3TransformCoord(arrLocalPos[3], matRight);
		vAxis[14] = XMVector3TransformCoord(arrLocalPos[7], matRight) - XMVector3TransformCoord(arrLocalPos[3], matRight);

		vAxis[15] = XMVector3TransformCoord(arrLocalPos[0], matRight) - XMVector3TransformCoord(arrLocalPos[1], matRight);
		vAxis[16] = XMVector3TransformCoord(arrLocalPos[2], matRight) - XMVector3TransformCoord(arrLocalPos[1], matRight);
		vAxis[17] = XMVector3TransformCoord(arrLocalPos[5], matRight) - XMVector3TransformCoord(arrLocalPos[1], matRight);

		vAxis[18] = XMVector3TransformCoord(arrLocalPos[2], matRight) - XMVector3TransformCoord(arrLocalPos[6], matRight);
		vAxis[19] = XMVector3TransformCoord(arrLocalPos[5], matRight) - XMVector3TransformCoord(arrLocalPos[6], matRight);
		vAxis[20] = XMVector3TransformCoord(arrLocalPos[7], matRight) - XMVector3TransformCoord(arrLocalPos[6], matRight);

		vAxis[21] = XMVector3TransformCoord(arrLocalPos[0], matRight) - XMVector3TransformCoord(arrLocalPos[4], matRight);
		vAxis[22] = XMVector3TransformCoord(arrLocalPos[5], matRight) - XMVector3TransformCoord(arrLocalPos[4], matRight);
		vAxis[23] = XMVector3TransformCoord(arrLocalPos[7], matRight) - XMVector3TransformCoord(arrLocalPos[4], matRight);
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

		Vec3 vC = _pLeft->GetFinalPos() - _pRight->GetFinalPos();
		Vec3 vCenterDir = Vec3{ vC.x, vC.y, vC.z };

		float fSum{ 0.f };

		for (int i{ 0 }; i < 24; ++i)
		{
			Vec3 vA = vAxis[i];
			vA.Normalize();

			float fProjDist{ 0.f };

			for (int j{ 0 }; j < 24; ++j)
			{
				fProjDist += fabsf(vAxis[j].Dot(vA)) / 2.f;
			}

			if (fProjDist < fabsf(vCenterDir.Dot(vA)))
			{
				return false;
			}
		}
		return true;
	}
	else if (_pLeft->GetCollider3DType() == COLLIDER3D_TYPE::COLLIDER3D_SPHERE && _pRight->GetCollider3DType() == COLLIDER3D_TYPE::COLLIDER3D_SPHERE)
	{
		Vec3 vLeftPos = _pLeft->Transform()->GetRelativePos();
		Vec3 vRightPos = _pRight->Transform()->GetRelativePos();

		Vec3 vLeftScale = _pLeft->GetScale();
		Vec3 vRightScale = _pRight->GetScale();

		float fScaleDistance = vLeftScale.Distance(vLeftScale, vRightScale);
		float fDistance = vLeftPos.Distance(vLeftPos, vRightPos);

		if (fDistance <= fScaleDistance)
		{
			return true;
		}

		return false;
	}
	return false;
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