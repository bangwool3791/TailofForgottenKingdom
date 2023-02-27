#include "pch.h"
#include "CParticleExplosion.h"
#include "CParticleSystem.h"
#include "CStructuredBuffer.h"

#include "CResMgr.h"
#include "CTimeMgr.h"

#include "CTransform.h"


CParticleExplosion::CParticleExplosion()
	:CParticleSystem()
{

	m_iMaxCount = 10;
	m_fAccTime = 10;
	m_fFrequency = 5;
	
	m_vStartColor = Vec4{ 0.82f, 0.82f, 0.82f, 1.f };
	m_vEndColor = Vec4{ 0.972f, 0.976f, 0.980f, 1.f };
	m_vStartScale = Vec4{ 10.f, 10.f, 1.f, 0.f };
	m_vEndScale = Vec4{ 100.f, 100.f, 1.f, 0.f };
	m_vMinMaxSpeed = Vec2{ 100.f, 300.f };
	m_vMinMaxLifeTime = Vec2{ 1.f, 3.f };
	m_fSpawnRange = 1.f;
	m_WorldSpawn = 1;
	m_iType = 1;

	SetName(L"CParticleExplosion");
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleRenderMtrl"));
	GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"SmokeParticle"));
	m_UpdateCS = (CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateShader").Get();
	m_share.iAliveCount = 1;
	m_ParticleShare->SetData(&m_share, 1);
}

CParticleExplosion::CParticleExplosion(const CParticleExplosion& _Rhs)
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

	SetName(L"CParticleExplosion");

	GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"SmokeParticle"));
}

CParticleExplosion::~CParticleExplosion()
{
}

void CParticleExplosion::finaltick()
{
	float fAliveTime = 1.f / m_fFrequency;

	m_fAccTime += DT;

	m_UpdateCS->SetParticleBuffer(m_ParticleBuffer);
	m_UpdateCS->SetParticleShareData(m_ParticleShare);

	__super::finaltick();
}

void CParticleExplosion::render()
{
	__super::render();

	m_ParticleBuffer->Clear();
}

void CParticleExplosion::SaveToFile(FILE* _File)
{
	__super::SaveToFile(_File);
}
void CParticleExplosion::LoadFromFile(FILE* _File)
{
	__super::SaveToFile(_File);
}