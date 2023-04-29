#include "pch.h"
#include "CAnimation3DShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

CAnimation3DShader::CAnimation3DShader()
	:CComputeShader(256, 1, 1)
	, m_pFrameDataBuffer{}
	, m_pOffsetMatBuffer{}
	, m_pOutputBuffer{}
	, m_pBoneSocketBuffer{}
{
	
}

CAnimation3DShader::~CAnimation3DShader()
{
}


void CAnimation3DShader::UpdateData()
{
	//t16
	m_pFrameDataBuffer->UpdateData_CS(16, true);
	//t17
	m_pOffsetMatBuffer->UpdateData_CS(17, true);
	//u0
	m_pOutputBuffer->UpdateData_CS(0, false);
	m_pBoneSocketBuffer->UpdateData_CS(1, false);
	m_iGroupX = (m_Param.iArr[0] / m_iGroupPerThreadX) + 1;
	m_iGroupY = 1;
	m_iGroupZ = 1;
}

void CAnimation3DShader::Clear()
{
	// 전달한 구조화버퍼 클리어	
	if (nullptr != m_pFrameDataBuffer)
	{
		m_pFrameDataBuffer->Clear();
		m_pFrameDataBuffer = nullptr;
	}

	if (nullptr != m_pOffsetMatBuffer)
	{
		m_pOffsetMatBuffer->Clear();
		m_pOffsetMatBuffer = nullptr;
	}

	if (nullptr != m_pOutputBuffer)
	{
		m_pOutputBuffer->Clear();
		m_pOutputBuffer = nullptr;
	}

	if (nullptr != m_pBoneSocketBuffer)
	{
		m_pBoneSocketBuffer->Clear();
		m_pBoneSocketBuffer = nullptr;
	}
}