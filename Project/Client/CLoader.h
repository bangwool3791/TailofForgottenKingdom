#pragma once
#include "pch.h"
#include <Engine\CDevice.h>
class CLoader;
struct tData
{
	CLoader* pLoader;
	ID3D11Device* dev;
	ID3D11DeviceContext* context;
	CRITICAL_SECTION critical_section;
};
class CLoader 
{
public:
	explicit CLoader();
	~CLoader();

private:
	tData				m_tData;
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_CriticalSection;
	bool				m_isFinished = false;
	wstring m_strLoadingText;
public:
	UINT Loading_Fbx(UINT& iProgress);

	HRESULT Initialize();
	CRITICAL_SECTION& Get_CriticalSection() {
		return m_CriticalSection;
	}

	const wstring& Get_String() const {
		return m_strLoadingText;
	}

	bool Get_isFinished() const {
		return m_isFinished;
	}
};
