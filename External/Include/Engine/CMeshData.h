#pragma once
#include "CRes.h"

#include "Ptr.h"
#include "CMaterial.h"
#include "CMesh.h"


class CMeshData :
	public CRes
{
	Ptr<CMesh>				m_pMesh;
	/*
	* Mesh ������
	* �ε��� - ���͸���
	* 0  ~ 10, 1 ���͸���
	* 11 ~ 20, 2 ���͸���
	*/

	vector<Ptr<CMaterial>>	m_vecMtrl;

public:
	static CMeshData* LoadFromFBX(const wstring& _strFilePath);
	const vector<Ptr<CMaterial>>& GetMtrls() { return m_vecMtrl; }
	virtual void Save(const wstring& _strFilePath);
	virtual int Load(const wstring& _strFilePath);

	CGameObject* Instantiate();

	CLONE_ASSERT(CMeshData)
public:
	CMeshData(bool _bEngine = false);
	virtual ~CMeshData();
};

