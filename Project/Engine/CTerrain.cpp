#include "pch.h"
#include "CTerrain.h"

#include "CLevel.h"
#include "CLevelMgr.h"
#include "CResMgr.h"

#include "CTransform.h"
#include "CCamera.h"
#include "CStructuredBuffer.h"

#include "Base.h"

CTerrain::CTerrain()
	: CRenderComponent(COMPONENT_TYPE::TERRAIN)
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"TerrainMesh"));
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TileMapMtrl"));
	m_TileBuffer = new CStructuredBuffer;
}

CTerrain::~CTerrain()
{
	if (nullptr != m_TileBuffer)
		delete m_TileBuffer;
}

void CTerrain::begin()
{
}

void CTerrain::finaltick()
{
	static bool bCheck = false;
	if (!bCheck)
	{
		bCheck = true;
		m_AtlasTex[32] = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\Mask\\TileMask.png");
	}
	/*
	* 타일 Edit 모드이면 타일 벡터 순회 사이즈 조절
	*/
}

void CTerrain::render()
{
	Transform()->UpdateData();

	if (m_pCamera)
		m_vCameraPos = m_pCamera->Transform()->GetRelativePos();


	//GetCurMaterial()->SetScalarParam(VEC4_0, m_vTileSize);
	//GetCurMaterial()->SetScalarParam(VEC4_0, &m_vCameraPos);
	
	GetCurMaterial()->SetTexParam(TEX_0, m_AtlasTex[0]);
	GetCurMaterial()->SetTexParam(TEX_1, m_AtlasTex[1]);
	GetCurMaterial()->SetTexParam(TEX_2, m_AtlasTex[2]);
	GetCurMaterial()->SetTexParam(TEX_3, m_AtlasTex[3]);
	GetCurMaterial()->SetTexParam(TEX_4, m_AtlasTex[4]);
	GetCurMaterial()->SetTexParam(TEX_5, m_AtlasTex[5]);
	GetCurMaterial()->SetTexParam(TEX_6, m_AtlasTex[6]);
	GetCurMaterial()->SetTexParam(TEX_7, m_AtlasTex[7]);
	GetCurMaterial()->SetTexParam(TEX_8, m_AtlasTex[8]);
	GetCurMaterial()->SetTexParam(TEX_9, m_AtlasTex[9]);
	GetCurMaterial()->SetTexParam(TEX_10, m_AtlasTex[10]);
	GetCurMaterial()->SetTexParam(TEX_11, m_AtlasTex[11]);
	GetCurMaterial()->SetTexParam(TEX_12, m_AtlasTex[12]);
	GetCurMaterial()->SetTexParam(TEX_13, m_AtlasTex[13]);
	GetCurMaterial()->SetTexParam(TEX_14, m_AtlasTex[14]);
	GetCurMaterial()->SetTexParam(TEX_15, m_AtlasTex[15]);
	GetCurMaterial()->SetTexParam(TEX_16, m_AtlasTex[16]);
	GetCurMaterial()->SetTexParam(TEX_17, m_AtlasTex[17]);
	GetCurMaterial()->SetTexParam(TEX_18, m_AtlasTex[18]);
	GetCurMaterial()->SetTexParam(TEX_19, m_AtlasTex[19]);
	GetCurMaterial()->SetTexParam(TEX_20, m_AtlasTex[20]);
	GetCurMaterial()->SetTexParam(TEX_21, m_AtlasTex[21]);
	GetCurMaterial()->SetTexParam(TEX_22, m_AtlasTex[22]);
	GetCurMaterial()->SetTexParam(TEX_23, m_AtlasTex[23]);
	GetCurMaterial()->SetTexParam(TEX_24, m_AtlasTex[24]);
	GetCurMaterial()->SetTexParam(TEX_25, m_AtlasTex[25]);
	GetCurMaterial()->SetTexParam(TEX_26, m_AtlasTex[26]);
	GetCurMaterial()->SetTexParam(TEX_27, m_AtlasTex[27]);
	GetCurMaterial()->SetTexParam(TEX_28, m_AtlasTex[28]);
	GetCurMaterial()->SetTexParam(TEX_29, m_AtlasTex[29]);
	GetCurMaterial()->SetTexParam(TEX_30, m_AtlasTex[30]);
	GetCurMaterial()->SetTexParam(TEX_31, m_AtlasTex[31]);
	GetCurMaterial()->SetTexParam(TEX_32, m_AtlasTex[32]);

	GetCurMaterial()->UpdateData();

	GetMesh()->render_particle(1);

	CMaterial::Clear();
}



void CTerrain::SaveToFile(FILE* _File)
{
	CRenderComponent::SaveToFile(_File);

	size_t size = m_AtlasTex.size();

	fwrite(&size, sizeof(size_t), 1, _File);
	
	for (size_t i{}; i < size; ++i)
	{
		SaveResourceRef(m_AtlasTex[i], _File);
	}
}

void CTerrain::LoadFromFile(FILE* _File)
{
	CRenderComponent::LoadFromFile(_File);

	size_t size = 0;

	fread(&size, sizeof(size_t), 1, _File);

	m_AtlasTex.resize(size);

	for (size_t i{}; i < size; ++i)
	{
		LoadResourceRef(m_AtlasTex[i], _File);
	}
}

void CTerrain::SetTextureID(Ray _ray, UINT i)
{
	GetMesh()->SetTextureID(_ray, (float)i);
}