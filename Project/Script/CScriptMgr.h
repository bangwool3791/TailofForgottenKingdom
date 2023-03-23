#pragma once

#include <vector>
#include <string>

enum SCRIPT_TYPE
{
	ENGINEBRUSHSCRIPT,
	FRUSTUMSCRIPT,
	LEVELCAMERASCRIPT,
	PLAYERSCRIPT,
	SECONDCAMERASCRIPT,
	END,
};

using namespace std;

class CScript;

class CScriptMgr
{
public:
	static void GetScriptInfo(vector<wstring>& _vec);
	static void GetScriptInfo(vector<string>& _vec);
	static CScript * GetScript(const wstring& _strScriptName);
	static CScript * GetScript(const string& _strScriptName);
	static CScript * GetScript(UINT _iScriptType);
	static const wchar_t * GetScriptWName(CScript * _pScript);
	static const char* GetScriptName(CScript * _pScript);
};
