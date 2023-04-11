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

	void SetMaterialCoefficient(Vec4 _vDiff, Vec4 _vSpec, Vec4 _vAmb, Vec4 _vEmis)
	{
		m_tConst.mtrl.vDiff = _vDiff;
		m_tConst.mtrl.vAmb = _vAmb;
		m_tConst.mtrl.vSpec = _vSpec;
		m_tConst.mtrl.vEmv = _vEmis;
	}

	void SetTexParam(TEX_PARAM _eTex, Ptr<CTexture> _pTex);
	Ptr<CTexture> GetTexParam(TEX_PARAM _eTexParam) { return m_arrTex[(UINT)_eTexParam]; }

	void SetAnim3D(bool _bTrue) { m_tConst.arrAnimData[0] = (int)_bTrue; }
	void SetBoneCount(int _iBoneCount) { m_tConst.arrAnimData[1] = _iBoneCount; }

	void UpdateData();
	void UpdateData_Inst();
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