#pragma once
#include "CParticleSystem.h"

class CStructuredBuffer;

/*
* ���� ������Ʈ(����, �޽�)
* ����->������ ���̴�
* ��ǻƮ ���̴�
*/
class CBlood :
	public CParticleSystem
{
public:
	virtual void finaltick()override;
	virtual void render()override;

public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;
	CLONE(CBlood);

public:
	CBlood();
	CBlood(COMPONENT_TYPE _type);
	CBlood(const CBlood& _Rhs);
	virtual ~CBlood();
};