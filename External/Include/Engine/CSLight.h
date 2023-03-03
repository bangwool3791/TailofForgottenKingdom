#pragma once
#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CSLight :
	public CComputeShader
{
private:
	float	m_fLumAverage;
	Ptr<CTexture> m_pTargetTex;
public:
	CStructuredBuffer* m_LightShareBuffer;
public:
	void SetTargetTex(Ptr<CTexture> _ptr) 
	{ 
		m_pTargetTex = _ptr; 
		m_Param.v2Arr[0] = Vec2(m_pTargetTex->GetWidth(), m_pTargetTex->GetHeight());
	}
public:
	virtual void UpdateData()override;
	virtual void Clear() override;

public:
	CSLight();
	virtual ~CSLight();
};