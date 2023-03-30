#include "pch.h"
#include "CMeshData.h"

#include "CPathMgr.h"
#include "CResMgr.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"

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

	return pNewObj;
}

CMeshData* CMeshData::LoadFromFBX(const wstring& _strPath)
{
	wstring strFullPath = CPathMgr::GetInst()->GetContentPath();
	strFullPath += _strPath;

	CFBXLoader loader;
	loader.init();
	loader.LoadFbx(strFullPath);

	// �޽� ��������
	CMesh* pMesh = nullptr;
	pMesh = CMesh::CreateFromContainer(loader);
	pMesh->Read();
	// ResMgr �� �޽� ���
	wstring strMeshKey = L"mesh\\";
	strMeshKey += path(strFullPath).stem();
	strMeshKey += L".mesh";

	//���ҽ� �޽� ����
	CResMgr::GetInst()->AddRes<CMesh>(strMeshKey, pMesh);

	vector<Ptr<CMaterial>> vecMtrl;

	// ���׸��� ��������
	for (UINT i = 0; i < loader.GetContainer(0).vecMtrl.size(); ++i)
	{
		// ����ó�� (material �̸��� �Է� �ȵǾ����� ���� �ִ�.)
		/*
		* Loading �� Material ����
		* Contanier Mtrl vec ���� ��Ʈ�� ����
		* Res Load
		*/
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(loader.GetContainer(0).vecMtrl[i].strMtrlName);
		vecMtrl.push_back(pMtrl);
	}

	CMeshData* pMeshData = new CMeshData(true);
	pMeshData->m_pMesh = pMesh;
	pMeshData->m_vecMtrl = vecMtrl;

	return pMeshData;
}