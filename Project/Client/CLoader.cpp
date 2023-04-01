#include "pch.h"
#include "CLoader.h"

#include <Engine\CFBXLoader.h>

UINT APIENTRY EntryFunction(void* pArg);

CLoader::CLoader()
{
}

CLoader::~CLoader()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);
}

HRESULT CLoader::Initialize()
{
	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, EntryFunction, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

UINT APIENTRY EntryFunction(void* pArg)
{
	//CLoader* pLoader = (CLoader*)pArg;

	//EnterCriticalSection(&pLoader->Get_CriticalSection());

	//UINT iProgress = 0;


	//LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return UINT();
}

UINT CLoader::Loading_Fbx()
{
	CResMgr::GetInst()->LoadFBX(L"fbx\\House.fbx");
	CResMgr::GetInst()->LoadFBX(L"fbx\\Monster.fbx");
	CResMgr::GetInst()->LoadFBX(L"fbx\\rock1.fbx");
	m_isFinished = true;
	return UINT();
}
