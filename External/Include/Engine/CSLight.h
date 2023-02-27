#pragma once
#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CSLight :
	public CComputeShader
{
public:
	CStructuredBuffer* m_LightShareBuffer;
public:
	void SetLightShareBuffer(CStructuredBuffer* _Buffer);
public:
	virtual void UpdateData()override;
	virtual void Clear() override;

public:
	CSLight();
	virtual ~CSLight();
};