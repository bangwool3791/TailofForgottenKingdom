#include "pch.h"
#include "CParticleSystem.h"
#include "CStructuredBuffer.h"

#include "CResMgr.h"
#include "CTimeMgr.h"

#include "CTransform.h"

CParticleSystem::CParticleSystem()
	: CRenderComponent(COMPONENT_TYPE::PARTICLESYSTEM)
	, m_iMaxCount(10)
	, m_iAliveCount(0)
	, m_vStartScale(Vec4(350.f, 350.f, 350.f, 0.f))
	, m_vEndScale(Vec4(50.f, 50.f, 50.f, 0.f))
	, m_vStartColor(Vec4(0.24f, 0.28f, 0.8f, 1.f))
	, m_vEndColor(Vec4(0.78f, 0.74f, 0.9f, 1.f))
	, m_vMinMaxSpeed(Vec2(400.f, 600.f))
	, m_vMinMaxLifeTime(Vec2(0.2f, 2.f))
	, m_fSpawnRange(200.f)
	, m_fFrequency(5.f)
	, m_fAccTime(0.f)
	, m_ParticleBuffer(nullptr)
	, m_ParticleShare(nullptr)
	, m_WorldSpawn(1)
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleRenderMtrl"),0);
	m_UpdateCS = (CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateShader").Get();

	//GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"SmokeParticle"));
	//GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"CartoonSmoke"));	

	GetCurMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\particle\\T_Decal_Crack03.png"));


	// ����ȭ���� ����
	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_iMaxCount, SB_TYPE::UAV_INC, nullptr, true);

	m_ParticleShare = new CStructuredBuffer;
	m_ParticleShare->Create(sizeof(tParticleShare), 1, SB_TYPE::UAV_INC, nullptr, true);
}

CParticleSystem::CParticleSystem(COMPONENT_TYPE _type)
	:CRenderComponent(_type)
	, m_iMaxCount(1000)
	, m_iAliveCount(0)
	, m_vStartScale(Vec4(150.f, 150.f, 150.f, 0.f))
	, m_vEndScale(Vec4(10.f, 10.f, 10.f, 0.f))
	, m_vStartColor(Vec4(0.24f, 0.28f, 0.8f, 1.f))
	, m_vEndColor(Vec4(0.78f, 0.74f, 0.9f, 1.f))
	, m_vMinMaxSpeed(Vec2(100.f, 300.f))
	, m_vMinMaxLifeTime(Vec2(0.2f, 1.f))
	, m_fSpawnRange(200.f)
	, m_fFrequency(5.f)
	, m_fAccTime(0.f)
	, m_ParticleBuffer(nullptr)
	, m_ParticleShare(nullptr)
	, m_WorldSpawn(1)

{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleRenderMtrl"),0);
	m_UpdateCS = (CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateShader").Get();

	//GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"SmokeParticle"));
	//GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"CartoonSmoke"));	
	GetCurMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\particle\\T_Decal_Crack03.png"));


	// ����ȭ���� ����
	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_iMaxCount, SB_TYPE::UAV_INC, nullptr);

	m_ParticleShare = new CStructuredBuffer;
	m_ParticleShare->Create(sizeof(tParticleShare), 1, SB_TYPE::UAV_INC, nullptr, true);
}

CParticleSystem::CParticleSystem(const CParticleSystem& _Rhs)
	:CRenderComponent(COMPONENT_TYPE::PARTICLESYSTEM)
	, m_iMaxCount(_Rhs.m_iMaxCount)
	, m_iAliveCount(_Rhs.m_iAliveCount)
	, m_vStartScale(_Rhs.m_vStartScale)
	, m_vEndScale(_Rhs.m_vEndScale)
	, m_vStartColor(_Rhs.m_vStartColor)
	, m_vEndColor(_Rhs.m_vEndColor)
	, m_vMinMaxSpeed(_Rhs.m_vMinMaxSpeed)
	, m_vMinMaxLifeTime(_Rhs.m_vMinMaxLifeTime)
	, m_fSpawnRange(_Rhs.m_fSpawnRange)
	, m_fFrequency(_Rhs.m_fFrequency)
	, m_fAccTime(_Rhs.m_fAccTime)
	, m_ParticleBuffer(nullptr)
	, m_ParticleShare(nullptr)
	, m_WorldSpawn(_Rhs.m_WorldSpawn)
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleRenderMtrl"),0);
	m_UpdateCS = (CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateShader").Get();

	//GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"SmokeParticle"));
	//GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"CartoonSmoke"));	
	GetCurMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\particle\\T_Decal_Crack03.png"));


	// ����ȭ���� ����
	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_iMaxCount, SB_TYPE::UAV_INC, nullptr);

	m_ParticleShare = new CStructuredBuffer;
	m_ParticleShare->Create(sizeof(tParticleShare), 1, SB_TYPE::UAV_INC, nullptr, true);
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
		float f = (m_fAccTime / fAliveTime);
		UINT iAliveCount = (UINT)f;
		m_fAccTime = f - floorf(f);

		//tParticleShare share = { iAliveCount, };
		tParticleShare share = { 10, };
		m_ParticleShare->SetData(&share, 1);
	}
	else
	{
		tParticleShare share = {};
		m_ParticleShare->SetData(&share, 1);
	}
	
	vector< tParticle> vec{};
	vec.resize(1000);
	m_ParticleBuffer->GetData(vec.data());
	m_UpdateCS->SetParticleBuffer(m_ParticleBuffer);
	m_UpdateCS->SetParticleShareData(m_ParticleShare);
	m_UpdateCS->SetWorldSpawn(m_WorldSpawn);
	m_UpdateCS->SetObjectWorldPos(Transform()->GetWorldPos());
	m_UpdateCS->SetSpawnRange(m_fSpawnRange);
	m_UpdateCS->SetMinMaxSpeed(m_vMinMaxSpeed);
	m_UpdateCS->SetMinMaxLifeTime(m_vMinMaxLifeTime);
	m_UpdateCS->Execute();
}	//

void CParticleSystem::render()
{
	// ��ġ���� ������Ʈ
	Transform()->UpdateData();
	m_ParticleBuffer->UpdateData(16, PIPELINE_STAGE::VS | PIPELINE_STAGE::GS | PIPELINE_STAGE::PS);
	
	// ���� ������Ʈ
	GetCurMaterial(0)->SetScalarParam(INT_1, &m_WorldSpawn);
	GetCurMaterial(0)->SetScalarParam(VEC4_0, &m_vStartScale);
	GetCurMaterial(0)->SetScalarParam(VEC4_1, &m_vEndScale);
	GetCurMaterial(0)->SetScalarParam(VEC4_2, &m_vStartColor);
	GetCurMaterial(0)->SetScalarParam(VEC4_3, &m_vEndColor);
	GetCurMaterial(0)->UpdateData();
	
	// ������
	GetMesh()->render_particle(m_iMaxCount);
	
	// Ŭ����
	m_UpdateCS->Clear();
	m_ParticleBuffer->Clear();
	CMaterial::Clear();
}

void CParticleSystem::render(UINT _iSubset)
{
	render();
}

void CParticleSystem::SaveToFile(FILE* _File)
{
	CRenderComponent::SaveToFile(_File);

	// ��ǻƮ ���̴� ���� ����
	bool bCS = !!m_UpdateCS.Get();
	fwrite(&bCS, sizeof(bool), 1, _File);

	if (bCS)
	{
		SaveWStringToFile(m_UpdateCS->GetKey(), _File);
	}

	// ��ƼŬ �ɼ�
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

	// ��ǻƮ���̴� ����
	bool bCS = false;
	fread(&bCS, sizeof(bool), 1, _File);

	if (bCS)
	{
		wstring strKey;
		LoadWStringFromFile(strKey, _File);
		m_UpdateCS = dynamic_cast<CParticleUpdateShader*>(CResMgr::GetInst()->FindRes<CComputeShader>(strKey).Get());
	}

	// ��ƼŬ �ɼ�
	fread(&m_iMaxCount, sizeof(UINT), 1, _File);
	fread(&m_iAliveCount, sizeof(UINT), 1, _File);
	fread(&m_vStartScale, sizeof(Vec4), 1, _File);
	fread(&m_vEndScale, sizeof(Vec4), 1, _File);
	fread(&m_vStartColor, sizeof(Vec4), 1, _File);
	fread(&m_vEndColor, sizeof(Vec4), 1, _File);
	fread(&m_vMinMaxSpeed, sizeof(Vec2), 1, _File);
	fread(&m_vMinMaxLifeTime, sizeof(Vec2), 1, _File);
	fread(&m_fSpawnRange, sizeof(float), 1, _File);
	fread(&m_fFrequency, sizeof(float), 1, _File);
	fread(&m_fAccTime, sizeof(float), 1, _File);
	fread(&m_WorldSpawn, sizeof(int), 1, _File);
}

