#include "pch.h"
#include "CRenderComponent.h"

#include "CTransform.h"
#include "CAnimator3D.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _eComponentType)
	:CComponent(_eComponentType)
	, m_bUseFrustumCulling(true)
	, m_bDynamicShadow(true)

{
	SetName(L"CRenderComponent");
}

CRenderComponent::CRenderComponent(const CRenderComponent& _origin)
	: CComponent(_origin)
	, m_pMesh(_origin.m_pMesh)
	, m_bUseFrustumCulling(_origin.m_bUseFrustumCulling)
	, m_bDynamicShadow(_origin.m_bDynamicShadow)
{
	if (!_origin.m_vecMtrls.empty())
	{
		UINT size = _origin.m_vecMtrls.size();
		m_vecMtrls.resize(size);
		for (size_t i = 0; i < _origin.m_vecMtrls.size(); ++i)
		{
			SetSharedMaterial(_origin.m_vecMtrls[i].pSharedMtrl, i);
		}
	}
}


CRenderComponent::~CRenderComponent()
{
	for (size_t i = 0; i < m_vecMtrls.size(); ++i)
	{
		m_vecMtrls[i].pDynamicMtrl = nullptr;
	}
}

void CRenderComponent::SetMesh(Ptr<CMesh> _pMesh)
{
	m_pMesh = _pMesh;

	if (!m_vecMtrls.empty())
	{
		m_vecMtrls.clear();
		vector<tMtrlSet> vecMtrls;
		m_vecMtrls.swap(vecMtrls);
	}

	m_vecMtrls.resize(m_pMesh->GetSubsetCount());
}

void CRenderComponent::SetSharedMaterial(Ptr<CMaterial> _pMtrl, UINT _iIdx)
{
	m_vecMtrls[_iIdx].pSharedMtrl = _pMtrl;
	m_vecMtrls[_iIdx].pCurMtrl = _pMtrl;
}

void CRenderComponent::ClearMaterials()
{
	vector<tMtrlSet> temp;
	m_vecMtrls.clear();
	m_vecMtrls.swap(temp);
}

Ptr<CMaterial> CRenderComponent::GetCurMaterial(UINT _iIdx)
{
	assert(!(m_vecMtrls.size() == 0));

	if (nullptr == m_vecMtrls[_iIdx].pCurMtrl)
	{
		m_vecMtrls[_iIdx].pCurMtrl = m_vecMtrls[_iIdx].pSharedMtrl;
	}

	return m_vecMtrls[_iIdx].pCurMtrl;
}

Ptr<CMaterial> CRenderComponent::GetSharedMaterial(UINT _iIdx)
{
	m_vecMtrls[_iIdx].pCurMtrl = m_vecMtrls[_iIdx].pSharedMtrl;

	if (m_vecMtrls[_iIdx].pDynamicMtrl.Get())
	{
		m_vecMtrls[_iIdx].pDynamicMtrl = nullptr;
	}

	return m_vecMtrls[_iIdx].pSharedMtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial(UINT _iIdx)
{
	if (nullptr != m_vecMtrls[_iIdx].pDynamicMtrl)
		return m_vecMtrls[_iIdx].pDynamicMtrl;

	m_vecMtrls[_iIdx].pDynamicMtrl = m_vecMtrls[_iIdx].pSharedMtrl->Clone();
	m_vecMtrls[_iIdx].pDynamicMtrl->SetName(m_vecMtrls[_iIdx].pSharedMtrl->GetName() + L"_Clone");
	m_vecMtrls[_iIdx].pCurMtrl = m_vecMtrls[_iIdx].pDynamicMtrl;

	return m_vecMtrls[_iIdx].pCurMtrl;
}

void CRenderComponent::render_depthmap()
{
	Transform()->UpdateData();

	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DepthMapMtrl");

	// Animator3D 업데이트
	if (Animator3D())
	{
		Animator3D()->UpdateData();
		pMtrl->SetAnim3D(true);
		pMtrl->SetBoneCount(Animator3D()->GetBoneCount());
	}

	// 사용할 재질 업데이트
	pMtrl->UpdateData();

	UINT iSubsetCount = GetMesh()->GetSubsetCount();

	for (int i = 0; i < iSubsetCount; ++i)
	{
		// 사용할 메쉬 업데이트 및 렌더링
		GetMesh()->render(i);
	}
	pMtrl->SetAnim3D(false); // Animation Mesh 알리기
	//뼈 개수 0
	pMtrl->SetBoneCount(0);
}

void CRenderComponent::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE type = GetType();
	fwrite(&type, sizeof(UINT), 1, _File);

	SaveResourceRef(m_pMesh, _File);

	UINT iMtrlCount = GetMtrlCount();
	fwrite(&iMtrlCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		SaveResourceRef(m_vecMtrls[i].pSharedMtrl, _File);
	}


	fwrite(&m_bDynamicShadow, 1, 1, _File);
	fwrite(&m_bUseFrustumCulling, 1, 1, _File);
}

void CRenderComponent::LoadFromFile(FILE* _File)
{
	LoadResourceRef(m_pMesh, _File);

	UINT iMtrlCount = GetMtrlCount();
	fread(&iMtrlCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		Ptr<CMaterial> pMtrl;
		LoadResourceRef(pMtrl, _File);
		SetSharedMaterial(pMtrl, i);
	}

	fread(&m_bDynamicShadow, 1, 1, _File);
	fread(&m_bUseFrustumCulling, 1, 1, _File);
}