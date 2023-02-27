#pragma once
#include "CParticleSystem.h"

class CStructuredBuffer;

/*
* ���� ������Ʈ(����, �޽�)
* ����->������ ���̴�
* ��ǻƮ ���̴�
*/
class CExplosion :
	public CParticleSystem
{
public:
	virtual void finaltick()override;
	virtual void render()override;

public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;
	CLONE(CExplosion);

public:
	CExplosion();
	CExplosion(COMPONENT_TYPE _type);
	CExplosion(const CExplosion& _Rhs);
	virtual ~CExplosion();
};