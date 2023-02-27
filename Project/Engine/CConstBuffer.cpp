#include "pch.h"
#include "CConstBuffer.h"

#include "CDevice.h"

CConstBuffer::CConstBuffer(CB_TYPE _CBType)
	: m_tDesc{}
	, m_eCBType(_CBType)
{
}

CConstBuffer::~CConstBuffer()
{

}


int CConstBuffer::Create(size_t _iBufferSize)
{
	// 상수버퍼 생성
	m_tDesc.ByteWidth = (UINT)_iBufferSize; // 16 의 배수로
	m_tDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_tDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = DEVICE->CreateBuffer(&m_tDesc, nullptr, m_CB.GetAddressOf());
	//m_CB->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CConstBuffer::m_CB") - 1, "CConstBuffer::m_CB");
	return hr;
}

void CConstBuffer::SetData(void* _pSrc, size_t _iSize)
{
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tSub);

	memcpy(tSub.pData, _pSrc, _iSize);

	CONTEXT->Unmap(m_CB.Get(), 0);
}

void CConstBuffer::SetData(void* _pSrc)
{
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tSub);

	memcpy(tSub.pData, _pSrc, m_tDesc.ByteWidth);

	CONTEXT->Unmap(m_CB.Get(), 0);
}

void* CConstBuffer::GetData()
{
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	void* _pSrc{};

	CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(_pSrc, tSub.pData, m_tDesc.ByteWidth);
	CONTEXT->Unmap(m_CB.Get(), 0);
	return _pSrc;
}

void CConstBuffer::UpdateData(UINT _iPipelineStage)
{
	if ((UINT)PIPELINE_STAGE::VS & _iPipelineStage)
		CONTEXT->VSSetConstantBuffers((UINT)m_eCBType, 1, m_CB.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::HS & _iPipelineStage)
		CONTEXT->HSSetConstantBuffers((UINT)m_eCBType, 1, m_CB.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::DS & _iPipelineStage)
		CONTEXT->DSSetConstantBuffers((UINT)m_eCBType, 1, m_CB.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::GS & _iPipelineStage)
		CONTEXT->GSSetConstantBuffers((UINT)m_eCBType, 1, m_CB.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::PS & _iPipelineStage)
		CONTEXT->PSSetConstantBuffers((UINT)m_eCBType, 1, m_CB.GetAddressOf());
}

void CConstBuffer::UpdateData_CS()
{
	CONTEXT->CSSetConstantBuffers((UINT)m_eCBType, 1, m_CB.GetAddressOf());
}

