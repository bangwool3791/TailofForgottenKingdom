#pragma once
#include "CRenderComponent.h"
#include "CParticleUpdateShader.h"

class CStructuredBuffer;

/*
* 랜더 컴포넌트(재질, 메쉬)
* 재질->랜더용 셰이더
* 컴퓨트 셰이더
*/
class CParticleSystem :
	public CRenderComponent
{
protected:
	UINT						m_iMaxCount;
	UINT						m_iAliveCount;

	Vec4						m_vStartScale;
	Vec4						m_vEndScale;

	Vec4						m_vStartColor;
	Vec4						m_vEndColor;

	Vec2						m_vMinMaxSpeed;
	Vec2						m_vMinMaxLifeTime;

	float						m_fSpawnRange;

	float						m_fMinLifeTime;
	float						m_fMaxLifeTime;
	int							m_WorldSpawn;
	float						m_fAccTime;
	float						m_fFrequency;
	int							m_iType;

	tParticle					m_arrParticle[100];
	tParticleShare				m_share;
	/*
	* 복사 된 객체의 effect는 다를 수 있으므로 
	* 복사 생성자에서 버퍼 새로 할당해야한다.
	*/
	CStructuredBuffer*			m_ParticleBuffer;
	CStructuredBuffer*			m_ParticleShare;
	Ptr<CParticleUpdateShader>  m_UpdateCS;

public :
	void SetWorldSpawn(bool _bSet) { m_WorldSpawn = (int)_bSet; }
protected :
	virtual void finaltick()override;
	virtual void render()override;
	virtual void render_Instancing()override {}
	CLONE(CParticleSystem);
	
public:
	void SaveToFile(FILE* _File);
	void LoadFromFile(FILE* _File);
public :
	CParticleSystem();
	CParticleSystem(COMPONENT_TYPE _type);
	CParticleSystem(const CParticleSystem& _Rhs);
	virtual ~CParticleSystem();
};