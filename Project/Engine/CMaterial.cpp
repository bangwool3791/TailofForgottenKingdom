#include "pch.h"

#include "CMaterial.h"
#include "CDevice.h"
#include "CGraphicsShader.h"
#include "CConstBuffer.h"

CMaterial::CMaterial()
	: CRes(RES_TYPE::MATERIAL, true)
	, m_tConst{}
	, m_arrTex{}
{

}

CMaterial::CMaterial(bool _bEngineRes)
	: CRes(RES_TYPE::MATERIAL, _bEngineRes)
	, m_tConst{}
	, m_arrTex{}
{

}

CMaterial::CMaterial(const CMaterial& _other)
	: CRes(_other)
	, m_tConst(_other.m_tConst)
	, m_pShader(_other.m_pShader)
{
	for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
	{
		m_arrTex[i] = _other.m_arrTex[i];
	}

	m_pMasterMtrl = const_cast<CMaterial*>(&_other);
}

CMaterial::~CMaterial()
{

}

void CMaterial::SetScalarParam(SCALAR_PARAM _eScalarType, void* _pData)
{
	switch (_eScalarType)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
		m_tConst.iArr[(UINT)_eScalarType] = *(int*)_pData;
		break;
	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
		m_tConst.fArr[(UINT)_eScalarType - FLOAT_0] = *(float*)_pData;
		break;
	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
		m_tConst.v2Arr[(UINT)_eScalarType - VEC2_0] = *(Vec2*)_pData;
		break;
	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
		m_tConst.v4Arr[(UINT)_eScalarType - VEC4_0] = *(Vec4*)_pData;
		break;
	case MAT_0:
	case MAT_1:
	case MAT_2:
	case MAT_3:
		m_tConst.matArr[(UINT)_eScalarType - MAT_0] = *(Matrix*)_pData;
		break;
	}
}

void CMaterial::GetScalarParam(SCALAR_PARAM _eScalarType, void* _pOut)
{
	switch (_eScalarType)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
		*(int*)_pOut = m_tConst.iArr[(UINT)_eScalarType];
		break;
	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
		*(float*)_pOut = m_tConst.fArr[(UINT)_eScalarType - FLOAT_0];
		break;
	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
		*(Vec2*)_pOut = m_tConst.v2Arr[(UINT)_eScalarType - VEC2_0];
		break;
	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
		*(Vec4*)_pOut = m_tConst.v4Arr[(UINT)_eScalarType - VEC4_0];
		break;
	case MAT_0:
	case MAT_1:
	case MAT_2:
	case MAT_3:
		*(Matrix*)_pOut = m_tConst.matArr[(UINT)_eScalarType - MAT_0];
		break;
	}
}

void CMaterial::UpdateData()
{
	for (UINT i{ 0 }; i < (UINT)TEX_END; ++i)
	{
		if (nullptr != m_arrTex[i])
		{
			m_arrTex[i]->UpdateData(i, ALL_STAGE);
			m_tConst.HasTex[i] = 1;
		}
		else
		{
			m_tConst.HasTex[i] = 0;
		}
	}

	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_tConst);
	pCB->UpdateData(PIPELINE_STAGE::ALL_STAGE);
	
	m_pShader->UpdateData();
}

void CMaterial::SetTexParam(TEX_PARAM _eTex, Ptr<CTexture> _pTex)
{
	m_arrTex[_eTex] = _pTex;
}

void CMaterial::Clear()
{
	for (UINT i{ 0 }; i < (UINT)TEX_END; ++i)
	{
		CTexture::Clear(i);
	}
}
void CMaterial::Save(const wstring& _strRelativePath)
{
	if (!CheckRelativePath(_strRelativePath))
	{
		MessageBox(nullptr, L"CMaterial Path Overlapped", L"Error", MB_OK);
		return;
	}

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	FILE* pFile = nullptr;

	if (!pFile)
		return;

	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	CRes::SaveKeyPath(pFile);

	SaveResourceRef(m_pShader, pFile);

	if (nullptr != m_pShader)
	{
		fwrite(&m_tConst, sizeof(tMtrlConst),1, pFile);

		for (UINT i{}; i < TEX_PARAM::TEX_END; ++i)
		{
			SaveResourceRef(m_arrTex[i], pFile);
		}
	}

	fclose(pFile);

	MessageBox(nullptr, L"CMaterial Save", L"Success", MB_OK);
}

int CMaterial::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;

	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	if (!pFile)
		return -1;

	CRes::LoadKeyPath(pFile);

	LoadResourceRef(m_pShader, pFile);

	if (nullptr != m_pShader)
	{
		fread(&m_tConst, sizeof(tMtrlConst), 1, pFile);

		for (UINT i{}; i < TEX_PARAM::TEX_END; ++i)
		{
			LoadResourceRef(m_arrTex[i], pFile);
		}
	}

	fclose(pFile);

	return  S_OK;
}

void CMaterial::SwapFile(const wstring _strRelativePath)
{
	m_strRelativePath = _strRelativePath;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	if (!pFile)
		return;

	CRes::SaveKeyPath(pFile);

	SaveResourceRef(m_pShader, pFile);

	if (nullptr != m_pShader)
	{
		fwrite(&m_tConst, sizeof(tMtrlConst), 1, pFile);

		for (UINT i{}; i < TEX_PARAM::TEX_END; ++i)
		{
			SaveResourceRef(m_arrTex[i], pFile);
		}
	}

	fclose(pFile);
}
