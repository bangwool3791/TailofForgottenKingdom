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

	void SavePrefab();
	void LoadPrefab();
	static void SaveGameObject(CGameObject* _Object, FILE* _File);
	static CGameObject* LoadGameObject(FILE* _File);

	void LoadNaviMesh(const char* path);
private:
	CSaveLoadMgr();
	~CSaveLoadMgr();

	friend class CSingleton<CSaveLoadMgr>;
};