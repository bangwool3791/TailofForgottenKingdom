#pragma once
#include "CParticleSystem.h"

class CStructuredBuffer;

/*
* ���� ������Ʈ(����, �޽�)
* ����->������ ���̴�
* ��ǻƮ ���̴�
*/
class CSmoke :
	public CParticleSystem
{
public:
	virtual void finaltick()override;
	virtual void render()override;

public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;
	CLONE(CSmoke);

public:
	CSmoke();
	CSmoke(COMPONENT_TYPE _type);
	CSmoke(const CSmoke& _Rhs);
	virtual ~CSmoke();
};