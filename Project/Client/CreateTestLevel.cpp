#include "pch.h"

#include "CreateTestLevel.h"
#include "CSaveLoadMgr.h"

#include <Engine\CPrefab.h>
#include <Engine\CResMgr.h>

#include <Engine\CCamera.h>
#include <Engine\CTransform.h>
#include <Engine\CGameObject.h>
#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>

#include <Engine\CCollisionMgr.h>

#include <Script\CPlayerScript.h>
#include <Script\CFrustumScript.h>

#include <Script\CLevelCameraScript.h>
#include <Script\CSecondCameraScript.h>

void CreateDefaultPrefab()
{
	CGameObject* pObject = new CGameObject;
	pObject->SetName(L"Missile");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->Transform()->SetRelativeScale(Vec3{ 50.f, 50.f, 1.f });
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DMtrl"));
	pObject->MeshRender()->GetSharedMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\Player.bmp"));

	CResMgr::GetInst()->AddRes<CPrefab>(L"MissilePrefab", new CPrefab(pObject, true));

	pObject = new CGameObject;
	pObject->SetName(L"MouseObject");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider2D);
	CResMgr::GetInst()->AddRes<CPrefab>(L"MousePrefab", new CPrefab(pObject, true));
}

void CreateTestLelvel()
{
	//CGameObject* pGameObect{};

	//CreateDefaultPrefab();

	//CLevel* Level = CSaveLoadMgr::GetInst()->LoadLevel(L"level\\Test.lv");

	//Level->GetLayer(0)->SetName(L"Terrain");
	//Level->GetLayer(1)->SetName(L"Player");
	//Level->GetLayer(2)->SetName(L"PlayerProjecttile");
	//Level->GetLayer(3)->SetName(L"Monster");
	//Level->GetLayer(4)->SetName(L"MonsterProjecttile");

	////지형은 매시만 로드하면 됨.
	//pGameObect = new CGameObject;
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
	//Level->GetLayer(L"Terrain")->AddGameObject(pGameObect);

	//CLevelMgr::GetInst()->ChangeLevel(Level);
	//Level->begin();
	//return;

	//주석 경계 
	CreateDefaultPrefab();

	CLevel* pLevel = new CLevel;
	pLevel->SetName(L"Level");
#if true
	pLevel->GetLayer(0)->SetName(L"Terrain");
	//pLevel->GetLayer(1)->SetName(L"Player");
	//pLevel->GetLayer(2)->SetName(L"PlayerProjecttile");
	//pLevel->GetLayer(3)->SetName(L"Monster");
	//pLevel->GetLayer(4)->SetName(L"MonsterProjecttile");

	// Camera Object 추가
	CGameObject* pCamObj = new CGameObject;
	pCamObj->SetName(L"MainCamera");

	pCamObj->AddComponent(new CTransform);
	pCamObj->AddComponent(new CCamera);
	pCamObj->AddComponent(new CMeshRender);
	pCamObj->AddComponent(new CCollider3D);
	pCamObj->AddComponent(new CFrustumScript);
	pCamObj->AddComponent(new CLevelCameraScript);

	pCamObj->Transform()->SetRelativePos(Vec3(0.f, 0.f, -1000.f));
	pCamObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"FrustumMesh"));
	pCamObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));

	pCamObj->Camera()->SetProjType(PERSPECTIVE);
	pCamObj->Camera()->SetLayerMaskZero();
	pLevel->AddGameObject(pCamObj, 0);

	pCamObj = new CGameObject;
	pCamObj->SetName(L"SecendCamera");

	pCamObj->AddComponent(new CTransform);
	pCamObj->AddComponent(new CCamera);
	pCamObj->AddComponent(new CSecondCameraScript);

	pCamObj->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
	pCamObj->Camera()->SetProjType(PERSPECTIVE);
	pCamObj->Camera()->SetLayerMask(1);
	pLevel->AddGameObject(pCamObj, 0);

	// Directional Light 추가
	CGameObject* pDirLight = new CGameObject;
	pDirLight->SetName(L"DirectionalLight");
	
	pDirLight->AddComponent(new CTransform);
	pDirLight->AddComponent(new CLight3D);
	
	pDirLight->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
	pDirLight->Transform()->SetRelativeRotation(XM_PI / 2.f, 0.f, 0.f);
	pDirLight->Light3D()->SetLightColor(Vec3(0.5f, 0.5f, 0.5f));
	pDirLight->Light3D()->SetLightSpecular(Vec3(0.4f, 0.4f, 0.4f));
	pDirLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));
	pDirLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	//
	pLevel->AddGameObject(pDirLight, 1);
	// Point Light 추가
	//CGameObject* pPointLight = new CGameObject;
	//pPointLight->SetName(L"PointLight");

	//pPointLight->AddComponent(new CTransform);
	//pPointLight->AddComponent(new CLight3D);

	//pPointLight->Transform()->SetRelativePos(0.f, -200.f, 400.f);
	//pPointLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	//pPointLight->Light3D()->SetLightSpecular(Vec3(0.f, 0.f, 0.f));
	//pPointLight->Light3D()->SetLightAmbient(Vec3(0.f, 0.f, 0.f));
	//pPointLight->Light3D()->SetRadius(400.f);
	//pPointLight->Light3D()->SetLightType(LIGHT_TYPE::POINT2);

	//Instantiate(pPointLight, Vec3(0.f, -200.f, 400.f), 0);

	CGameObject* pSpotLight = new CGameObject;
	pSpotLight->SetName(L"SpotLight");

	pSpotLight->AddComponent(new CTransform);
	pSpotLight->AddComponent(new CLight3D);

	float r = 500.f;
	pSpotLight->Transform()->SetRelativePos(0.f, -100.f, 400.f);
	pSpotLight->Transform()->SetRelativeScale(r * sinf(XM_PI * 22.5f / 180.f) * 2.f, r * cosf(XM_PI * 22.5f / 180.f), r * sinf(XM_PI * 22.5f / 180.f) * 2.f);
	pSpotLight->Light3D()->SetLightColor(Vec3(0.5f, 0.5f, 0.5f));
	pSpotLight->Light3D()->SetLightSpecular(Vec3(0.f, 0.f, 0.f));
	pSpotLight->Light3D()->SetLightAmbient(Vec3(0.f, 0.f, 0.f));
	pSpotLight->Light3D()->SetRadius(r);
	pSpotLight->Light3D()->SetAngle(XM_PI / 4.f);
	pSpotLight->Light3D()->SetLightType(LIGHT_TYPE::SPOT);

	pLevel->AddGameObject(pSpotLight, 1);
	// GameObject 초기화
	CGameObject* pObject = nullptr;

#ifdef false
	pObject = new CGameObject;
	pObject->SetName(L"Sphere");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider3D);
	pObject->AddComponent(new CPlayerScript);

	pObject->Transform()->SetRelativePos(Vec3(0.f, 500.f, 400.f));
	pObject->Transform()->SetRelativeScale(Vec3(256.f, 256.f, 256.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DDeferredMtrl"));
	pObject->MeshRender()->GetCurMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	pObject->MeshRender()->GetCurMaterial()->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	pLevel->AddGameObject(pObject, 1);

	//DiffuseTargetTex

	pObject = new CGameObject;
	pObject->SetName(L"Plane");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider3D);
	pObject->AddComponent(new CPlayerScript);

	pObject->Transform()->SetRelativePos(Vec3(0.f, -200.f, 400.f));
	pObject->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1000.f));
	pObject->Transform()->SetRelativeRotation(Vec3(XM_PI / 2.f, 0.f, 0.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DDeferredMtrl"));
	pObject->MeshRender()->GetCurMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	pObject->MeshRender()->GetCurMaterial()->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	pLevel->AddGameObject(pObject, 1);

	pObject = new CGameObject;
	pObject->SetName(L"Diffuse");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->Transform()->SetRelativePos(Vec3(-200.f, -200.f, 400.f));
	pObject->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));
	pObject->Transform()->SetRelativeRotation(Vec3(XM_PI / 2.f, 0.f, 0.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DMtrl"));
	pObject->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"BlurTargetTex"));
	pLevel->AddGameObject(pObject, 1);

	pObject = new CGameObject;
	pObject->SetName(L"Decal");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CDecal);

	pObject->Transform()->SetRelativePos(Vec3(0.f, -200.f, 400.f));
	pObject->Transform()->SetRelativeScale(Vec3(250.f, 250.f, 250.f));

	pObject->Decal()->SetDecalTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\MagicCircle.png"));
	pObject->Decal()->SetDefaultLit(true);
	pLevel->AddGameObject(pObject, 1);
#endif


	// SkyBox 추가
	CGameObject* pSkyBox = new CGameObject;
	pSkyBox->SetName(L"SkyBox");

	pSkyBox->AddComponent(new CTransform);
	pSkyBox->AddComponent(new CSkyBox);

	pSkyBox->Transform()->SetRelativePos(Vec3(0.f, 500.f, 400.f));
	pSkyBox->Transform()->SetRelativeScale(300.f, 300.f, 300.f);
	pSkyBox->SkyBox()->SetSkyBoxTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\SkyWater.dds"));
	pSkyBox->SkyBox()->SetType(SKYBOX_TYPE::CUBE);
	pLevel->AddGameObject(pSkyBox, 1);

	// LandScape 추가
	CGameObject* pLandScape = new CGameObject;
	pLandScape->SetName(L"LandScape");

	pLandScape->AddComponent(new CTransform);
	pLandScape->AddComponent(new CLandScape);

	pLandScape->Transform()->SetRelativeScale(100.f, 100.f, 100.f);
	pLandScape->LandScape()->SetFaceCount(16, 16);
	pLandScape->LandScape()->SetFrustumCulling(false);

	pLevel->AddGameObject(pLandScape, 1);


	CCollisionMgr::GetInst()->CollisionLayerCheck(0, 0);
	CCollisionMgr::GetInst()->CollisionLayerCheck(1, 1);
	CLevelMgr::GetInst()->ChangeLevel(pLevel);
	pLevel->begin();
#endif
}
