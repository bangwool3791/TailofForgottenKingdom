#include "pch.h"
#include "CPrefab.h"

#include "CGameObject.h"

SAVE_GAMEOBJECT CPrefab::Save_GameObject_Func = nullptr;
LOAD_GAMEOBJECT CPrefab::Load_GameObject_Func = nullptr;

CPrefab::CPrefab()
	:CRes(RES_TYPE::PREFAB)
	, m_pProtoObj{nullptr}
{

}

CPrefab::CPrefab(bool _bEngine)
	:CRes(RES_TYPE::PREFAB, _bEngine)
	, m_pProtoObj{ nullptr }
{

}

CPrefab::CPrefab(CGameObject* _pProto)
	:CRes(RES_TYPE::PREFAB)
	, m_pProtoObj{_pProto}
{
	if (-1 != _pProto->GetLayerIndex())
	{
		m_pProtoObj = _pProto->Clone();
	}
}

CPrefab::CPrefab(CGameObject* _pProto, bool _bEngine)
	:CRes(RES_TYPE::PREFAB, _bEngine)
	, m_pProtoObj{ _pProto}
{
	if (-1 != _pProto->GetLayerIndex())
	{
		m_pProtoObj = _pProto->Clone();
	}
}


CPrefab::~CPrefab()
{
	if (m_pProtoObj)
		delete m_pProtoObj;
}

CGameObject* CPrefab::Instantiate() 
{
	if (m_pProtoObj)
		return m_pProtoObj->Clone();

	return nullptr;
}

void CPrefab::Save(const wstring& _strRelativePath)
{
	if (!CheckRelativePath(_strRelativePath))
		MessageBox(nullptr, L"CPrefab File Save", L"Failed", MB_OK);
	
	FILE* pFile = nullptr;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	if (!pFile)
		return;

	SaveKeyPath(pFile);

	Save_GameObject_Func(m_pProtoObj, pFile);

	fclose(pFile);
}

int CPrefab::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;

	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	if (!pFile)
		return -1;

	LoadKeyPath(pFile);

	m_pProtoObj = Load_GameObject_Func(pFile);

	fclose(pFile);

	return S_OK;
}

void CPrefab::Save(FILE* pFile)
{
	if (!pFile)
		return;

	SaveKeyPath(pFile);

	Save_GameObject_Func(m_pProtoObj, pFile);

	fclose(pFile);
}

int CPrefab::Load(FILE* pFile)
{
	if (!pFile)
		return -1;
	LoadKeyPath(pFile);

	m_pProtoObj = Load_GameObject_Func(pFile);

	return S_OK;
}

