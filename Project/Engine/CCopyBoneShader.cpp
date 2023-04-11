#include "pch.h"
#include "CCopyBoneShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

CCopyBoneShader::CCopyBoneShader()
	: CComputeShader(1024, 1, 1)
	, m_pSrcBuffer(nullptr)
	, m_pDestBuffer(nullptr)
{
}

CCopyBoneShader::~CCopyBoneShader()
{
}

void CCopyBoneShader::UpdateData()
{
	// 구조화버퍼 전달
	m_pSrcBuffer->UpdateData_CS(16, true);  // t16
	m_pDestBuffer->UpdateData_CS(0, false);	// u0

	// 그룹 수 계산
	int iBoneCount = m_Param.iArr[0];

	m_iGroupX = iBoneCount / m_iGroupPerThreadX + 1;
	m_iGroupY = 1;
	m_iGroupZ = 1;
}

void CCopyBoneShader::Clear()
{
	// 전달한 구조화버퍼 클리어	
	m_pSrcBuffer->Clear();
	m_pDestBuffer->Clear();
}