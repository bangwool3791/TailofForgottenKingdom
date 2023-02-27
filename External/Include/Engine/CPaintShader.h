#pragma once

#include "CComputeShader.h"

#include "Ptr.h"
#include "CTexture.h"

class CPaintShader :
	public CComputeShader
{
private:
	Ptr<CTexture> m_pTargetTex;

public :
	void SetTexture(Ptr<CTexture> _pTarget)
	{
		m_pTargetTex = _pTarget;
		m_Param.v2Arr[0] = Vec2(_pTarget->GetWidth(), _pTarget->GetHeight());
	}

	void SetColor(Vec4 _vColor) { m_Param.v4Arr[0] = _vColor; }

public :
	virtual void UpdateData() override;
	virtual void Clear() override;

public:
	CPaintShader();
	virtual ~CPaintShader();
};