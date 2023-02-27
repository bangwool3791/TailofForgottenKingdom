#pragma once

#include "pch.h"

class CLevel;
class CLayer;
class CGameObject;

class CSaveLoadMgr
	:public CSingleton<CSaveLoadMgr>
{
private:

public:
	void init();
public :
	void SaveLevel(CLevel* _Level, wstring _strRelativePath);
	CLevel* LoadLevel(const wstring& _strRelativePath);

	void SavePrefab(wstring _strRelativePath);
	void LoadPrefab(wstring _strRelativePath);
	static void SaveGameObject(CGameObject* _Object, FILE* _File);
	static CGameObject* LoadGameObject(FILE* _File);
private:
	CSaveLoadMgr();
	~CSaveLoadMgr();

	friend class CSingleton<CSaveLoadMgr>;
};