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

	//���⼭ �� Ʈ������?
	if (nullptr == GetMesh())
		return;
	/*
	* �ؽ�ó ���� ������Ʈ ó��
	*
	* CRenderMgr::UpdateLight2D() ����
	* �̱� �� �Ǵ� static vector ó��
	*/
	//���� ����

	if (Animator2D())
	{
		//���� ����
		Animator2D()->UpdateData();
	}

	// Animator3D ������Ʈ
	if (Animator3D())
	{
		Animator3D()->UpdateData();

		for (size_t i = 0; i < GetMtrlCount(); ++i)
		{
			if (nullptr == GetCurMaterial(i))
				continue;
			GetCurMaterial(i)->SetAnim3D(true); // Animation Mesh �˸���
			GetCurMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
		}
	}

	if (TrailComponent())
	{
		TrailComponent()->UpdateData();
	}

	Transform()->UpdateData();
	// ����� ���� ������Ʈ
	UINT iSubsetCount = GetMesh()->GetSubsetCount();

	for (int i = 0; i < iSubsetCount; ++i)
	{
		if (nullptr != GetCurMaterial(i))
		{
			GetCurMaterial(i)->UpdateData();

			// ����� �޽� ������Ʈ �� ������
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
	// Transform ������Ʈ

	// Animation2D ������Ʈ
	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	// Animator3D ������Ʈ
	if (Animator3D())
	{
		Animator3D()->UpdateData();
		GetCurMaterial(_iSubset)->SetAnim3D(true); // Animation Mesh �˸���
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