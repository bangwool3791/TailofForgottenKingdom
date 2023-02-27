#pragma once
#include "CRes.h"

class CShader :
	public CRes
{
protected:
	ComPtr<ID3DBlob>		m_ErrBlob;
	vector<tScalarParam>	m_vecScalarParam;
	vector<tTextureParam>	m_vecTexParam;
public:
	void AddScalarParam(SCALAR_PARAM _eParam, const string& _name) { m_vecScalarParam.push_back(tScalarParam{ _eParam, _name }); }
	void AddTexureParam(TEX_PARAM _eParam, const string& _name) { m_vecTexParam.push_back(tTextureParam{ _eParam, _name }); }

	const vector<tScalarParam>& GetScalarParam() { return m_vecScalarParam; }
	const vector<tTextureParam>& GetTextureParam() { return m_vecTexParam; }

	virtual void Save(const wstring& _strRelativePath) {}
	virtual int Load(const wstring& _strFilePath) override { return S_OK; }

	CLONE_ASSERT(CShader);
public:
	CShader(RES_TYPE _eResType);
	virtual ~CShader();
};