#include "pch.h"
#include "CEngine.h"

#include "smallobjallocator.h"

#include "CPathMgr.h"
#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CResMgr.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"
#include "CFontMgr.h"
#include "CDevice.h"
#include "CEventMgr.h"
#include "CCollisionMgr.h"
#include "CSound.h"

#include "CAnimator2D.h"

extern std::unordered_map<std::string, ChunkGroup*>			g_map_pool;
CEngine::CEngine()
	: m_hMainWnd(nullptr)
{

}

CEngine::~CEngine()
{
	for (auto iter{ g_map_pool.begin() }; iter != g_map_pool.end(); ++iter)
	{
		delete iter->second;
	}
}

int CEngine::init(HWND _hWnd, UINT _iWidth, UINT _iHeight)
{
	m_hMainWnd = _hWnd;
	m_vResolution.x = (float)_iWidth;
	m_vResolution.y = (float)_iHeight;

	// ��Ʈ�� �ػ󵵸� �����ϱ� ���� ���� ������ ũ�� ���
	RECT rt = { 0, 0, (LONG)_iWidth , (LONG)_iHeight };
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);

	// ������ ũ�� ���� �� Show ����
	SetWindowPos(m_hMainWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);
	ShowWindow(m_hMainWnd, true);

	// Manager �ʱ�ȭ
	if (FAILED(CDevice::GetInst()->init(m_hMainWnd, m_vResolution)))
	{
		return E_FAIL;
	}

	CPathMgr::GetInst()->init();
	CKeyMgr::GetInst()->init();
	CTimeMgr::GetInst()->init();
	CResMgr::GetInst()->init();
	CRenderMgr::GetInst()->init();
	CLevelMgr::GetInst()->init();
	CFontMgr::GetInst()->init();
	return S_OK;
}

void CEngine::progress()
{
	tick();
	finaltick();
	render();
}

void CEngine::tick()
{
	// Manager ������Ʈ
	CResMgr::GetInst()->tick();
	//CSound::g_pFMOD->update();
	CTimeMgr::GetInst()->tick();
	CKeyMgr::GetInst()->tick();
	CRenderMgr::GetInst()->tick();
	CLevelMgr::GetInst()->progress();
	CCollisionMgr::GetInst()->tick();
}

void CEngine::finaltick()
{

}

void CEngine::render()
{
	CRenderMgr::GetInst()->render();

	CTimeMgr::GetInst()->render();
	/*
	* 1 Tick
	* Present
	* ����� ����Ʈ ���� ��ȯ�� �Ͼ��.
	* [Ŭ���̾�Ʈ] �׸��带 �׸���.
	* 2 Tick
	* Target Ŭ��� �Ͼ��.
	*/

	CEventMgr::GetInst()->tick();
}
