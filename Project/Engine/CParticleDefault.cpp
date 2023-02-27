#include "pch.h"
#include "CParticleDefault.h"
#include "CStructuredBuffer.h"

#include "CResMgr.h"
#include "CTimeMgr.h"

#include "CTransform.h"

CParticleDefault::CParticleDefault()
	:CParticleSystem()

{
	m_iMaxCount = 10;
	m_fAccTime = 10;
	m_fFrequency = 10.f;

	m_vStartColor = Vec4{ 0.24f, 0.28f, 0.7f, 1.f };
	m_vEndColor = Vec4{ 0.78f, 0.74f, 0.9f, 1.f };
	m_vStartScale = Vec4{ 50.f, 50.f, 1.f, 0.f };
	m_vEndScale = Vec4{ 1.f, 1.f, 1.f, 0.f };
	m_vMinMaxSpeed = Vec2{ 100.f, 300.f };
	m_vMinMaxLifeTime = Vec2{ 0.2f, 10.f };
	m_fSpawnRange = 1900.f;
	m_WorldSpawn = 0;
	m_iType = 1;

	SetName(L"CParticleDefault");
	GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"SmokeParticle"));
}

CParticleDefault::CParticleDefault(const CParticleDefault& _Rhs)
	:CParticleSystem(_Rhs)
{
	SetName(L"CParticleDefault");
	GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"SmokeParticle"));
}

CParticleDefault::~CParticleDefault()
{
}

void CParticleDefault::finaltick()
{
	float fAliveTime = 1.f / m_fFrequency;

	m_fAccTime += DT;

	if (fAliveTime < m_fAccTime)
	{
		float f = m_fAccTime / fAliveTime;
		UINT iAliveCount = (UINT)f;
		m_fAccTime = f - floor(f);
		m_share.iAliveCount = 10;
		m_ParticleShare->SetData(&m_share, 1);
	}
	else
	{
		m_share.iAliveCount = 0;
		m_ParticleShare->SetData(&m_share, 1);
	}

	__super::finaltick();
}

void CParticleDefault::render()
{
	__super::render();

	// 렌더링
	GetMesh()->render_particle(m_iMaxCount);

	// 클리어
	m_ParticleBuffer->Clear();
}

void CParticleDefault::SaveToFile(FILE* _File)
{
	__super::SaveToFile(_File);
}
void CParticleDefault::LoadFromFile(FILE* _File)
{
	__super::SaveToFile(_File);
}