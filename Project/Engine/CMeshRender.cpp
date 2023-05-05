#include "pch.h"
#include "CMeshRender.h"
#include "CRenderMgr.h"
#include "CTransform.h"
#include "CAnimator2D.h"
#include "CAnimator3D.h"
#include "CTrailComponent.h"
CMeshRender::CMeshRender()
	:CRenderComponent{ COMPONENT_TYPE::MESHRENDER }
{
	SetName(L"CMeshRender");
	m_eInsType = INSTANCING_TYPE::NONE;
}

CMeshRender::CMeshRender(INSTANCING_TYPE _eInsType)
	:CRenderComponent{ COMPONENT_TYPE::MESHRENDER }
{
	m_eInsType = _eInsType;
}


CMeshRender::CMeshRender(const CMeshRender& rhs)
	:CRenderComponent{ rhs }
{
	m_eInsType = rhs.m_eInsType;
}

CMeshRender::~CMeshRender()
{

}

void CMeshRender::tick()
{

}

void CMeshRender::finaltick()
{

}

void CMeshRender::render()
{
	if (!IsActive())
		return;

	//여기서 왜 트랜스폼?
	if (nullptr == GetMesh())
		return;
	/*
	* 텍스처 버퍼 업데이트 처리
	*
	* CRenderMgr::UpdateLight2D() 참고
	* 싱글 톤 또는 static vector 처리
	*/
	//삭제 예정

	if (Animator2D())
	{
		//삭제 예정
		Animator2D()->UpdateData();
	}

	// Animator3D 업데이트
	if (Animator3D())
	{
		Animator3D()->UpdateData();

		for (size_t i = 0; i < GetMtrlCount(); ++i)
		{
			if (nullptr == GetCurMaterial(i))
				continue;
			GetCurMaterial(i)->SetAnim3D(true); // Animation Mesh 알리기
			GetCurMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
		}
	}

	if (TrailComponent())
	{
		TrailComponent()->UpdateData();
	}

	Transform()->UpdateData();
	// 사용할 재질 업데이트
	UINT iSubsetCount = GetMesh()->GetSubsetCount();

	for (int i = 0; i < iSubsetCount; ++i)
	{
		if (nullptr != GetCurMaterial(i))
		{
			GetCurMaterial(i)->UpdateData();

			// 사용할 메쉬 업데이트 및 렌더링
			GetMesh()->render(i);
		}
	}

	CMaterial::Clear();

	if (Animator2D())
		Animator2D()->Clear();

	if (Animator3D())
		Animator3D()->ClearData();

	if (TrailComponent())
		TrailComponent()->Clear();
}

void CMeshRender::render(UINT _iSubset)
{
	if (!IsActive())
		return;

	if (nullptr == GetMesh() || nullptr == GetCurMaterial(_iSubset))
		return;

	// ==========
	// UpdateData
	// ==========
	// Transform 업데이트

	// Animation2D 업데이트
	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	// Animator3D 업데이트
	if (Animator3D())
	{
		Animator3D()->UpdateData();
		GetCurMaterial(_iSubset)->SetAnim3D(true); // Animation Mesh 알리기
		GetCurMaterial(_iSubset)->SetBoneCount(Animator3D()->GetBoneCount());
	}

	if (TrailComponent())
	{
		TrailComponent()->UpdateData();
	}

	Transform()->UpdateData();
	// ======
	// Render
	// ======
	GetCurMaterial(_iSubset)->UpdateData();
	GetMesh()->render(_iSubset);

	// =====
	// Clear
	// =====	
	if (Animator2D())
		Animator2D()->Clear();

	if (Animator3D())
		Animator3D()->ClearData();

	if (TrailComponent())
		TrailComponent()->Clear();
}

void CMeshRender::SaveToFile(FILE* _File)
{
	__super::SaveToFile(_File);
}

void CMeshRender::LoadFromFile(FILE* _File)
{
	__super::LoadFromFile(_File);
}