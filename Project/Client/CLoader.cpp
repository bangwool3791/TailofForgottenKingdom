#include "pch.h"
#include "CLoader.h"

#include <Engine\CFBXLoader.h>

UINT APIENTRY EntryFunction(void* pArg);

UINT APIENTRY EntryFunction(void* pArg)
{
	tData* Data = (tData*)pArg;

	EnterCriticalSection(&Data->critical_section);

	UINT iProgress = 0;

	Data->pLoader->Loading_Fbx(iProgress);

	LeaveCriticalSection(&Data->critical_section);

	return UINT();
}

CLoader::CLoader()
{
}

CLoader::~CLoader()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_tData.critical_section);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);
}

HRESULT CLoader::Initialize()
{
	m_tData.pLoader = this;
	m_tData.dev = DEVICE;
	m_tData.context = CONTEXT;
	memset(&m_tData.critical_section, 0, sizeof(CRITICAL_SECTION));

	InitializeCriticalSection(&m_tData.critical_section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, EntryFunction, &m_tData, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}


UINT CLoader::Loading_Fbx(UINT& iProgress)
{
	//CResMgr::GetInst()->LoadFBX(L"fbx\\House.fbx");
	//++iProgress;
	//CResMgr::GetInst()->LoadFBX(L"fbx\\Monster.fbx");
	//++iProgress;
	//CResMgr::GetInst()->LoadFBX(L"fbx\\rock1.fbx");
	m_isFinished = true;
	return UINT();
}
