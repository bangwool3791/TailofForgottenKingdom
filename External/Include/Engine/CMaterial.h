#pragma once
#include "CRes.h"
#include "CTexture.h"
#include "CGraphicsShader.h"

class CMaterial :
	public CRes
{
private:
	tMtrlConst						m_tConst;
	Ptr<CTexture>					m_arrTex[TEX_END];
	Ptr<CGraphicsShader>			m_pShader;

	Ptr<CMaterial>					m_pMasterMtrl;
public :
	const tMtrlConst& GetMaterial() { return m_tConst; }
	void SetScalarParam(SCALAR_PARAM _eScalarType, void* _pData);
	void GetScalarParam(SCALAR_PARAM _eScalarType, void* _pOut);
	void SetShader(Ptr<CGraphicsShader> _pShader) { m_pShader = _pShader; }
	Ptr<CGraphicsShader> GetShader(){return m_pShader;}

	void SetTexParam(TEX_PARAM _eTex, Ptr<CTexture> _pTex);
	Ptr<CTexture> GetTexParam(TEX_PARAM _eTexParam) { return m_arrTex[(UINT)_eTexParam]; }

	void UpdateData();
	static void Clear();

	virtual void Save(const wstring& _strRelativePath);
	virtual int Load(const wstring& _strFilePath);
	void SwapFile(const wstring _strRelativePath);
	CLONE(CMaterial)
public :
	CMaterial();
	CMaterial(bool _bEngineRes);
	CMaterial(const CMaterial& _other);
	virtual ~CMaterial();
};