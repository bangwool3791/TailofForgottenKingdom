#include "pch.h"
#include "CScriptMgr.h"

#include "CEngineBrushScript.h"
#include "CFrustumScript.h"
#include "CLevelCameraScript.h"
#include "CPlayerScript.h"
#include "CSecondCameraScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CEngineBrushScript");
	_vec.push_back(L"CFrustumScript");
	_vec.push_back(L"CLevelCameraScript");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CSecondCameraScript");
}


void CScriptMgr::GetScriptInfo(vector<string>& _vec)
{
	_vec.push_back("CEngineBrushScript");
	_vec.push_back("CFrustumScript");
	_vec.push_back("CLevelCameraScript");
	_vec.push_back("CPlayerScript");
	_vec.push_back("CSecondCameraScript");
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CEngineBrushScript" == _strScriptName)
		return new CEngineBrushScript;
	if (L"CFrustumScript" == _strScriptName)
		return new CFrustumScript;
	if (L"CLevelCameraScript" == _strScriptName)
		return new CLevelCameraScript;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CSecondCameraScript" == _strScriptName)
		return new CSecondCameraScript;
	return nullptr;
}

CScript * CScriptMgr::GetScript(const string& _strScriptName)
{
	if ("CEngineBrushScript" == _strScriptName)
		return new CEngineBrushScript;
	if ("CFrustumScript" == _strScriptName)
		return new CFrustumScript;
	if ("CLevelCameraScript" == _strScriptName)
		return new CLevelCameraScript;
	if ("CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if ("CSecondCameraScript" == _strScriptName)
		return new CSecondCameraScript;
	return nullptr;
}

CScript * CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::ENGINEBRUSHSCRIPT:
		return new CEngineBrushScript;
		break;
	case (UINT)SCRIPT_TYPE::FRUSTUMSCRIPT:
		return new CFrustumScript;
		break;
	case (UINT)SCRIPT_TYPE::LEVELCAMERASCRIPT:
		return new CLevelCameraScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::SECONDCAMERASCRIPT:
		return new CSecondCameraScript;
		break;
	}
	return nullptr;
}

const wchar_t * CScriptMgr::GetScriptWName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::ENGINEBRUSHSCRIPT:
		return L"CEngineBrushScript";
		break;

	case SCRIPT_TYPE::FRUSTUMSCRIPT:
		return L"CFrustumScript";
		break;

	case SCRIPT_TYPE::LEVELCAMERASCRIPT:
		return L"CLevelCameraScript";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::SECONDCAMERASCRIPT:
		return L"CSecondCameraScript";
		break;

	}
	return nullptr;
}

const char* CScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::ENGINEBRUSHSCRIPT:
		return "CEngineBrushScript";
		break;

	case SCRIPT_TYPE::FRUSTUMSCRIPT:
		return "CFrustumScript";
		break;

	case SCRIPT_TYPE::LEVELCAMERASCRIPT:
		return "CLevelCameraScript";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return "CPlayerScript";
		break;

	case SCRIPT_TYPE::SECONDCAMERASCRIPT:
		return "CSecondCameraScript";
		break;

	}
	return nullptr;
}