#include "pch.h"
#include "CSLight.h"

#include "CDevice.h"
#include "CStructuredBuffer.h"

CSLight::CSLight()
	: CComputeShader(1024, 900, 2)
	, m_LightShareBuffer(nullptr)
{
}

CSLight::~CSLight()
{
}


void CSLight::SetLightShareBuffer(CStructuredBuffer* _Buffer)
{
	assert(_Buffer);

	m_LightShareBuffer = _Buffer;
	m_Param.iArr[0] = 1600 * 900;
}

void CSLight::UpdateData()
{
	assert(m_LightShareBuffer);

	m_LightShareBuffer->UpdateData_CS(0, false);

	Vec4 vData = {};
	m_LightShareBuffer->GetData(&vData);

	//100 / 128
	//1024 -> 2¹ø
	m_iGroupX = 2;
	m_iGroupY = 1;
	m_iGroupZ = 1;
}

void CSLight::Clear()
{
	m_LightShareBuffer->Clear();
}
