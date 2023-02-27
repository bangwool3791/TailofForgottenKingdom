#include "pch.h"
#include "CParticleSystem.h"
#include "CStructuredBuffer.h"

#include "CResMgr.h"
#include "CTimeMgr.h"

#include "CTransform.h"

CParticleSystem::CParticleSystem()
	:CRenderComponent(COMPONENT_TYPE::PARTICLESYSTEM)
	, m_iMaxCount(10)
	, m_ParticleBuffer{}
	, m_ParticleShare{}
	, m_arrParticle{}
	, m_fAccTime{}

{
	//SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));

	m_ParticleBuffer = new CStructuredBuffer;

	tParticle arr[10] = {};
	for (UINT i{ 0 }; i < 10; ++i)
	{
		m_arrParticle[i].fSpeed = arr[i].fSpeed = 200.f;
		m_arrParticle[i].vDir   = arr[i].vDir   = Vec2(cosf(XM_PI * i / 5), sinf(XM_PI * i / 5));
	}

	m_ParticleBuffer->Create(sizeof(tParticle), m_iMaxCount, SB_TYPE::UAV_INC, &arr);

	m_ParticleShare = new CStructuredBuffer;
	m_ParticleShare->Create(sizeof(tParticleShare), 1, SB_TYPE::UAV_INC, nullptr, true);

}

CParticleSystem::CParticleSystem(COMPONENT_TYPE _type)
	:CRenderComponent(_type)
	, m_iMaxCount(10)
	, m_ParticleBuffer{}
	, m_ParticleShare{}
	, m_arrParticle{}
	, m_fAccTime{}

{
	//SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));

	m_ParticleBuffer = new CStructuredBuffer;

	tParticle arr[10] = {};
	for (UINT i{ 0 }; i < 10; ++i)
	{
		m_arrParticle[i].fSpeed = arr[i].fSpeed = 200.f;
		m_arrParticle[i].vDir = arr[i].vDir = Vec2(cosf(XM_PI * i / 5), sinf(XM_PI * i / 5));
	}

	m_ParticleBuffer->Create(sizeof(tParticle), m_iMaxCount, SB_TYPE::UAV_INC, &arr);

	m_ParticleShare = new CStructuredBuffer;
	m_ParticleShare->Create(sizeof(tParticleShare), 1, SB_TYPE::UAV_INC, nullptr, true);
}

CParticleSystem::CParticleSystem(const CParticleSystem& _Rhs)
	:CRenderComponent(COMPONENT_TYPE::PARTICLESYSTEM)
	, m_iMaxCount(_Rhs.m_iMaxCount)
	, m_fMinLifeTime(_Rhs.m_fMinLifeTime)
	, m_fMaxLifeTime(_Rhs.m_fMaxLifeTime)
	, m_fAccTime{_Rhs.m_fAccTime}
	, m_ParticleBuffer{}
	, m_ParticleShare{}
{
	memcpy(&m_arrParticle, _Rhs.m_arrParticle, sizeof(_Rhs.m_arrParticle) / sizeof(tParticle));

	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleRenderMtrl"));

	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_iMaxCount, SB_TYPE::UAV_INC, const_cast<tParticle*>(m_arrParticle));

	m_ParticleShare = new CStructuredBuffer;
	m_ParticleShare->Create(sizeof(tParticleShare), 1, SB_TYPE::UAV_INC, &m_share, true);
}

CParticleSystem::~CParticleSystem()
{
	Safe_Delete(m_ParticleBuffer);
	Safe_Delete(m_ParticleShare);
}

void CParticleSystem::finaltick()
{
	float fAliveTime = 1.f / m_fFrequency;

	m_fAccTime += DT;

	if (fAliveTime < m_fAccTime)
	{
		float f = m_fAccTime / fAliveTime;
		UINT iAliveCount = (UINT)f;
		m_fAccTime = f - floor(f);
		m_share.iAliveCount = 1;
		m_ParticleShare->SetData(&m_share, 1);
	}
	else
	{
		m_share.iAliveCount = 0;
		m_ParticleShare->SetData(&m_share, 1);
	}


	m_UpdateCS->SetWorldSpawn(m_WorldSpawn);
	m_UpdateCS->SetObjectWorldPos(Transform()->GetWorldPos());
	m_UpdateCS->SetSpawnRange(m_fSpawnRange);
	m_UpdateCS->SetMinMaxSpeed(m_vMinMaxSpeed);
	m_UpdateCS->SetMinMaxLifeTime(m_vMinMaxLifeTime);
	m_UpdateCS->SetParticleType(m_iType);
	m_UpdateCS->Excute();
}

void CParticleSystem::render()
{
	Transform()->UpdateData();
	m_ParticleBuffer->UpdateData(17, PIPELINE_STAGE::VS | PIPELINE_STAGE::GS | PIPELINE_STAGE::PS);

	GetCurMaterial()->SetScalarParam(INT_1, &m_WorldSpawn);
	GetCurMaterial()->SetScalarParam(VEC4_0, &m_vStartScale);
	GetCurMaterial()->SetScalarParam(VEC4_1, &m_vEndScale);
	GetCurMaterial()->SetScalarParam(VEC4_2, &m_vStartColor);
	GetCurMaterial()->SetScalarParam(VEC4_3, &m_vEndColor);
	GetCurMaterial()->UpdateData();
}

void CParticleSystem::SaveToFile(FILE* _File)
{
	CRenderComponent::SaveToFile(_File);

	// 컴퓨트 쉐이더 정보 저장
	bool bCS = !!m_UpdateCS.Get();
	fwrite(&bCS, sizeof(bool), 1, _File);

	if (bCS)
	{
		SaveWStringToFile(m_UpdateCS->GetKey(), _File);
	}

	// 파티클 옵션
	fwrite(&m_iMaxCount, sizeof(UINT), 1, _File);
	fwrite(&m_iAliveCount, sizeof(UINT), 1, _File);
	fwrite(&m_vStartScale, sizeof(Vec4), 1, _File);
	fwrite(&m_vEndScale, sizeof(Vec4), 1, _File);
	fwrite(&m_vStartColor, sizeof(Vec4), 1, _File);
	fwrite(&m_vEndColor, sizeof(Vec4), 1, _File);
	fwrite(&m_vMinMaxSpeed, sizeof(Vec2), 1, _File);
	fwrite(&m_vMinMaxLifeTime, sizeof(Vec2), 1, _File);
	fwrite(&m_fSpawnRange, sizeof(float), 1, _File);
	fwrite(&m_fFrequency, sizeof(float), 1, _File);
	fwrite(&m_fAccTime, sizeof(float), 1, _File);
	fwrite(&m_WorldSpawn, sizeof(int), 1, _File);
}

void CParticleSystem::LoadFromFile(FILE* _File)
{
	CRenderComponent::LoadFromFile(_File);

	// 컴퓨트쉐이더 참조
	bool bCS = false;
	fread(&bCS, sizeof(bool), 1, _File);

	if (bCS)
	{
		wstring strKey;
		LoadWStringFromFile(strKey, _File);
		m_UpdateCS = dynamic_cast<CParticleUpdateShader*>(CResMgr::GetInst()->FindRes<CComputeShader>(strKey).Get());
	}

	// 파티클 옵션
	fwrite(&m_iMaxCount, sizeof(UINT), 1, _File);
	fwrite(&m_iAliveCount, sizeof(UINT), 1, _File);
	fwrite(&m_vStartScale, sizeof(Vec4), 1, _File);
	fwrite(&m_vEndScale, sizeof(Vec4), 1, _File);
	fwrite(&m_vStartColor, sizeof(Vec4), 1, _File);
	fwrite(&m_vEndColor, sizeof(Vec4), 1, _File);
	fwrite(&m_vMinMaxSpeed, sizeof(Vec2), 1, _File);
	fwrite(&m_vMinMaxLifeTime, sizeof(Vec2), 1, _File);
	fwrite(&m_fSpawnRange, sizeof(float), 1, _File);
	fwrite(&m_fFrequency, sizeof(float), 1, _File);
	fwrite(&m_fAccTime, sizeof(float), 1, _File);
	fwrite(&m_WorldSpawn, sizeof(int), 1, _File);
}

