#pragma once
#include "CParticleSystem.h"

class CStructuredBuffer;

/*
* ���� ������Ʈ(����, �޽�)
* ����->������ ���̴�
* ��ǻƮ ���̴�
*/
class CDust :
	public CParticleSystem
{
public:
	virtual void finaltick()override;
	virtual void render()override;

public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;
	CLONE(CDust);

public:
	CDust();
	CDust(COMPONENT_TYPE _type);
	CDust(const CDust& _Rhs);
	virtual ~CDust();
};