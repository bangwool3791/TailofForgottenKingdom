#include "pch.h"
#include "CParticlePhycis.h"
#include "CParticleSystem.h"
#include "CStructuredBuffer.h"

#include "CResMgr.h"
#include "CTimeMgr.h"

#include "CTransform.h"


CParticlePhycis::CParticlePhycis()
	:CParticleSystem()
{

	m_iMaxCount = 10;
	m_fAccTime = 10;
	m_fFrequency = 5;

	m_vStartColor = Vec4{ 255.f / 255.f, 0.f, 0.f, 1.f };
	m_vEndColor = Vec4{ 249.f / 255.f, 130.f / 255.f, 15.f / 255.f, 1.f };
	m_vStartScale = Vec4{ 5.f, 5.f, 1.f, 0.f };
	m_vEndScale = Vec4{ 15.f, 15.f, 1.f, 0.f };
	m_vMinMaxSpeed = Vec2{ 100.f, 300.f };
	m_vMinMaxLifeTime = Vec2{ 0.f, 0.5f };
	m_fSpawnRange = 1.f;
	m_WorldSpawn = 1;
	m_iType = 1;

	SetName(L"CParticlePhycis");
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleRenderWoodMtrl"));
	GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\particle\\Sparks.png"));
	m_UpdateCS = (CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateWoodShader").Get();

	m_share.iAliveCount = 1;
	m_ParticleShare->SetData(&m_share, 1);
}

CParticlePhycis::CParticlePhycis(const CParticlePhycis& _Rhs)
	:CParticleSystem(_Rhs)
{
	m_iMaxCount = _Rhs.m_iMaxCount;
	m_fAccTime = _Rhs.m_fAccTime;
	m_fFrequency = _Rhs.m_fFrequency;

	m_vStartColor = _Rhs.m_vStartColor;
	m_vEndColor = _Rhs.m_vEndColor;
	m_vStartScale = _Rhs.m_vStartScale;
	m_vEndScale = _Rhs.m_vEndScale;
	m_vMinMaxSpeed = _Rhs.m_vMinMaxSpeed;
	m_vMinMaxLifeTime = _Rhs.m_vMinMaxLifeTime;
	m_fSpawnRange = _Rhs.m_fSpawnRange;
	m_WorldSpawn = _Rhs.m_WorldSpawn;
	m_iType = _Rhs.m_iType;

	SetName(L"CParticlePhycis");
	GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\particle\\Sparks.png"));
	m_UpdateCS = (CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateWoodShader").Get();
}

CParticlePhycis::~CParticlePhycis()
{
}

void CParticlePhycis::finaltick()
{
	float fAliveTime = 1.f / m_fFrequency;

	m_fAccTime += DT;

	m_UpdateCS->SetParticleBuffer(m_ParticleBuffer);
	m_UpdateCS->SetParticleShareData(m_ParticleShare);

	__super::finaltick();
}

void CParticlePhycis::render()
{
	__super::render();

	GetMesh()->render_particle(m_iMaxCount);

	m_ParticleBuffer->Clear();
}

void CParticlePhycis::SaveToFile(FILE* _File)
{
	__super::SaveToFile(_File);
}
void CParticlePhycis::LoadFromFile(FILE* _File)
{
	__super::SaveToFile(_File);
}
