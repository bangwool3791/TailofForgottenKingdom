#include "pch.h"
#include "CMeshData.h"

#include "CPathMgr.h"
#include "CResMgr.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CAnimator3D.h"

#include "CFBXLoader.h"

CMeshData::CMeshData(bool _bEngine)
	: CRes(RES_TYPE::MESHDATA, _bEngine)
{
}

CMeshData::~CMeshData()
{
}

CGameObject* CMeshData::Instantiate()
{
	CGameObject* pNewObj = new CGameObject;
	pNewObj->AddComponent(new CTransform);
	pNewObj->AddComponent(new CMeshRender);

	pNewObj->MeshRender()->SetMesh(m_pMesh);

	for (UINT i = 0; i < m_vecMtrl.size(); ++i)
	{
		pNewObj->MeshRender()->SetSharedMaterial(m_vecMtrl[i], i);
	}

	// Animation 파트 추가
	if (false == m_pMesh->IsAnimMesh())
		return pNewObj;

	CAnimator3D* pAnimator = new CAnimator3D;
	pNewObj->AddComponent(pAnimator);
	pAnimator->SetBones(m_pMesh->GetBones());
	pAnimator->SetAnimClip(m_pMesh->GetAnimClip());
	pAnimator->SetMeshData(this);

	return pNewObj;
}

CMeshData* CMeshData::LoadFromFBX(const wstring& _strPath, UINT idx)
{
	wstring strFullPath = CPathMgr::GetInst()->GetContentPath();
	strFullPath += _strPath;

	CFBXLoader loader;
	loader.init();
	loader.LoadFbx(strFullPath);

	// 메쉬 가져오기
	CMesh* pMesh = nullptr;
	pMesh = CMesh::CreateFromContainer(loader, idx);
	//멀티스레드에서 Read시 문제 발생
	pMesh->Read();

	// ResMgr 에 메쉬 등록
	if (nullptr != pMesh)
	{
		wstring strMeshKey = L"mesh\\";
		strMeshKey += path(strFullPath).stem();
		strMeshKey += std::to_wstring(idx) + L".mesh";
		CResMgr::GetInst()->AddRes<CMesh>(strMeshKey, pMesh);

		// 메시를 실제 파일로 저장
		// Rev 105 AddRes only -> Save
		pMesh->Save(strMeshKey);
	}


	vector<Ptr<CMaterial>> vecMtrl;

	// 메테리얼 가져오기

	for (UINT i = 0; i < loader.GetContainer(idx).vecMtrl.size(); ++i)
	{
		// 예외처리 (material 이름이 입력 안되어있을 수도 있다.)
		/*
		* Loading 시 Material 생성
		* Contanier Mtrl vec 에서 스트링 저장
		* Res Load
		*/
		/*
		Loading 된 Material을 Find
		*/
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(loader.GetContainer(idx).vecMtrl[i].strMtrlName);
		assert(pMtrl.Get());
		vecMtrl.push_back(pMtrl);
	}

	CMeshData* pMeshData = new CMeshData(true);
	pMeshData->m_pMesh = pMesh;
	pMeshData->m_vecMtrl = vecMtrl;

	return pMeshData;
}

CMeshData* CMeshData::LoadFromFBX(const wstring& _strPath)
{
	wstring strFullPath = CPathMgr::GetInst()->GetContentPath();
	strFullPath += _strPath;

	CFBXLoader loader;
	loader.init();
	loader.LoadFbx(strFullPath);

	// 메쉬 가져오기
	CMesh* pMesh = nullptr;
	pMesh = CMesh::CreateFromContainer(loader);
	//멀티스레드에서 Read시 문제 발생
	//pMesh->ReadByConsole();

	// ResMgr 에 메쉬 등록
	if (nullptr != pMesh)
	{
		wstring strMeshKey = L"mesh\\";
		strMeshKey += path(strFullPath).stem();
		strMeshKey += L".mesh";
		CResMgr::GetInst()->AddRes<CMesh>(strMeshKey, pMesh);

		// 메시를 실제 파일로 저장
		// Rev 105 AddRes only -> Save
		pMesh->Save(strMeshKey);
	}


	vector<Ptr<CMaterial>> vecMtrl;

	// 메테리얼 가져오기

	for (UINT i = 0; i < loader.GetContainer(0).vecMtrl.size(); ++i)
	{
		// 예외처리 (material 이름이 입력 안되어있을 수도 있다.)
		/*
		* Loading 시 Material 생성
		* Contanier Mtrl vec 에서 스트링 저장
		* Res Load
		*/
		/*
		Loading 된 Material을 Find
		*/
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(loader.GetContainer(0).vecMtrl[i].strMtrlName);
		assert(pMtrl.Get());
		vecMtrl.push_back(pMtrl);
	}

	CMeshData* pMeshData = new CMeshData(true);
	pMeshData->m_pMesh = pMesh;
	pMeshData->m_vecMtrl = vecMtrl;

	return pMeshData;
}
/*
* When save MeshData, save mesh key, string and material key string, size
*/
void CMeshData::Save(const wstring& _strRelativePath)
{
	SetRelativePath(_strRelativePath);

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + _strRelativePath;

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");
	assert(pFile);

	// Mesh Key 저장	
	// Mesh Path 저장
	SaveResourceRef<CMesh>(m_pMesh, pFile);

	// material size 저장
	UINT iMtrlCount = (UINT)m_vecMtrl.size();
	fwrite(&iMtrlCount, sizeof(UINT), 1, pFile);

	UINT i = 0;
	wstring strMtrlPath = CPathMgr::GetInst()->GetContentPath();
	strMtrlPath += L"material\\";

	for (; i < iMtrlCount; ++i)
	{
		if (nullptr == m_vecMtrl[i])
			continue;

		// Material 인덱스, Key, Path 저장
		fwrite(&i, sizeof(UINT), 1, pFile);
		SaveResourceRef<CMaterial>(m_vecMtrl[i], pFile);
	}

	i = -1; // 마감 값
	fwrite(&i, sizeof(UINT), 1, pFile);

	fclose(pFile);
}

int CMeshData::Load(const wstring& _strFilePath)
{
	FILE* pFile = NULL;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	assert(pFile);

	// Mesh Load
	LoadResourceRef<CMesh>(m_pMesh, pFile);
	assert(m_pMesh.Get());

	// material 정보 읽기
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(UINT), 1, pFile);

	m_vecMtrl.resize(iMtrlCount);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		UINT idx = -1;
		fread(&idx, 4, 1, pFile);
		if (idx == -1)
			break;

		wstring strKey, strPath;

		Ptr<CMaterial> pMtrl;
		LoadResourceRef<CMaterial>(pMtrl, pFile);

		m_vecMtrl[i] = pMtrl;
	}

	fclose(pFile);

	return S_OK;
}