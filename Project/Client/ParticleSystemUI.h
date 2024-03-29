#pragma once
#include "ComponentUI.h"

#include <Engine\CParticleSystem.h>

class ParticleSystemUI final
	: public ComponentUI
{
private:
	CParticleSystem* m_pParticleSystem;

	int  m_iMaxCount;   // 파티클 최대 개수
	bool m_bPosInherit; // 오브젝트 좌표에 영향
	int  m_iAliveCount; // 이번 프레임에 생성(활성화) 될 파티클 수

	float m_fArrMinMaxTime[2];    // 파티클의 최소/최대 생명 시간
	float m_fArrMinMaxSpeed[2];   // 파티클의 최소/최대 속력
	Vec4  m_vArrStartEndColor[2]; // 파티클 초기/최종 색상
	Vec3  m_vArrStartEndScale[2]; // 파티클 초기/최종 크기
	int   m_iArrDirection[2];     // 파티클 방향

	float m_fRange; // 파티클 생성 범위
	float m_fTerm;  // 파티클 생성 간격

	bool m_bShaderUseDirection;

private:
	void TextureSelect(void* _pTextureName);
	void ShaderSelect(void* _pShaderName);

	void SetData();

public:
	void render_update() override;

public:
	ParticleSystemUI();
	virtual ~ParticleSystemUI();
};