#pragma once
#include "CParticleSystem.h"
#include "CParticleUpdateShader.h"

class CStructuredBuffer;

/*
* 랜더 컴포넌트(재질, 메쉬)
* 재질->랜더용 셰이더
* 컴퓨트 셰이더
*/
class CParticlePhycis :
	public CParticleSystem
{
public:
	virtual void finaltick()override;
	virtual void render()override;

public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;
	CLONE(CParticlePhycis);

public:
	CParticlePhycis();
	CParticlePhycis(const CParticlePhycis& _Rhs);
	virtual ~CParticlePhycis();
};