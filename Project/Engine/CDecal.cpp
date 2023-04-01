#include "pch.h"
#include "CDecal.h"

#include "CTransform.h"

CDecal::CDecal()
	: CRenderComponent(COMPONENT_TYPE::DECAL)
	, m_bLighting(false)
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	SetDefaultLit(m_bLighting);
}

CDecal::CDecal(const CDecal& _rhs)
	: CRenderComponent(COMPONENT_TYPE::DECAL)
	, m_bLighting(_rhs.m_bLighting)
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	m_DecalTex = CResMgr::GetInst()->FindRes<CTexture>(_rhs.m_DecalTex->GetName());
	SetDefaultLit(m_bLighting);
}

CDecal::~CDecal()
{
}

void CDecal::SetDefaultLit(bool _bSet)
{
	m_bLighting = _bSet;

	if (m_bLighting)
	{
		SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DeferredDecalMtrl"), 0);
	}
	else
	{
		SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DecalMtrl"), 0);
	}
}

void CDecal::finaltick()
{
}

void CDecal::render()
{
	//DebugDrawCube(Vec4(0.2f, 0.8f, 0.2f, 1.f), Transform()->GetWorldPos(), Transform()->GetWorldScale(), Transform()->GetRelativeRotation());

	// 위치 행렬 업데이트
	// 데칼을 찍을 큐브의 위치 행렬을 셰이더로 업데이트 한다.
	Transform()->UpdateData();

	// 머테리얼 업데이트
	/* TEX_0 Position Tex
	*  TEX_1 Decal Tex
	*/
	GetCurMaterial(0)->SetTexParam(TEX_1, m_DecalTex);
	GetCurMaterial(0)->UpdateData();

	// 렌더링
	GetMesh()->render(0);

	CMaterial::Clear();
}

void CDecal::render_Instancing()
{
	if (!IsActive())
		return;

	if (nullptr == GetCurMaterial(0) || nullptr == GetMesh() || nullptr == Transform())
		return;

	//월드 정보만 갱신 후
	Transform()->Update();

	tTransform transform = g_transform;
	tMtrlConst tMtrl = GetCurMaterial(0)->GetMaterial();
	tAnim2DInfo tAnimInfo{};

	tObjectRender tObjectInfo = { transform, tMtrl, tAnimInfo };

	g_vecInfoObject.push_back(tObjectInfo);
}


void CDecal::SaveToFile(FILE * _File)
{
	// 참조 아틀라스 텍스쳐
	COMPONENT_TYPE type = GetType();

	fwrite(&type, sizeof(UINT), 1, _File);
	SaveResourceRef<CTexture>(m_DecalTex, _File);
}

void CDecal::LoadFromFile(FILE* _File)
{
	LoadResourceRef<CTexture>(m_DecalTex, _File);
}
