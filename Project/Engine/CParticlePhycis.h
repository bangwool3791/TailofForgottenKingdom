#pragma once
#include "CParticleSystem.h"
#include "CParticleUpdateShader.h"

class CStructuredBuffer;

/*
* ���� ������Ʈ(����, �޽�)
* ����->������ ���̴�
* ��ǻƮ ���̴�
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