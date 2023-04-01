#pragma once
#include "pch.h"

class CLoader 
{
private:
	explicit CLoader();
	~CLoader();

private:
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_CriticalSection;
	bool				m_isFinished = false;
	wstring m_strLoadingText;
public:
	UINT Loading_Fbx();

	HRESULT Initialize();
	CRITICAL_SECTION Get_CriticalSection() const {
		return m_CriticalSection;
	}

	const wstring& Get_String() const {
		return m_strLoadingText;
	}

	bool Get_isFinished() const {
		return m_isFinished;
	}
};
