#pragma once
#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CShadowMapShader :
	public CComputeShader
{
private:
	float	m_fLumAverage;
	Ptr<CTexture> m_pShwdowMapTex;
	Ptr<CTexture> m_pShadowValidTex;
public:
	void SetShadowValidTex(Ptr<CTexture> _ptr)
	{
		m_pShadowValidTex = _ptr;
	}
	void SetShwdowMapTex(Ptr<CTexture> _ptr)
	{
		m_pShwdowMapTex = _ptr;
	}
public:
	virtual void UpdateData()override;
	virtual void Clear() override;

public:
	CShadowMapShader();
	virtual ~CShadowMapShader();
};