#include "pch.h"
#include "CSkyBox.h"

#include "CTransform.h"

CSkyBox::CSkyBox()
	: CRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_Type(SKYBOX_TYPE::SPHERE)
{
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyBoxMtrl"));
	SetType(m_Type);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::finaltick()
{
	// MainCam 위치를 알아낸다.
	// SkyBox 의 위치값을 갱신

	// MainCam 의 Far 값을 가져온다
	// Transform Scale 로 적용
}

void CSkyBox::render()
{
	Transform()->UpdateData();

	GetCurMaterial()->SetScalarParam(INT_0, (void*)&m_Type);

	if (SKYBOX_TYPE::SPHERE == m_Type)
	{
		GetCurMaterial()->SetTexParam(TEX_0, m_SkyBoxTex);
	}
	else
	{
		GetCurMaterial()->SetTexParam(TEX_CUBE_0, m_SkyBoxTex);
	}

	GetCurMaterial()->UpdateData();

	GetMesh()->render();

	CMaterial::Clear();
}

void CSkyBox::SetType(SKYBOX_TYPE _type)
{
	m_Type = _type;

	if (SKYBOX_TYPE::CUBE == _type)
	{
		SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	}

	else if (SKYBOX_TYPE::SPHERE == _type)
	{
		SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	}
}

void CSkyBox::SetSkyBoxTex(Ptr<CTexture> _tex)
{
	m_SkyBoxTex = _tex;
}

void CSkyBox::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE type = GetType();
	fwrite(&type, sizeof(UINT), 1, _File);
	
	fwrite(&m_Type, sizeof(UINT), 1, _File);
}

void CSkyBox::LoadFromFile(FILE* _File)
{
	fread(&m_Type, sizeof(UINT), 1, _File);

	SetType(m_Type);
}