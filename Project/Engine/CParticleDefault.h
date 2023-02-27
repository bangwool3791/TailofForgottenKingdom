#pragma once
#include "CParticleSystem.h"
#include "CParticleUpdateShader.h"

class CStructuredBuffer;

/*
* ���� ������Ʈ(����, �޽�)
* ����->������ ���̴�
* ��ǻƮ ���̴�
*/
class CParticleDefault :
	public CParticleSystem
{
public:
	virtual void finaltick()override;
	virtual void render()override;
public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;
	CLONE(CParticleDefault);

public:
	CParticleDefault();
	CParticleDefault(const CParticleDefault& _Rhs);
	virtual ~CParticleDefault();
};