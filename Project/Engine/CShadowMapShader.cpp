#include "pch.h"
#include "CShadowMapShader.h"
#include "CDevice.h"


CShadowMapShader::CShadowMapShader()
	: CComputeShader(32, 32, 1)
{
}

CShadowMapShader::~CShadowMapShader()
{
}


void CShadowMapShader::UpdateData()
{
	m_Param.v2Arr[0] = Vec2{ 4096.f * 2.f, 4096.f * 2.f };
	m_pShadowValidTex->UpdateData_CS(0, false);
	m_pShwdowMapTex->UpdateData_CS(1, true);

	//100 / 128
	//1024 -> 2번
	// 그룹 수 계산
	m_iGroupX = m_pShadowValidTex->GetWidth() / m_iGroupPerThreadX + 1;
	m_iGroupY = m_pShadowValidTex->GetHeight() / m_iGroupPerThreadY + 1;
	m_iGroupZ = 1;
}

void CShadowMapShader::Clear()
{
	m_pShadowValidTex->Clear_CS(0, false);
	m_pShwdowMapTex->Clear_CS(1, true);
}
