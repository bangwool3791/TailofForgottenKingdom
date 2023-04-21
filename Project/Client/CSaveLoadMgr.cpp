#include "CSaveLoadMgr.h"

#include <Engine\CLevel.h>

#include <Engine\CCollisionMgr.h>
#include <Engine\CComponent.h>
#include <Engine\CGameObject.h>
#include <Engine\CScript.h>
#include <Engine\CPrefab.h>
#include <Script\CScriptMgr.h>

CSaveLoadMgr::CSaveLoadMgr()
{

}

CSaveLoadMgr::~CSaveLoadMgr()
{

}

void CSaveLoadMgr::init()
{
	CPrefab::Save_GameObject_Func = &CSaveLoadMgr::SaveGameObject;
	CPrefab::Load_GameObject_Func = &CSaveLoadMgr::LoadGameObject;
}

void CSaveLoadMgr::SaveLevel(CLevel* _Level, wstring _strRelativePath)
{
	//입력 레벨이 nullptr이면 assert
	assert(nullptr != _Level);

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;
	
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");
	if (!pFile)
		return;
	//레벨 이름 저장
	_Level->SaveToFile(pFile);

	for (UINT i{}; i < MAX_LAYER; ++i)
	{
		//레이어 이름 저장
		CLayer* pLayer = _Level->GetLayer(i);
		pLayer->SaveToFile(pFile);

		const vector<CGameObject*>& vecParentObj = pLayer->GetParentObjects();
		size_t iObjectCount = vecParentObj.size();
		fwrite(&iObjectCount, sizeof(size_t), 1, pFile);

		for (size_t j{}; j < vecParentObj.size(); ++j)
		{
			SaveGameObject(vecParentObj[j], pFile);
		}
	}

	CCollisionMgr::GetInst()->SaveToFile(pFile);
	fclose(pFile);
}

void CSaveLoadMgr::SaveGameObject(CGameObject* _Object, FILE* _File)
{
	//오브젝트 이름 저장
	_Object->SaveToFile(_File);

	for (UINT i{}; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		CComponent* pComponent = _Object->GetComponent((COMPONENT_TYPE)i);
		if (nullptr != pComponent)
		{
			pComponent->SaveToFile(_File);
		}
	}

	COMPONENT_TYPE ComponentEnd = COMPONENT_TYPE::END;
	fwrite(&ComponentEnd, sizeof(UINT), 1, _File);

	const vector<CScript*>& vecScript = _Object->GetScripts();
	size_t iScriptCount = vecScript.size();
	fwrite(&iScriptCount, sizeof(size_t), 1, _File);

	for (size_t i{}; i < vecScript.size(); ++i)
	{
		SaveWStringToFile(CScriptMgr::GetScriptWName(vecScript[i]), _File);
		vecScript[i]->SaveToFile(_File);
	}

	const vector<CGameObject*> vecChild = _Object->GetChilds();

	size_t iChildCount = vecChild.size();
	fwrite(&iChildCount, sizeof(size_t), 1, _File);

	for (size_t i{}; i < iChildCount; ++i)
	{
		SaveGameObject(vecChild[i], _File);
	}
}

CLevel* CSaveLoadMgr::LoadLevel(const wstring& _strRelativePath)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	if (!pFile)
		return nullptr;

	CLevel* pLevel = new CLevel;
	pLevel->LoadFromFile(pFile);

	for (UINT i{}; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pLevel->GetLayer(i);
		pLayer->LoadFromFile(pFile);

		size_t iObjectCount = 0;
		fread(&iObjectCount, sizeof(size_t), 1, pFile);

		for (size_t j{}; j < iObjectCount; ++j)
		{
			CGameObject* pObject = LoadGameObject(pFile);
			pLayer->AddGameObject(pObject);
		}
	}

	CCollisionMgr::GetInst()->LoadToFile(pFile);

	fclose(pFile);

	////지형은 매시만 로드하면 됨.
	//CGameObject* pGameObect = new CGameObject;
	//pGameObect->AddComponent(new CTransform);
	//pGameObect->AddComponent(new CTerrain);
	//pGameObect->SetName(L"LevelTerrain");

	//pGameObect->Transform()->SetRelativePos(0.f, 0.f, 0.f);
	//pGameObect->Transform()->SetRelativeScale(1.f, 1.f, 1.f);
	//pGameObect->begin();

	//for (UINT i{}; i < TEX_32 + 1; ++i)
	//{
	//	wstring str = L"texture\\Terrain\\Tile\\Tile";
	//	str += std::to_wstring(i);
	//	str += L".png";
	//	pGameObect->Terrain()->SetTileAtlas(CResMgr::GetInst()->FindRes<CTexture>(str));
	//}

	//pGameObect->Terrain()->GetMesh()->Load(L"Terrain\\Terrain.dat");
	//pLevel->GetLayer(L"Terrain")->AddGameObject(pGameObect);

	//pGameObect = new CGameObject;
	//pGameObect->AddComponent(new CTransform);
	//pGameObect->AddComponent(new CTileMap);
	//pGameObect->SetName(L"LevelTile");

	//pGameObect->Transform()->SetRelativePos(0.f, 0.f, 0.f);
	//pGameObect->Transform()->SetRelativeScale(1.f, 1.f, 1.f);
	//pGameObect->begin();

	//pLevel->GetLayer(L"Terrain")->AddGameObject(pGameObect);
	return pLevel;
}

CGameObject* CSaveLoadMgr::LoadGameObject(FILE* _File)
{
	CGameObject* pObject = new CGameObject;

	pObject->LoadFromFile(_File);

	bool bProgess = true;

	while (true)
	{
		COMPONENT_TYPE type = COMPONENT_TYPE::END;
		fread(&type, sizeof(UINT), 1, _File);

		CComponent* pComponent = nullptr;

		switch (type)
		{
		case COMPONENT_TYPE::TRANSFORM:
			pComponent = new CTransform;
			break;
		case COMPONENT_TYPE::CAMERA:
			pComponent = new CCamera;
			break;
		case COMPONENT_TYPE::COLLIDER2D:
			pComponent = new CCollider2D;
			break;
		case COMPONENT_TYPE::COLLIDER3D:
			pComponent = new CCollider3D;
			break;
		case COMPONENT_TYPE::ANIMATOR2D:
			pComponent = new CAnimator2D;
			break;
		case COMPONENT_TYPE::ANIMATOR3D:
			pComponent = new CAnimator3D;
			break;
		case COMPONENT_TYPE::LIGHT2D:
			pComponent = new CLight2D;
			break;
		case COMPONENT_TYPE::LIGHT3D:
			pComponent = new CLight3D;
			break;
		case COMPONENT_TYPE::MESHRENDER:
			pComponent = new CMeshRender;
			break;
		case COMPONENT_TYPE::PARTICLESYSTEM:
			pComponent = new CParticleSystem;
			break;
		case COMPONENT_TYPE::SKYBOX:
			pComponent = new CSkyBox;
			break;
		case COMPONENT_TYPE::DECAL:
			pComponent = new CDecal;
			break;
		case COMPONENT_TYPE::LANDSCAPE:
			pComponent = new CLandScape;
			break;
		case COMPONENT_TYPE::WAVERENDERER:
			pComponent = new CWaveRenderer;
			break;
		case COMPONENT_TYPE::PHYSX:
			pComponent = new CPhysXComponent;
			break;
		default:
			bProgess = false;
			break;
		}

		if (!bProgess)
			break;


		pObject->AddComponent(pComponent);

		pComponent->LoadFromFile(_File);

	}
	size_t iScriptCount = 0;
	fread(&iScriptCount, sizeof(size_t), 1, _File);

	for (size_t i{}; i < iScriptCount; ++i)
	{
		wstring strScriptName;
		LoadWStringFromFile(strScriptName, _File);
		CScript* pNewScript = CScriptMgr::GetScript(strScriptName);
		pNewScript->LoadFromFile(_File);
		pObject->AddComponent(pNewScript);
	}

	size_t iChildCount = 0;
	fread(&iChildCount, sizeof(size_t), 1, _File);

	for (size_t i{}; i < iChildCount; ++i)
	{
		CGameObject* pChild = LoadGameObject(_File);
		pObject->AddChild(pChild);
	}
	return pObject;
}

void CSaveLoadMgr::SavePrefab()
{
	wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
	wstring strFilePath = strContentPath + L"prefab\\PrefabCount.dat";

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	if (!pFile)
		return;

	decltype(CResMgr::GetInst()->GetResource(RES_TYPE::PREFAB)) map = CResMgr::GetInst()->GetResource(RES_TYPE::PREFAB);

	size_t iPrefabCount = 0;

	for (auto iter{ map.begin() }; iter != map.end(); ++iter)
	{
		if (!iter->second->IsEngineRes())
			++iPrefabCount;
	}

	fwrite(&iPrefabCount, sizeof(size_t), 1, pFile);

	fclose(pFile);

	for (auto iter{ map.begin() }; iter != map.end(); ++iter)
	{
		if (!iter->second->IsEngineRes())
		{
			wchar_t sz_data[255] = { L"prefab\\" };
			wstring temp = lstrcat(sz_data, iter->second->GetKey().data());
			lstrcpy(sz_data, temp.data());
			wstring wstrRelativePath = lstrcat(sz_data, L".pref");
			iter->second->Save(wstrRelativePath);
		}
	}
}

void CSaveLoadMgr::LoadPrefab()
{

	wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
	wstring strFilePath = strContentPath + L"PrefabCount.dat";

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	if (pFile)
	{
		size_t iPrefabCount = 0;
		fread(&iPrefabCount, sizeof(size_t), 1, pFile);
		fclose(pFile);
	}


	decltype(CResMgr::GetInst()->GetResourceRef(RES_TYPE::PREFAB)) map = CResMgr::GetInst()->GetResourceRef(RES_TYPE::PREFAB);


	for (const auto& entry : fs::directory_iterator(strContentPath + L"prefab\\"))
	{
		if(std::string::npos == entry.path().string().find(".pref"))
			continue;

		if (entry.path() == strFilePath)
			continue;

		CPrefab* pPrefab = new CPrefab(false);

		pPrefab->Load(entry.path());

		auto iter = map.find(pPrefab->GetKey());

		if (iter != map.end())
		{
			iter->second = nullptr;
			map.erase(iter);
		}

		map.emplace(make_pair(pPrefab->GetKey(), pPrefab));
	}

	//FILE* pFile = nullptr;
	//_wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	//decltype(CResMgr::GetInst()->GetResource(RES_TYPE::PREFAB)) map = CResMgr::GetInst()->GetResource(RES_TYPE::PREFAB);

	//for (auto iter{ map.begin() }; iter != map.end(); ++iter)
	//{
	//	iter->second->Save(_strRelativePath);
	//}
}

void CSaveLoadMgr::LoadNaviMesh(const char* path)
{

}