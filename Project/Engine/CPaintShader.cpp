#include "pch.h"
#include "CPaintShader.h"

#include "CTexture.h"

CPaintShader::CPaintShader()
	:CComputeShader(32, 32, 1)
{
}

CPaintShader::~CPaintShader()
{
}

void CPaintShader::UpdateData()
{
	m_pTargetTex->UpdateData_CS(0, true);

	// 그룹 수 계산
	m_iGroupX = m_pTargetTex->GetWidth() / m_iGroupPerThreadX + 1;
	m_iGroupY = m_pTargetTex->GetHeight() / m_iGroupPerThreadY + 1;
	m_iGroupZ = 1;
}

void CPaintShader::Clear()
{
	CTexture::Clear_CS(0);
}

