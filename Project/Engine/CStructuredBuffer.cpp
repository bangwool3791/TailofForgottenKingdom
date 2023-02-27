#include "pch.h"
#include "CStructuredBuffer.h"

#include "CDevice.h"

CStructuredBuffer::CStructuredBuffer()
	: m_tDesc{}
	, m_eType(SB_TYPE::SRV_ONLY)
	, m_iElementSize(0)
	, m_iElementCount(0)
	, m_iRecentRegisterNum(0)
	, m_iRecentRegisterNumRW(0)
{
}

CStructuredBuffer::CStructuredBuffer(const CStructuredBuffer& _origin)
{

}

CStructuredBuffer::~CStructuredBuffer()
{
}

int CStructuredBuffer::Create(UINT _iElementSize, UINT _iElementCount, SB_TYPE _eType, void* _pInitial, bool _CPUAccess)
{
	m_bCPU = _CPUAccess;
	m_SB = nullptr;
	m_SRV = nullptr;
	m_UAV = nullptr;

	m_WriteBuffer = nullptr;
	m_ReadBuffer = nullptr;

	m_eType = _eType;
	m_iElementSize = _iElementSize;
	m_iElementCount = _iElementCount;
		
	m_tDesc.ByteWidth = _iElementSize * _iElementCount;	// 버퍼 전체 크기
	m_tDesc.StructureByteStride = _iElementSize;		// 버퍼 요소 크기			
	m_tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 구조화 버퍼 추가 플래그 설정
	m_tDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tDesc.CPUAccessFlags = 0;

	if (SB_TYPE::SRV_ONLY == m_eType)
	{
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// Texture Register Binding		
	}
		
	// UnorderedAccess Flag 가 있으면 CPU 쓰기 옵션이 불가능
	else if (SB_TYPE::UAV_INC == m_eType)
	{
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;		
	}
	
	HRESULT hr = S_OK;

	if (nullptr == _pInitial)
	{
		hr = DEVICE->CreateBuffer(&m_tDesc, nullptr, m_SB.GetAddressOf());
	}
	else
	{
		D3D11_SUBRESOURCE_DATA tSub = {};
		tSub.pSysMem = _pInitial;

		hr = DEVICE->CreateBuffer(&m_tDesc, &tSub, m_SB.GetAddressOf());
	}
	//m_SB->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CStructuredBuffer::m_SB") - 1, "CStructuredBuffer::m_SB");
	// 버퍼 생성 실패
	if (FAILED(hr))
		return E_FAIL;


	// Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
		
	tSRVDesc.BufferEx.NumElements = _iElementCount;	
	tSRVDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFEREX;

	if (FAILED(DEVICE->CreateShaderResourceView(m_SB.Get(), &tSRVDesc, m_SRV.GetAddressOf())))
	{
		return E_FAIL;
	}
	//m_SRV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CStructuredBuffer::m_SRV") - 1, "CStructuredBuffer::m_SRV");
	if (SB_TYPE::UAV_INC == m_eType)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};

		tUAVDesc.Buffer.NumElements = _iElementCount;
		tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		if (FAILED(DEVICE->CreateUnorderedAccessView(m_SB.Get(), &tUAVDesc, m_UAV.GetAddressOf())))
		{
			return E_FAIL;
		}

		//m_UAV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CStructuredBuffer::m_UAV") - 1, "CStructuredBuffer::m_UAV");
	}

	// 쓰기 전용 버퍼
	if (_CPUAccess)
	{
		m_tDesc.ByteWidth = _iElementSize * _iElementCount;	// 버퍼 전체 크기
		m_tDesc.StructureByteStride = _iElementSize;		// 버퍼 요소 크기			
		m_tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 구조화 버퍼 추가 플래그 설정
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// Texture Register Binding	

		m_tDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_WriteBuffer.GetAddressOf())))
		{
			assert(nullptr);
			return E_FAIL;
		}
		//m_WriteBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CStructuredBuffer::m_WriteBuffer") - 1, "CStructuredBuffer::m_WriteBuffer");

		m_tDesc.Usage = D3D11_USAGE_DEFAULT;
		m_tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_ReadBuffer.GetAddressOf())))
		{
			assert(nullptr);
			return E_FAIL;
		}		
		//m_ReadBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CStructuredBuffer::m_ReadBuffer") - 1, "CStructuredBuffer::m_ReadBuffer");
	}




	return hr;
}

void CStructuredBuffer::SetData(void* _pSrc, UINT _iElementCount)
{
	// 공간이 모자라면 추가할당하면서 초기화한다.
	if (m_iElementCount < _iElementCount)
	{
		Create(m_iElementSize, _iElementCount, m_eType, _pSrc, m_bCPU);
	}

	// 공간이 충분하다면, 데이터 전송
	else
	{
		D3D11_MAPPED_SUBRESOURCE tMapSub = {};

		CONTEXT->Map(m_WriteBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tMapSub);
		memcpy(tMapSub.pData, _pSrc, m_iElementSize * _iElementCount);
		CONTEXT->Unmap(m_WriteBuffer.Get(), 0);

		// 쓰기버퍼 -> 메인버퍼
		CONTEXT->CopyResource(m_SB.Get(), m_WriteBuffer.Get());
	}
}

void CStructuredBuffer::GetData(void* _pDst, UINT _iSizeByte)
{
	assert(!(_iSizeByte != 0 && _iSizeByte < m_iElementSize * m_iElementCount));

	// 메인버퍼 -> 읽기버퍼
	CONTEXT->CopyResource(m_ReadBuffer.Get(), m_SB.Get());

	// 읽기버퍼 -> SystemMem
	D3D11_MAPPED_SUBRESOURCE tMapSub = {};

	CONTEXT->Map(m_ReadBuffer.Get(), 0, D3D11_MAP_READ, 0, &tMapSub);

	if (0 == _iSizeByte)
	{
		memcpy(_pDst, tMapSub.pData, m_iElementSize * m_iElementCount);
	}
	else
	{
		memcpy(_pDst, tMapSub.pData, _iSizeByte);
	}	

	CONTEXT->Unmap(m_ReadBuffer.Get(), 0);
}

void CStructuredBuffer::UpdateData(UINT _iRegisterNum, UINT _PipelineStage)
{
	m_iRecentRegisterNum = _iRegisterNum;

	if ((UINT)PIPELINE_STAGE::VS & _PipelineStage)
		CONTEXT->VSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::HS & _PipelineStage)
		CONTEXT->HSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::DS & _PipelineStage)
		CONTEXT->DSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::GS & _PipelineStage)
		CONTEXT->GSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::PS & _PipelineStage)
		CONTEXT->PSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
}

void CStructuredBuffer::UpdateData_CS(UINT _iRegisterNum, bool _bShaderRes)
{
	m_iRecentRegisterNumRW = _iRegisterNum;

	if (_bShaderRes)
	{
		CONTEXT->CSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
	else
	{
		UINT i = -1;
		CONTEXT->CSSetUnorderedAccessViews(_iRegisterNum, 1, m_UAV.GetAddressOf(), &i);
	}
}

void CStructuredBuffer::Clear()
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->VSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);

	
	CONTEXT->CSSetShaderResources(m_iRecentRegisterNumRW, 1, &pSRV);
	ID3D11UnorderedAccessView* pUAV = nullptr;
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(m_iRecentRegisterNumRW, 1, &pUAV, &i);
}
