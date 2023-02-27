#include "pch.h"

#include "CLevelMgr.h"
#include "CResMgr.h"

#include "CDevice.h"
#include "CLevel.h"
#include "CGameObject.h"
#include "CAnimator2D.h"

#include "GlobalComponent.h"
#include "GlobalScript.h"

#include "CPaintShader.h"
#include "CCollisionMgr.h"

CLevelMgr::CLevelMgr()
	: m_pCurLevel(nullptr)
{
}

CLevelMgr::~CLevelMgr()
{
	if (nullptr != m_pCurLevel)
		delete m_pCurLevel;
}

void CLevelMgr::init()
{
	// Level �ϳ� �����ϱ�

	m_pCurLevel = new CLevel;
	m_pCurLevel->SetName(L"Level 0");
	// Layer �̸� ����

	// �ؽ��� �ε�
}

void CLevelMgr::progress()
{

	//m_pCurLevel->ClearLayer();
	//m_pCurLevel->tick();
	//m_pCurLevel->finaltick();
	 
	//GUI �ּ�
	m_pCurLevel->ClearLayer();
	if (LEVEL_STATE::PLAY == m_pCurLevel->GetState())
	{
		//Ű �Է��� �ȸԴ´�.
		m_pCurLevel->tick();
		m_pCurLevel->finaltick();
	}
	//GUI �ּ� ��
	//�������� �����ϴ� Component(Transform, register ����, Particle) finaltick����
}

void CLevelMgr::render()
{
	m_pCurLevel->render();
}

CGameObject* CLevelMgr::FindObjectByName(const wstring& _strName)
{
	return m_pCurLevel->FindObjectByName(_strName);
}

CGameObject* CLevelMgr::FindSelectedObject(const wstring& _strName)
{
	return m_pCurLevel->FindSelectedObject(_strName);
}

void CLevelMgr::ChangeLevel(CLevel* _NextLevel)
{
	if (m_pCurLevel)
	{
		delete m_pCurLevel;
	}

	m_pCurLevel = _NextLevel;
}

void CLevelMgr::ChangeLevelState(LEVEL_STATE _State)
{
	assert(!(m_pCurLevel->GetState() == _State));
	
	m_pCurLevel->SetState(_State);
	
	//[23.02.03]����
	//���� �ε� �ÿ��� Play�� ���� �ʴ´�.
	//���� ����
	//������ Play �ݺ� �� Ÿ�� ������ �̻�
	//if (LEVEL_STATE::PLAY == _State)
	//	m_pCurLevel->begin();
}

LEVEL_STATE CLevelMgr::GetLevelState()
{
	return m_pCurLevel->GetState();
}
